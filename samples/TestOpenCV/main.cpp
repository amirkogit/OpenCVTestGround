#include <QCoreApplication>
#include "opencv2/opencv.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    using namespace cv;
    Mat image = imread("c:/dev/testimages/stl_world_map.jpg");
    imshow("Output", image);
    return a.exec();
}
