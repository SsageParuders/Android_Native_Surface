cmake_minimum_required(VERSION 3.21)

##################### Android设置 #####################
set(CMAKE_SYSTEM_NAME ANDROID) # 设置目标编译平台参数 Android
set(CMAKE_SYSTEM_VERSION 21) # 系统版本
set(ANDROID_PLATFORM 21) # 平台版本
set(ANDROID_ABI arm64-v8a) # 设置目标构架 armeabi-v7a arm64-v8a x86 x86_64
set(ANDROID_NDK /Users/hongqing/Library/Android/sdk/ndk/23.1.7779620) # 设置ndk路径
set(CMAKE_TOOLCHAIN_FILE /Users/hongqing/Library/Android/sdk/ndk/23.1.7779620/build/cmake/android.toolchain.cmake) # 设置交叉编译链的cmake配置
##################### Android设置 #####################

project(NativeSurface)

set(CMAKE_CXX_STANDARD 17)

##################### 输出文件重定向 #####################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/outputs/${CMAKE_ANDROID_ARCH_ABI}/
        ) # 重定向输出产物(可执行文件)
##################### 输出文件重定向 #####################

##################### CMake头文件设置 #####################
FILE(GLOB_RECURSE FILE_INCLUDES # 遍历子目录下所有符合情况的头文件
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*
        )
include_directories( # 设置全局头文件目录 使其他源码文件可在任意目录<头文件.h>
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGui
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Android_touch
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Android_shm
        ${CMAKE_CURRENT_SOURCE_DIR}/include/native_surface
        )
##################### CMake头文件设置 #####################

##################### CMake源文件设置 #####################
FILE(GLOB_RECURSE FILE_SOURCES # 遍历子目录下所有符合情况的源文件
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
        )
##################### CMake源文件设置 #####################

##################### 设置三方库文件目录 #####################
#link_directories(${PROJECT_NAME} PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/
#        )
##################### 设置三方库文件目录 #####################

##################### 添加产物 #####################
add_executable(NativeSurface # 生成可执行文件
        ${FILE_INCLUDES} # 头文件
        ${FILE_SOURCES} # 源文件
        )
##################### 添加产物 #####################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue
        )
##################### 连接库文件 #####################
target_link_libraries(${PROJECT_NAME} PRIVATE
        EGL GLESv3 log android GLESv2 m dl
        )
##################### 连接库文件 #####################