cmake_minimum_required(VERSION 3.21)
project(avif-convert C)
set(CMAKE_C_STANDARD 99)
set(VCPKG_LIBRARY_LINKAGE static)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
find_package(libavif CONFIG REQUIRED)
find_package(libheif CONFIG REQUIRED)
find_package(SPNG CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(WebP CONFIG REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})

add_library(avif_convert SHARED
        src/avif_convert/common.h
        src/avif_convert/common.c
        src/avif_convert/input/loaders.h
        src/avif_convert/input/png_loader.c
        src/avif_convert/input/heif_loader.c
        src/avif_convert/output/avif_writer.c
        src/avif_convert/input/jpg_loader.c
        src/avif_convert/input/webp_loader.c
        src/avif_convert/input/loaders.h
        src/avif_convert/avif_convert.c
        src/avif_convert/avif_convert.h
)

target_compile_definitions(avif_convert PRIVATE AVIF_CONVERT_EXPORTS)

target_link_libraries(avif_convert PRIVATE
        avif
        heif
        JPEG::JPEG
        WebP::webp WebP::webpdecoder WebP::webpdemux
        $<IF:$<TARGET_EXISTS:spng::spng>,spng::spng,spng::spng_static>
)

add_library(avif_convert_jni SHARED
        src/avif_convert_jni/avif_convert_jni.h
        src/avif_convert_jni/avif_convert_jni.c
)

target_link_libraries(avif_convert_jni PUBLIC avif_convert)

add_executable(avif-convert-cli src/avif_convert_cli/main.c
        src/avif_convert_cli/file_io.h
        src/avif_convert_cli/file_io.c
)

target_link_libraries(avif-convert-cli PUBLIC avif_convert)

if(UNIX AND NOT APPLE)
    target_link_libraries(avif_convert PRIVATE stdc++)
endif()

if (WIN32)
    set(VCPKG_BIN_DIR "vcpkg_installed/x64-windows/bin")
    add_custom_command(TARGET avif-convert-cli POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VCPKG_BIN_DIR}/aom.dll"
            "${VCPKG_BIN_DIR}/avif.dll"
            "${VCPKG_BIN_DIR}/heif.dll"
            "${VCPKG_BIN_DIR}/spng.dll"
            "${VCPKG_BIN_DIR}/jpeg62.dll"
            "${VCPKG_BIN_DIR}/libwebp.dll"
            "${VCPKG_BIN_DIR}/libwebpdecoder.dll"
            "${VCPKG_BIN_DIR}/libwebpdemux.dll"
            "${VCPKG_BIN_DIR}/libyuv.dll"
            "${VCPKG_BIN_DIR}/libsharpyuv.dll"
            "${VCPKG_BIN_DIR}/libx265.dll"
            "${VCPKG_BIN_DIR}/libde265.dll"
            $<TARGET_FILE_DIR:avif-convert-cli>
    )
endif()