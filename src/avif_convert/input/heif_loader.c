//
// Created by Ramon on 02/06/2025.
//

#include <stdbool.h>
#include <stdio.h>
#include <libheif/heif.h>
#include <stdlib.h>
#include <string.h>

#include "loaders.h"

int load_heif(const uint8_t *data, const size_t size, LoadedImage *out_image) {
    struct heif_context *ctx = heif_context_alloc();
    const struct heif_error err1 = heif_context_read_from_memory_without_copy(ctx, data, size, NULL);
    if (err1.code != heif_error_Ok) {
        fprintf(stderr, "Failed to read HEIF file.\n");
        heif_context_free(ctx);
        return -1;
    }

    struct heif_image_handle *handle;
    const struct heif_error err2 = heif_context_get_primary_image_handle(ctx, &handle);
    if (err2.code != heif_error_Ok) {
        fprintf(stderr, "Failed to get image handle\n");
        heif_image_handle_release(handle);
        heif_context_free(ctx);
        return -1;
    }

    struct heif_image *img;
    const struct heif_error err3 = heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, NULL);
    if (err3.code != heif_error_Ok) {
        fprintf(stderr, "Failed to decode HEIF image\n");
        heif_image_release(img);
        heif_image_handle_release(handle);
        heif_context_free(ctx);
        return -1;
    }

    const int width = heif_image_get_width(img, heif_channel_interleaved);
    const int height = heif_image_get_height(img, heif_channel_interleaved);
    int stride;
    const uint8_t *src = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);

    uint8_t *copy = malloc(height * stride);
    memcpy(copy, src, height * stride);

    *out_image = construct_image(
        copy,
        width,
        height,
        stride,
        false
        // HEIC files from iPhone are typically lossy. They can be lossless but more effort than it's worth to check.
    );

    // Handle metadata
    const int metadata_count = heif_image_handle_get_number_of_metadata_blocks(handle, NULL);
    if (metadata_count > 0) {
        heif_item_id *ids = malloc(sizeof(heif_item_id) * metadata_count);
        if (!ids) {
            heif_image_release(img);
            heif_image_handle_release(handle);
            heif_context_free(ctx);
            return -1;
        }
        heif_image_handle_get_list_of_metadata_block_IDs(handle, NULL, ids, 99);
        for (int i = 0; i < metadata_count; i++) {
            const char *type = heif_image_handle_get_metadata_type(handle, ids[i]);
            const char *content_type = heif_image_handle_get_metadata_content_type(handle, ids[i]);
            if (strcmp(type, "Exif") == 0) {
                const size_t exif_size = heif_image_handle_get_metadata_size(handle, ids[i]);
                unsigned char *exif = malloc(exif_size);
                heif_image_handle_get_metadata(handle, ids[i], exif);
                out_image->exif_data = exif;
                out_image->exif_size = exif_size;
            } else if (strcmp(content_type, "application/rdf+xml") == 0) {
                const size_t xmp_size = heif_image_handle_get_metadata_size(handle, ids[i]);
                unsigned char *xmp = malloc(xmp_size);
                heif_image_handle_get_metadata(handle, ids[i], xmp);
                out_image->xmp_data = xmp;
                out_image->xmp_size = xmp_size;
            }
        }
        free(ids);
    }

    heif_image_release(img);
    heif_image_handle_release(handle);
    heif_context_free(ctx);
    return 0;
}
