//
// Created by Ramon on 02/06/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_io.h"
#include "../avif_convert/avif_convert.h"

#define OPTION_DELETE_ORIGINAL "--delete-original"
#define OPTION_DELETE_ORIGINAL_SHORT "-d"

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s <input.[png|jpg|webp|heic]> [%s | %s]\n\n",
            argv[0],
            OPTION_DELETE_ORIGINAL, OPTION_DELETE_ORIGINAL_SHORT);
        fprintf(stderr,
            "Options:\n"
            "  %s, %s    Delete the original file after conversion\n",
            OPTION_DELETE_ORIGINAL, OPTION_DELETE_ORIGINAL_SHORT);
        return 1;
    }
    const char *input_path = argv[1];
    char *output_path = make_avif_path(input_path);
    bool delete_original = false;

    // Parse additional options
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], OPTION_DELETE_ORIGINAL) == 0 || strcmp(argv[i], OPTION_DELETE_ORIGINAL_SHORT) == 0) {
            delete_original = true;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    uint8_t *file_data = NULL;
    size_t file_size;
    if (read_file(input_path, &file_data, &file_size) != 0) {
        fprintf(stderr, "Failed to read input file\n");
        return 1;
    }

    uint8_t *output_data = NULL;
    size_t output_size = 0;
    const int result = convert_to_avif(file_data, file_size, &output_data, &output_size);
    if (result != 0) return result;

    free(file_data);

    // Write to file
    if (write_file(output_path, output_data, output_size) != 0) {
        fprintf(stderr, "Failed to write to output file\n");
        return 1;
    }

    free(output_data);

    if (delete_original && remove(input_path) != 0) {
        fprintf(stderr, "Warning: Failed to delete original file: %s\n", input_path);
    }

    free(output_path);

    return result;
}
