//
// Created by Ssage on 2022/3/18.
//
#include "main.h"


/**
 * imgui测试
 */

int main(int argc, char *argv[]) {
    // 初始化imgui
    if (!initDraw(true)) {
        return -1;
    }
    Init_touch_config();
    printf("Pid is %d\n", getpid());
    bool flag = true;
    while (flag) {
        // imgui画图开始前调用
        drawBegin();
        static bool show_demo_window = false;
        static bool show_another_window = false;

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        { // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
            static float f = 0.0f;
            static int counter = 0;
            static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.
            ImGui::Text(
                    "This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit4("clear color", (float *) &clear_color); // Edit 3 floats representing a color
            if (ImGui::Button("Button")) {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("IsWindowFocused = %d", ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow));
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            if (ImGui::Button("exit")) {
                flag = false;
            }
            ImGui::End();
        }

        if (show_another_window) { // 3. Show another simple window.
            ImGui::Begin("Another Window",
                         &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) {
                show_another_window = false;
            }
            ImGui::End();
        }
        // imgui画图结束调用
        drawEnd();
        std::this_thread::sleep_for(1ms);
    }
    shutdown();
    touchEnd();
    return 0;
}

