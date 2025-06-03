#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../avif_convert_io/file_io.h"

#include "file_watcher.h"
#include "../avif_convert_core/avif_convert.h"

void file_change_handler(const char *input_path) {
    // Read file
    uint8_t *file_data = NULL;
    size_t file_size;
    if (read_file(input_path, &file_data, &file_size) != 0) {
        fprintf(stderr, "Failed to read input file\n");
        return;
    }

    if (is_convertible(file_data, file_size) > 0) {
        // Convert to AVIF
        uint8_t *output_data = NULL;
        size_t output_size = 0;
        const int convert_result = convert_to_avif(file_data, file_size, &output_data, &output_size);
        free(file_data);
        if (convert_result != 0) {
            free(output_data);
            return;
        }

        // Write to new file
        char *output_path = make_avif_path(input_path);
        const int write_result = write_file(output_path, output_data, output_size);
        free(output_data);
        if (write_result != 0) {
            fprintf(stderr, "Failed to write to output file\n");
            return;
        }

        // Copy date created/date modified information from the input file to the output file
        if (copy_file_times(input_path, output_path) != 0) {
            fprintf(stderr, "Failing to copy file metadata: %s\n", input_path);
            free(output_path);
            return;
        }

        // Delete the original file
        if (remove(input_path) != 0) {
            fprintf(stderr, "Failed to delete original file: %s\n", input_path);
        }
        free(output_path);
    }
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
