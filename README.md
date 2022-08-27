## Android_Native_Surface


> This is a Demo for Android Native Surface <br>
> Made by SsageParuders(泓清)<br>
> Had supported for Android 10~12<br>

> If you had used code from this repository.<br>
> I hope you can show this repository's address.<br>
> Please maintain basic respect for open source.<br>
> 如果你有使用到这个仓库的代码<br>
> 希望你能标明仓库的开源地址<br>
> 请对开源保持最基本的尊重<br>

---

### libs

- All dynamic libraries can be used for NDK without AOSP environment.<br>
- All dynamic libraries are built from AOSP.<br>
- Just support for Android 10 ~ 12<br>

---

### res/aosp_native_surface

- Sources for AOSP
    
    - You can build dynamic libraries from AOSP,and use it for NDK

- How To Build 

    - See [README.md](./res/aosp_native_surface/README.md) for aosp_native_surface

---

### outputs 

> The out file for Demo.<br>
> Can be executed by 
>   ```bash
>     chmod +x NativeSurface
>     ./NativeSurface
>   ```
> Then you can see a surface without any Java Api<br>
> Had supported for Android 10 ~ 12

---

### src && include

> Example For How To Use This Project

#### How to build

##### First Way

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

##### Second way

- Open this Project with CLion
    
  - Then Make Sure Your Toolchains is right for NDK.
    <img width="681" alt="image" src="https://raw.githubusercontent.com/SsageParuders/RES/master/img/202208270243940.png">
  - Change The NDK PATH for [CMakeLists.txt](https://github.com/SsageParuders/Android_Native_Surface/blob/e578428618899ec3bb1f4dcebba985a731b04606/CMakeLists.txt#L8)
  - Just build it.

---

### ToDo List

- [ ] Hidden from recording screen.
- [ ] Hide from the screenshot.
- [ ] Update the touch for Native Surface

---

## LICENSE

MIT License

> MIT License is uncontrolled License<br>
> I really like Open Sources Project<br>
> But, please.<br>
> Don't change my signature for this Project if you had used it.<br>
> Please maintain basic respect<br>

> MIT 协议是一个不受约束的宽松协议 我很喜欢开源项目<br>
> 但是 如果你使用了我的项目 请最基本的 不要修改我的署名&签名<br>
> 请保持最基本的尊重<br>

## Donate

- USTD
  > TRC20 : TNsRvFsLZzbf5EL9buzpAKKJn757obstEa

- ZFB & WX

  <img width="170" alt="image" src="https://raw.githubusercontent.com/SsageParuders/RES/master/img/202208270304053.png">
  <img width="200" alt="image" src="https://raw.githubusercontent.com/SsageParuders/RES/master/img/202208270301440.png">

## Connect

[TG Chat Group](https://t.me/+9Q9_NQHuAotlMjFl)

[TG Channel](https://t.me/SsageParudersHack)
