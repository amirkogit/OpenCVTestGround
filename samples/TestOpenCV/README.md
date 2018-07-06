# TestOpenCV
Simple console based Qt program that tests if OpenCV installation is successful.
The application reads the image named 'stl_world_map.jpg' from the hard-coded folder location. If successful, it should display the image in the window.

```cpp
using namespace cv;
Mat image = imread("c:/dev/testimages/stl_world_map.jpg");
imshow("Output", image);
```
