# How To Build it
- Build AOSP, first.
    ```shell
    source build/envsetup.sh
    lunch 2
    # choice which you need
    m -j8
    ```
- Move sources to `AOSP/external`.
    ```shell
  mv aosp_native_surface path/to/AOSP/external/
  ```
- Just build this module
    ```shell
  mmm external/aosp_native_surface
    ```
- Find target on out folder
    ```shell
    find out -name "*Ssage.*"
    ```