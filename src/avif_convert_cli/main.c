//
// Created by Ramon on 02/06/2025.
//

#include <stdio.h>
#include <stdlib.h>

#include "file_io.h"
#include "../avif_convert/avif_convert.h"

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s input.[png|jpg|webp|heic]\n", argv[0]);
        return 1;
    }
    const char *input_path = argv[1];
    char *output_path = make_avif_path(input_path);

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
    free(output_path);

    return result;
}
