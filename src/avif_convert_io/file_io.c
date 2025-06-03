//
// Created by Ramon on 02/06/2025.
//
#include "file_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AVIF_EXTENSION ".avif"

AVIF_CONVERT_IO_API char *make_avif_path(const char *input_path) {
    if (!input_path) return NULL;

    const char *dot = strrchr(input_path, '.');
    const size_t base_len = dot ? (size_t)(dot - input_path) : strlen(input_path);
    const size_t ext_len = strlen(AVIF_EXTENSION);
    const size_t total_len = base_len + ext_len + 1;  // +1 for null terminator

    char *output_path = malloc(total_len);
    if (!output_path) return NULL;

    // Copy base path
    memcpy(output_path, input_path, base_len);
    // Append extension
    memcpy(output_path + base_len, AVIF_EXTENSION, ext_len + 1);  // includes null terminator

    return output_path;
}

AVIF_CONVERT_IO_API int read_file(const char *path, uint8_t **out_data, size_t *out_size) {
    FILE *f;
    portable_fopen(&f, path, "rb");
    if (!f) return 1;

    fseek(f, 0, SEEK_END);
    const long size = ftell(f);
    rewind(f);

    if (size < 0) {
        fclose(f);
        return 1;
    }

    uint8_t *data = malloc(size);
    if (!data) {
        fclose(f);
        return 1;
    }

    if (fread(data, 1, size, f) != (size_t) size) {
        fclose(f);
        return 1;
    }

    fclose(f);
    *out_data = data;
    *out_size = (size_t) size;
    return 0;
}

AVIF_CONVERT_IO_API int write_file(const char *path, uint8_t const *data, const size_t size) {
    FILE *f = NULL;
    const int err = portable_fopen(&f, path, "wb");
    if (f == NULL || err != 0) {
        fprintf(stderr, "Failed to open output file: %s\n", strerror(err));
        return 1;
    }
    fwrite(data, 1, size, f);
    fclose(f);
    printf("Wrote %s (%zu bytes)\n", path, size);
    return 0;
}