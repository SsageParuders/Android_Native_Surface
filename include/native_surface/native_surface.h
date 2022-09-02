//
// Created by Ssage on 2022/6/29.
//

#ifndef NATIVESURFACE_NATIVE_SURFACE_H
#define NATIVESURFACE_NATIVE_SURFACE_H
// System libs
#include <iostream>
#include <cstdlib>
#include <android/api-level.h>
// User libs
#include "utils.h"
#include "aosp_11/native_surface_11.h"
#include "aosp_10/native_surface_10.h"
#include "aosp_12/native_surface_12.h"
void* get_createNativeWindow();
#endif //NATIVESURFACE_NATIVE_SURFACE_H
