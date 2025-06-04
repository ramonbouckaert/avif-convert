# avifconvert
The command line tool `avifenc` provided by [`libavif`](https://github.com/AOMediaCodec/libavif) is limited. It can only encode AVIF files from supplied PNG, JPEG and raw YUV data files, with no support for HEIC or WebP files.

This library uses [`libavif`](https://github.com/AOMediaCodec/libavif) to encode AVIF files, but provides an interface for encoding AVIF files from a wider range of source formats. Currently supported input formats:
- PNG (using [`libspng`](https://github.com/randy408/libspng))
- JPEG (using [`libjpeg-turbo`](https://github.com/libjpeg-turbo/libjpeg-turbo))
- HEIC (using [`libheif`](https://github.com/strukturag/libheif))
- WebP (using [`libwebp`](https://chromium.googlesource.com/webm/libwebp/))

Just like `avifenc`, `avifconvert` handles EXIF and XMP metadata properly.

Alongside `avifconvert`, this project also provides a `avifconvertwatch` tool that can be used to watch a directory for new files and convert them to AVIF format automatically. This is useful for integrating into workflows where images are added to a directory. I use it on my NAS along with [PhotoSync](https://www.photosync-app.com/) on my phone to convert incoming images after upload.

In addition, JNI binding are provided, but they are untested.

This is not a drop-in replacement for `avifenc`, nor is it a very mature project. Many things won't work. Use at your discretion.

## Install
Pre-built binaries are available for download on the [releases page]("/releases") for the following platforms:
- Linux (x86_64 and arm64)
- Windows (x86_64)

Otherwise, you can build from source as described below.

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
- `-v` or `--version` with no other arguments will print the version of the tool.

```bash
./avifconvertwatch /path/to/my/dir
```
This will run continuously, watching for new convertible files in that directory, or subdirectories up to one level deep, converting to AVIF and deleting the original file.
Options:
- `-v` or `--version` with no other arguments will print the version of the tool.

## TODO
- Support Y4M files, like the original `avifenc`
- Support GIF files, including animated GIFs.
- Support BMP files
- Support TIFF files
- Support JPEG XL files
- Support `avifconvertwatch` with unlimited nested subdirectories, currently it only watches one subdirectory deep.
- Get static linking working on all platforms
- Add tests
