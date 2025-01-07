# About this Project

This repository is a fork of the original Google's Skia library. <br>
<br>
The goal was to use Skia in a browser environment (on the client-side). The existing 'skia/modules/canvaskit' module was a great starting point. However, after further investigation, it became clear that the original CanvasKit build didn't include the Skia PDF Backend features.<br>
<br>
To solve this, we modified the library to add the necessary functionality.

# Changes Made:
- Disabled the `Skottie` animation module in the build scripts.
- Added a new function `SkCanvas.getSizes()` to retrieve the canvas size.
- Added a new function `canvaskit.ConvertToPDF(SkPicture, width, height)` for generating PDF files from Skia pictures.

# How to use at JS/TS project
```ts
// Define the capture bounds
const sizes = MyOriginalSkiaCanvas.getSizes();
const bounds: Rect = canvasKit.LTRBRect(0, 0, sizes.width, sizes.height);

// Create a PictureRecorder object to record drawing commands
const recorder = new canvasKit.PictureRecorder();

// Begin recording, this returns a canvas where drawing operations will be recorded.
// This canvas will not be the same as the main canvas where the primary drawing occurs,
// but rather an auxiliary canvas where the drawing commands are recorded.
const captureCanvas = recorder.beginRecording(bounds);

// (Add Skia/CanvasKit drawing commands here, e.g., drawing a rectangle, etc.)
// For example somethink like:
MyOriginalSkiaCanvas.drawPath(path, fillPaint);
captureCanvas.drawPath(path, fillPaint);

// Finish recording and get the sequence of drawing commands and transformations as SkPicture
const capture: SkPicture = recorder.finishRecordingAsPicture();

// The ConvertToPDF function returns a Uint8Array stream which can be immediately converted to Blob
const fileUint8Array = canvasKit.ConvertToPDF(capture, sizes.width, sizes.height);
const pdfStream = new Blob([fileUint8Array], { type: 'application/pdf' });

// Convert the Blob into a URL for downloading
const pdfUrl = URL.createObjectURL(pdfStream);

const fileName = `exported_skia_canvas.pdf`;

// Create a download link and trigger the download of the PDF file
const downloadLink          = document.createElement('a');
      downloadLink.href     = pdfUrl;
      downloadLink.download = fileName;
      downloadLink.click();
```

# Files Affected:
- [modules/canvaskit/canvaskit_bindings.cpp](https://github.com/theManAndHisShadow/skia-browser-pdf/blob/main/modules/canvaskit/canvaskit_bindings.cpp)
- [modules/canvaskit/canvaskit_extended_bindings.cpp](https://github.com/theManAndHisShadow/skia-browser-pdf/blob/main/modules/canvaskit/canvaskit_extended_bindings.cpp)
- [modules/canvaskit/BUILD.gn](https://github.com/theManAndHisShadow/skia-browser-pdf/blob/main/modules/canvaskit/BUILD.gn)
- [modules/canvaskit/compile.sh](https://github.com/theManAndHisShadow/skia-browser-pdf/blob/main/modules/canvaskit/compile.sh)
- [modules/canvaskit/compile_gm.sh](https://github.com/theManAndHisShadow/skia-browser-pdf/blob/main/modules/canvaskit/compile_gm.sh)
