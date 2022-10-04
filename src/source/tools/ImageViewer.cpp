//
// Created by fgsqme on 2022/10/3.
//

#include "ImageTexture.h"
#include "ImageViewer.h"
#include <imgui.h>
#include <touch.h>
#include <draw.h>

ImageViewer::ImageViewer() {
    if (!initDraw(false)) {
        return;
    }
    Init_touch_config();
    gain = 1.0f;
}

float ImageViewer::getGain() {
    return gain;
}


void ImageViewer::imshow(string frame_name, cv::Mat &frame) {
    frame_names.push_back(frame_name);
    frames.push_back(&frame);
}

void ImageViewer::imshow(cv::Mat &frame) {
    imshow("image:" + to_string(frames.size()), frame);
}

void ImageViewer::showMainContents() {
    ImGui::Begin("Main");
    ImGui::SliderFloat("gain", &gain, 0.0f, 2.0f, "%.3f");
    ImGui::Text("IsWindowFocused = %d", ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow));
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::Text("height: %.0f width: %.0f", ImGui::GetWindowHeight(), ImGui::GetWindowWidth());
    ImGui::End();
}

void ImageViewer::show() {
    drawBegin();
    showMainContents();
    // initialize textures
    vector<ImageTexture *> my_textures;
    for (int i = 0; i < frames.size(); i++) {
        my_textures.push_back(new ImageTexture());
    }

    // imshow windows
    for (int i = 0; i < frames.size(); i++) {
        cv::Mat *frame = frames[i];
        string window_name;
        if (frame_names.size() <= i) {
            window_name = "image:" + to_string(i);
        } else {
            window_name = frame_names[i];
        }
        ImGui::Begin(window_name.c_str());
        my_textures[i]->setBuffer(frame->data, frame->cols, frame->rows);
        ImVec2 imVec2 = ImVec2((float) frame->cols, (float) frame->rows);
        ImGui::Image((ImTextureID) my_textures[i]->getOpenglTexture(), imVec2);
        ImGui::End();
    }

    drawEnd();
    // clear resources
    for (int i = 0; i < frames.size(); i++) {
        delete my_textures[i];
    }

    frame_names.clear();
    frames.clear();
    my_textures.clear();
}

void ImageViewer::shutdown() {
    ::shutdown();
}