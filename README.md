# About this Project

This repository is a fork of the original Google's Skia library.
The goal was to use Skia in a browser environment (on the client-side). The existing 'skia/modules/canvaskit' module was a great starting point. However, after further investigation, it became clear that the original CanvasKit build didn't include the Skia PDF Backend features.
<br>
To solve this, we modified the library to add the necessary functionality.