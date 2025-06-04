#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
    #include <windows.h>
    #define sleep_seconds(sec) Sleep((sec) * 1000)
#else
    #include <unistd.h>
    #define sleep_seconds(sec) sleep(sec)
#endif

#include "../avif_convert_io/file_io.h"
#include "file_watcher.h"
#include "../avif_convert_core/avif_convert.h"

typedef int (*retryable_fn)(const void *ctx);

int retry(const int max_attempts, const retryable_fn fn, const void *ctx) {
    for (int attempt = 1; attempt <= max_attempts; ++attempt) {
        if (fn(ctx) == 0)
            return 0;
        if (attempt < max_attempts)
            sleep_seconds(1);
    }
    return -1;
}

struct read_file_ctx {
    const char *path;
    uint8_t **data;
    size_t *size;
};

int try_read_file(const void *ctx) {
    const struct read_file_ctx *args = ctx;
    return read_file(args->path, args->data, args->size);
}

struct write_file_ctx {
    const char *path;
    const uint8_t *data;
    size_t size;
};

int try_write_file(const void *ctx) {
    const struct write_file_ctx *args = ctx;
    return write_file(args->path, args->data, args->size);
}

struct copy_time_ctx {
    const char *src;
    const char *dst;
};

int try_copy_time(const void *ctx) {
    const struct copy_time_ctx *args = ctx;
    return copy_file_times(args->src, args->dst);
}

struct remove_ctx {
    const char *path;
};

int try_remove_file(const void *ctx) {
    const struct remove_ctx *args = ctx;
    return remove(args->path);
}

void file_change_handler(const char *input_path) {
    // Read file
    uint8_t *file_data = NULL;
    size_t file_size;
    const struct read_file_ctx rctx = { input_path, &file_data, &file_size };
    if (retry(3, try_read_file, &rctx) != 0) {
        fprintf(stderr, "Failed to read input file\n");
        return;
    }

    // Check if the file is convertible
    if (is_convertible(file_data, file_size) != 1) {
        free(file_data);
        return;
    }

    // Convert to AVIF
    uint8_t *output_data = NULL;
    size_t output_size = 0;
    const int convert_result = convert_to_avif(file_data, file_size, &output_data, &output_size, 4, 80);
    free(file_data);
    if (convert_result != 0) {
        fprintf(stderr, "Failed to convert file to avif: %s\n", input_path);
        free(output_data);
        return;
    }

    // Write to new file
    char *output_path = make_avif_path(input_path);
    const struct write_file_ctx wctx = { output_path, output_data, output_size };
    const int write_result = retry(3, try_write_file, &wctx);
    free(output_data);
    if (write_result != 0) {
        fprintf(stderr, "Failed to write to output file: %s\n", output_path);
        free(output_path);
        return;
    }

    // Copy date created/date modified information from the input file to the output file
    const struct copy_time_ctx tctx = { input_path, output_path };
    if (retry(3, try_copy_time, &tctx) != 0) {
        fprintf(stderr, "Failed to copy file metadata: %s to %s\n", input_path, output_path);
        free(output_path);
        return;
    }

    // Delete the original file
    const struct remove_ctx rmctx = { input_path };
    if (retry(3, try_remove_file, &rmctx) != 0) {
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
