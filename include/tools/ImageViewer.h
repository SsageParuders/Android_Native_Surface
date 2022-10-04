//
// Created by fgsqme on 2022/10/3.
//

#ifndef NATIVESURFACE_IMAGEVIEWER_H
#define NATIVESURFACE_IMAGEVIEWER_H

#include <opencv2/opencv.hpp>
#include <string>

using namespace std;

class ImageViewer {
private:

    bool log = true;
    // dynamic contents
    vector<string> frame_names;
    vector<cv::Mat *> frames;
    float gain;

    void showMainContents();

public:
    ImageViewer();

    void imshow(string, cv::Mat &frame);

    void imshow(cv::Mat &frame);

    void show();

    float getGain();

    void shutdown();
};

#endif //NATIVESURFACE_IMAGEVIEWER_H
