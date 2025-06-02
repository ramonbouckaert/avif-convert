# avif-convert
The command line tool `avifenc` provided by [`libavif`](https://github.com/AOMediaCodec/libavif) is limited. It can only encode AVIF files from supplied PNG, JPEG and raw YUV data files, with no support for HEIC or WebP files.

This library uses `libavif` to encode AVIF files, but provides an interface for encoding AVIF files from a wider range of source formats. Currently supported input formats:
- PNG (using `libspng`)
- JPEG (using `libjpeg-turbo`)
- HEIC (using `libheif`)
- WebP (using `libwebp`)

Just like `avifenc`, `avif-convert` handles EXIF and XMP metadata properly.

## Build
This project uses [cmake](https://cmake.org/) and [vcpkg](https://vcpkg.io/). The dependencies for the project are described in [vcpkg.json](vcpkg.json).

Install cmake if necessary:
```bash
apt install cmake # or alternatives, depending on your operating system.
```

Install an instance of vcpkg if necessary:
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh # or ./bootstrap-vcpkg.bat on windows
```

Back in the repository root directory, configure cmake with the vcpkg toolchain:
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake # or the path to your existing vcpkg instance
```

Build:
```bash
cmake --build build --config release
```

The resulting binaries will be found in a `build/` or `build/Release/` folder depending on your cmake toolchain.

## Usage
```bash
./avif-convert-cli my-image.webp
```
This will create a file called `my-image.avif` in the working directory.

## TODO
- Support Y4M files, like the original `avifenc`
- Support GIF files, including animated GIFs.
- Support BMP files
- Support TIFF files
- Support JPEG XL files
- Add tests