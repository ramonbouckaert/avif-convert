//
// Created by Ramon on 02/06/2025.
//

#ifndef FILE_IO_H
#define FILE_IO_H
#ifdef __cplusplus
extern "C" {
#endif
#if defined(_WIN32) && defined(AVIFCONVERTIO_SHARED)
    #ifdef AVIFCONVERTIO_EXPORTS
        #define AVIF_CONVERT_IO_API __declspec(dllexport)
    #else
        #define AVIF_CONVERT_IO_API __declspec(dllimport)
    #endif
#else
    #define AVIF_CONVERT_IO_API
#endif
#ifdef _MSC_VER
// Microsoft compiler has fopen_s
#define portable_fopen(fp, filename, mode) fopen_s(fp, filename, mode)
#else
    // POSIX-compatible systems use fopen with error check
    #include <string.h>  // for strerror
    #include <stdio.h>
    #include <errno.h>

    static inline int portable_fopen(FILE** fp, const char* filename, const char* mode) {
        *fp = fopen(filename, mode);
        return (*fp == NULL) ? errno : 0;
    }
#endif
#include <stdint.h>

AVIF_CONVERT_IO_API char *make_avif_path(const char *input_path);

AVIF_CONVERT_IO_API int read_file(const char *path, uint8_t **out_data, size_t *out_size);

AVIF_CONVERT_IO_API int write_file(const char *path, uint8_t const *data, size_t size);

AVIF_CONVERT_IO_API int copy_file_times(const char *src, const char *dst);

#ifdef __cplusplus
}
#endif
#endif //FILE_IO_H
