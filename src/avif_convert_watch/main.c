#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../avif_convert_io/file_io.h"

#include "file_watcher.h"
#include "../avif_convert_core/avif_convert.h"

#define RETRY(max_attempts, expr) \
    ({ int __attempts = 0, __result; \
    do { \
        __result = (expr); \
        if (__result != 0 && ++__attempts < (max_attempts)) sleep(1); \
    } while (__result != 0 && __attempts < (max_attempts)); \
    __result; })

void file_change_handler(const char *input_path) {
    // Read file
    uint8_t *file_data = NULL;
    size_t file_size;
    if (RETRY(3, read_file(input_path, &file_data, &file_size)) != 0) {
        fprintf(stderr, "Failed to read input file\n");
        return;
    }

    if (is_convertible(file_data, file_size) != 1) {
        free(file_data);
        return;
    }

    // Convert to AVIF
    uint8_t *output_data = NULL;
    size_t output_size = 0;
    const int convert_result = convert_to_avif(file_data, file_size, &output_data, &output_size);
    free(file_data);
    if (convert_result != 0) {
        fprintf(stderr, "Failed to convert file to avif: %s\n", input_path);
        free(output_data);
        return;
    }

    // Write to new file
    char *output_path = make_avif_path(input_path);
    const int write_result = RETRY(3, write_file(output_path, output_data, output_size));
    free(output_data);
    if (write_result != 0) {
        fprintf(stderr, "Failed to write to output file: %s\n", output_path);
        free(output_path);
        return;
    }

    // Copy date created/date modified information from the input file to the output file
    if (RETRY(3, copy_file_times(input_path, output_path)) != 0) {
        fprintf(stderr, "Failed to copy file metadata: %s to %s\n", input_path, output_path);
        free(output_path);
        return;
    }

    // Delete the original file
    if (RETRY(3, remove(input_path)) != 0) {
        fprintf(stderr, "Failed to delete original file: %s\n", input_path);
    }
    free(output_path);
}

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    const char *root = argv[1];
    if (!is_directory(root)) {
        fprintf(stderr, "Not a valid directory: %s\n", root);
        return 1;
    }

    file_watcher_start(root, file_change_handler);

    return 0;
}
