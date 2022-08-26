//
// Created by Ssage on 2022/6/29.
//

#ifndef NATIVESURFACE_UTILS_H
#define NATIVESURFACE_UTILS_H
// System libs
#include <dlfcn.h>
#include <cerrno>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include <sys/system_properties.h>
// User libs
#include <shm_open_anon.h>
using namespace std;
string exec_native_surface(string command);
void* fdlopen(int fd,int mode);
void *dlblob(const void *blob, size_t len);
int get_android_api_level();
#endif //NATIVESURFACE_UTILS_H
