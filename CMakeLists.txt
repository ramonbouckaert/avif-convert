cmake_minimum_required(VERSION 3.21)
project(avifconvert C)
set(CMAKE_C_STANDARD 99)
set(VCPKG_LIBRARY_LINKAGE static)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
find_package(libavif CONFIG REQUIRED)
find_package(libheif CONFIG REQUIRED)
find_package(SPNG CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(WebP CONFIG REQUIRED)
find_package(libuv CONFIG REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})

# avifconvertcore

add_library(avifconvertcore SHARED
        src/avif_convert_core/common.h
        src/avif_convert_core/common.c
        src/avif_convert_core/input/loaders.h
        src/avif_convert_core/input/png_loader.c
        src/avif_convert_core/input/heif_loader.c
        src/avif_convert_core/output/avif_writer.c
        src/avif_convert_core/input/jpg_loader.c
        src/avif_convert_core/input/webp_loader.c
        src/avif_convert_core/input/loaders.h
        src/avif_convert_core/avif_convert.c
        src/avif_convert_core/avif_convert.h
)

target_compile_definitions(avifconvertcore PRIVATE AVIF_CONVERT_EXPORTS)

target_link_libraries(avifconvertcore PRIVATE
        avif
        heif
        JPEG::JPEG
        WebP::webp WebP::webpdecoder WebP::webpdemux
        $<IF:$<TARGET_EXISTS:spng::spng>,spng::spng,spng::spng_static>
)

# avifconvert-jni

add_library(avifconvert-jni SHARED
        src/avif_convert_jni/avif_convert_jni.h
        src/avif_convert_jni/avif_convert_jni.c
)

target_link_libraries(avifconvert-jni PUBLIC avifconvertcore)

# avifconvert-io
add_library(avifconvertio SHARED
        src/avif_convert_io/file_io.h
        src/avif_convert_io/file_io.c
)

target_compile_definitions(avifconvertio PRIVATE AVIF_CONVERT_IO_EXPORTS)

# avifconvert executable

add_executable(avifconvert src/avif_convert/main.c)

target_link_libraries(avifconvert PUBLIC avifconvertcore avifconvertio)

# avifconvertwatch executable

add_executable(avifconvertwatch src/avif_convert_watch/main.c
        src/avif_convert_watch/file_watcher.c
        src/avif_convert_watch/file_watcher.h
)

target_link_libraries(avifconvertwatch PUBLIC avifconvertcore avifconvertio
        PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
)

# Hacks

if(UNIX AND NOT APPLE)
    target_link_libraries(avifconvertcore PRIVATE stdc++)
endif()
