INCLUDEPATH += c:/dev/opencv/build/install/include
Debug: {
	LIBS += -lc:/dev/opencv/build/install/x86/vc14/lib/opencv_world330d
}
Release: {
	LIBS += -lc:/dev/opencv/build/install/x86/vc14/lib/opencv_world330
}
