#include <iostream>
#include <string>
#include <thread>
#include <opencv2/opencv.hpp>
#include "ImageViewer.h"

using namespace std;
using namespace cv;

/**
 * opencv测试
 * @return
 */
int main(int, char **) {
    ImageViewer gui;
    cv::VideoCapture video("/sdcard/a.mp4");

    if (!video.isOpened()) {
        cout << "打开失败" << endl;
        return -1;
    }

    double count = video.get(CAP_PROP_FRAME_COUNT);
    double rate = video.get(CAP_PROP_FPS);

    int width = (int) video.get(CAP_PROP_FRAME_WIDTH);
    int height = (int) video.get(CAP_PROP_FRAME_HEIGHT);

    cout << "帧率为:" << " " << std::to_string(rate) << endl;
    cout << "总帧数为:" << " " << std::to_string(count) << endl;//输出帧总数
    cout << "width:" << " " << std::to_string(width) << endl;//输出帧总数
    cout << "height:" << " " << std::to_string(height) << endl;//输出帧总数

    cv::Mat frame, frame2, img;

//    img = cv::imread("/sdcard/b.jpg");
//    cv::resize(img, img, cv::Size(0, 0), 0.5, 0.5, cv::INTER_LINEAR);

    while (video.read(frame)) {

//        cv::resize(frame, frame, cv::Size(0, 0), 1.5, 1.5, cv::INTER_LINEAR);
        float g = gui.getGain();
        cv::resize(frame, frame2, cv::Size(0, 0), 0.5, 0.5, cv::INTER_LINEAR);

        frame2.convertTo(frame2, CV_8U, g, 0);
        // show halfsize image
//        cv::resize(frame, frame2, cv::Size(0, 0), 0.5, 0.5, cv::INTER_LINEAR);

        gui.imshow("video", frame2);
//        gui.imshow("img", &img);
        // make quartersize image and show
//            gui.imshow("quater", &frame2);
        gui.show();
        std::this_thread::sleep_for(1ms);
//            if (cv::waitKey(20) >= 0) {
//            }
    }
    video.release();
    gui.shutdown();

    return 0;
}
