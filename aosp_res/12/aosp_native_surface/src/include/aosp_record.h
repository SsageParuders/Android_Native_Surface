/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SCREENRECORD_SCREENRECORD_H
#define SCREENRECORD_SCREENRECORD_H
#include <utils/Errors.h>

#define kVersionMajor 1
#define kVersionMinor 3
using android::status_t;
int initScreenrecord(const char* bitRate,float fps,uint32_t videoWidth, uint32_t videoHeight);
void stopScreenrecord();
ANativeWindow *getRecordWindow();
status_t runEncoder(bool *runFlag,void callback(uint8_t*,size_t));
#endif /*SCREENRECORD_SCREENRECORD_H*/
