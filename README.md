# OpenCVTestGround
Sandbox for trying out different stuffs in OpenCV. This repository consists of small projects written in Qt and integrating it with OpenCV.

## Steps to build OpenCV in Windows
1. Download sources from the following link:
https://opencv.org/releases.html

2. Create a folder C:\dev\opencv
Extract sources to this folder.

3. Create 'build' folder as:
C:\dev\opencv\build

4. Run CMake GUI and configure as:
```
Where is the source code: c:\dev\opencv
Where to build the binaries: C:\dev\opencv\build
```

5. Select Visual Studio 14 2015. This will build for 32 bit. 

6. Check the check box next to the BUILD_opencv_world option that allows building OpenCV modules into a single library.

7. Click Configure

8. Click Generate

9. In C:\dev\opencv\build folder, find the Visual Studio Solution

10. In Visual Studio, select the menu Build/Batch Build. Select options for ALL_BUILD and INSTALL

11. Configuring OpenCV installation
Following folder should have been created if the build is successful:
c:\dev\opencv\build\install\x86\vc14\
  bin
  lib

12. Create a file opencv.pri and copy in C:\dev\opencv\opencv.pri

```
INCLUDEPATH += c:/dev/opencv/build/install/include
Debug: {
	LIBS += -lc:/dev/opencv/build/install/x86/vc14/lib/opencv_world341d
}
Release: {
	LIBS += -lc:/dev/opencv/build/install/x86/vc14/lib/opencv_world41
}
```

This file can be used in Qt projects to link to OpenCV. Note the number like 341d or 341. This may vary depending on which version of OpenCV is built. Change this number accordingly otherwise we may get a linker error when building Qt and OpenCV programs.

13. Set PATH environment variables. Open System Properties window and add the following to PATH
```
c:\dev\opencv\build\install\x86\vc14\bin
c:\dev\opencv\build\install\x86\vc14\lib
```

