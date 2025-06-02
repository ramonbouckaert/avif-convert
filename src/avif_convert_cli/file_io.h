//
// Created by Ramon on 02/06/2025.
//

#ifndef FILE_IO_H
#define FILE_IO_H
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

char *make_avif_path(const char *input_path);

int read_file(const char *path, uint8_t **out_data, size_t *out_size);

int write_file(const char *path, uint8_t const *data, size_t size);
#endif //FILE_IO_H
