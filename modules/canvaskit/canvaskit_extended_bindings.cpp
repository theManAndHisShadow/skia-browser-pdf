/*
 * Copyright 2025 theManAndHisShadow
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <chrono>
#include <vector>
#include <emscripten/bind.h>
#include "include/core/SkCanvas.h"
#include "include/core/SkPicture.h"
#include "include/core/SkStream.h"
#include "include/docs/SkPDFDocument.h"

using namespace emscripten;

// Возвращает текущее время в формате SkPDF::DateTime { год, месяц, день недели, час, минуты, секунды }
SkPDF::DateTime getCurrentTime() {
    SkPDF::DateTime dt;

    // Получаем текущее время
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto localTime = *std::localtime(&in_time_t);

    // Заполняем структуру SkPDF::DateTime
    dt.fYear = localTime.tm_year + 1900;    // Год
    dt.fMonth = localTime.tm_mon + 1;       // Месяц (1-12)
    dt.fDayOfWeek = localTime.tm_wday;      // День недели (0-6, где 0 — воскресенье)
    dt.fDay = localTime.tm_mday;            // День месяца (1-31)
    dt.fHour = localTime.tm_hour;           // Час (0-23)
    dt.fMinute = localTime.tm_min;          // Минуты (0-59)
    dt.fSecond = localTime.tm_sec;          // Секунды (0-59)

    // Временная зона (в минутах)
    time_t rawTime;
    struct tm timeInfo;
    time(&rawTime);
    localtime_r(&rawTime, &timeInfo);
    dt.fTimeZoneMinutes = timeInfo.tm_gmtoff / 60;  // Временная зона в минутах

    return dt;
}

// Конвертирует SkPicture в PDF документ с заданной шириной и высотой страницы 
void ConvertToPDF(const sk_sp<SkPicture>& picture, std::vector<uint8_t>& outputBuffer, int width, int height) {
    // Проверяем что picture существует и не пустое
    if (picture == nullptr) {
        fprintf(stderr, "Error: SkPicture is null\n");
        return;
    }

    // Создаем выходной поток в памяти
    SkDynamicMemoryWStream memoryStream;

    // Записываем метаданные PDF-документа
    SkPDF::Metadata metadata;
    metadata.fTitle    = "Skia Canvas";                   // Указываем, что PDF страница создагна на основе Skia Canvas (SkCanvas)
    metadata.fCreator  = "Skia/CanvasKit PDF Export";     // Указываем название библиотеки/модуля как создатель данного файла
    metadata.fCreation = getCurrentTime();                // Записываем время создания 
    metadata.fModified = metadata.fCreation;              // Записываем время модифицкации

    // Создаем непосредственно сам PDF-документ используя встроенный метод
    auto pdfDocument = SkPDF::MakeDocument(&memoryStream, metadata);

    // Создаем страницу документа
    SkCanvas* pageCanvas = pdfDocument->beginPage(width, height);

    // Переносим содержимое picture на страницу документа
    // N.B: Так как picture в нашем случае был создан через захват команд отрисовки через 'beginRecording()' (см. подробнее внутри `pixi2skia()`)
    //      то все вызовы отрисовки силами самого Skia будут интерпретированы как векторные команды, а равно качество результата будет высоким,
    //      однако, если на странице была текстура (спррайт), то она будете перенесена как растровое изображение
    pageCanvas->drawPicture(picture);

    // Завершаем страницу
    pdfDocument->endPage();

    // Закрываем документ
    pdfDocument->close();

    // Копируем данные из потока в вектор
    size_t streamSize = memoryStream.bytesWritten();
    outputBuffer.resize(streamSize);
    memoryStream.copyTo(outputBuffer.data());
}

// Промежуточная функция для биндинга 
emscripten::val ConvertToPDFWrapper(const sk_sp<SkPicture>& picture, int width, int height) {
    std::vector<uint8_t> outputBuffer;
    ConvertToPDF(picture, outputBuffer, width, height); // Вызов функции, которая создаёт PDF

    // Возвращаем данные в виде Uint8Array
    return emscripten::val(emscripten::typed_memory_view(outputBuffer.size(), outputBuffer.data())); 
}

// Регистрация функции на стороне 'canvasKit.js'
EMSCRIPTEN_BINDINGS(canvaskit) {
    emscripten::function("ConvertToPDF", &ConvertToPDFWrapper);
}