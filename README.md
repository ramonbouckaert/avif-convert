# avifconvert
The command line tool `avifenc` provided by [`libavif`](https://github.com/AOMediaCodec/libavif) is limited. It can only encode AVIF files from supplied PNG, JPEG and raw YUV data files, with no support for HEIC or WebP files.

This library uses [`libavif`](https://github.com/AOMediaCodec/libavif) to encode AVIF files, but provides an interface for encoding AVIF files from a wider range of source formats. Currently supported input formats:
- PNG (using [`libspng`](https://github.com/randy408/libspng))
- JPEG (using [`libjpeg-turbo`](https://github.com/libjpeg-turbo/libjpeg-turbo))
- HEIC (using [`libheif`](https://github.com/strukturag/libheif))
- WebP (using [`libwebp`](https://chromium.googlesource.com/webm/libwebp/))

Just like `avifenc`, `avifconvert` handles EXIF and XMP metadata properly.

This is not a drop-in replacement for `avifenc`, nor is it a very mature project. Many things won't work. Use at your discretion.

## Build
This project uses [cmake](https://cmake.org/) and [vcpkg](https://vcpkg.io/), as well as Java for JNI bindings. Ensure cmake and Java are installed on your system before building. The dependencies for the project are described in [vcpkg.json](vcpkg.json).

Run this after cloning to fetch and initialise the vcpkg submodule:
```bash
git submodule update --init --recursive
vcpkg/bootstrap-vcpkg.sh # or vcpkg/bootstrap-vcpkg.bat on windows
```

Configure and build with cmake:
```bash
cmake --preset Release && cmake --build --preset Release
```

The resulting binaries will be found in a `build-release/` or `build-release/Release/` folder depending on your cmake toolchain.

## Usage
```bash
./avifconvert my-image.webp
```
This will create a file called `my-image.avif` in the same directory.
Options:
- `-d` or `--delete-original` will delete the original file, replacing it with the AVIF file.

```bash
./avifconvertwatch /path/to/my/dir
```
This will run continuously, watching for new convertible files in that directory, or subdirectories up to one level deep. If it finds a convertible file, it will convert it to AVIF format and delete the original.

## TODO
- Support Y4M files, like the original `avifenc`
- Support GIF files, including animated GIFs.
- Support BMP files
- Support TIFF files
- Support JPEG XL files
- Add tests
