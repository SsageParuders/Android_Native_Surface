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
#include <iostream>
#include <sys/system_properties.h>
// User libs
#include <shm_open_anon.h>
void* fdlopen(int fd,int mode){
    char BUF[PATH_MAX];
    if(isatty(fd)) {
        errno = EBADFD;
        return NULL;
    }
    if(access("/proc",F_OK)<0){
        errno = ENOSYS;
        return NULL;
    }
    snprintf(BUF,PATH_MAX,"/proc/self/fd/%d",fd);
    if(access(BUF,F_OK)<0){
        errno = EBADF;
        return NULL;
    }else if(access(BUF,R_OK)<0){
        errno = EACCES;
        return NULL;
    }
    struct stat st;
    stat(BUF,&st);
    if(S_ISDIR(st.st_mode)){
        errno = EISDIR;
        return NULL;
    }
    return dlopen(BUF,mode);
}
void *dlblob(const void *blob, size_t len) {
    int fd = shm_open_anon();
    ftruncate(fd, len);
    void *mem = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(mem, blob, len);
    munmap(mem, len);
    void *so = fdlopen(fd,RTLD_LAZY);
    close(fd);
    return so;
}
int get_android_api_level(){
    char prop_value[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.sdk", prop_value);
    return atoi(prop_value);
}
#endif //NATIVESURFACE_UTILS_H
