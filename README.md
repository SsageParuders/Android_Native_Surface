## Android_Native_Surface

> This is a Demo for Android Native Surface <br>
> Made by SsageParuders(泓清)<br>
> Had supported for Android 10~12

### libs

- All dynamic libraries can be used for NDK without AOSP environment.<br>
- All dynamic libraries are built from AOSP.<br>
- Just support for Android 10 ~ 12<br>

### res/aosp_native_surface

- Sources for AOSP
    
    - You can build dynamic libraries from AOSP,and use it for NDK

- How To Build 

    - See [README.md](./res/aosp_native_surface/README.md) for aosp_native_surface

### outputs 

> The out file for Demo.<br>
> Can be executed by 
>   ```bash
>     chmod +x NativeSurface
>     ./NativeSurface
>   ```
> Then you can see a surface without any Java Api<br>
> Had supported for Android 10 ~ 12

### src && include

> Example For How To Use This Project

#### How to build

- By command line with CMake and Ninja

    > Change The NDK PATH for [CMakeLists.txt](https://github.com/SsageParuders/Android_Native_Surface/blob/e578428618899ec3bb1f4dcebba985a731b04606/CMakeLists.txt#L8)<br>
      Make sure you can use cmake and ninja on your path.

    ```bash
    mkdir build
    cd build
    cmake ../
    cmake --build .
  ```

   <img width="681" alt="image" src="https://raw.githubusercontent.com/SsageParuders/RES/master/img/202208270207345.png">

- Open this Project with CLion
    
  - Then Make Sure Your Toolchains is right for NDK.
  - Change The NDK PATH for [CMakeLists.txt](https://github.com/SsageParuders/Android_Native_Surface/blob/e578428618899ec3bb1f4dcebba985a731b04606/CMakeLists.txt#L8)
  - Just build it.

### ToDo List

- [ ] Hidden from recording screen.
- [ ] Hide from the screenshot.
- [ ] Update the touch for Native Surface