//
// Created by Ramon on 02/06/2025.
//
#include <webp/decode.h>
#include <webp/demux.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loaders.h"

// Utility: Read 4-character chunk type
static int is_chunk(const uint8_t *data, const char *tag) {
    return memcmp(data, tag, 4) == 0;
}

int detect_webp_lossless(const uint8_t *data, const size_t size) {
    if (size < 16 || !is_chunk(data, "RIFF") || !is_chunk(data + 8, "WEBP")) {
        return -1; // Not a valid WebP
    }

    const uint8_t *ptr = data + 12; // Skip RIFF header

    while (ptr < data + size - 8) {
        if (is_chunk(ptr, "VP8L")) return 1; // Lossless
        if (is_chunk(ptr, "VP8 ")) return 0; // Lossy
        if (is_chunk(ptr, "VP8X")) {
            // Extended format — keep going
            const uint32_t chunk_size = ptr[4] | ptr[5] << 8 | ptr[6] << 16 | ptr[7] << 24;
            ptr += 8 + ((chunk_size + 1) & ~1); // 8 bytes header + aligned size
            continue;
        }

        // Unknown chunk — skip
        uint32_t chunk_size = ptr[4] | ptr[5] << 8 | ptr[6] << 16 | ptr[7] << 24;
        ptr += 8 + (chunk_size + 1 & ~1);
    }

    return -1; // Could not determine
}

int load_webp(const uint8_t *data, const size_t size, LoadedImage *out_image) {
    int width = 0;
    int height = 0;

    // Detect whether it's a lossless or lossy webp
    const int lossless = detect_webp_lossless(data, size);

    // Decode RGBA value from the image itself
    uint8_t *output = WebPDecodeRGBA(data, size, &width, &height);
    if (!output) {
        fprintf(stderr, "WebP decode failed\n");
        return -1;
    }

    unsigned char *rgba = malloc(width * height * 4);
    memcpy(rgba, output, width * height * 4);
    WebPFree(output);

    *out_image = construct_image(
        rgba,
        width,
        height,
        width * 4, // 4 bytes per pixel (RGBA)
        lossless == 1 // 1 = lossless, 0 = lossy
    );

    // Handle metadata
    const WebPData webp_data = {.bytes = data, .size = size};
    WebPDemuxer *demux = WebPDemux(&webp_data);
    if (!demux) {
        fprintf(stderr, "WebP demux failed\n");
        return -1;
    }
    WebPChunkIterator chunk_iter;
    if (WebPDemuxGetChunk(demux, "EXIF", 1, &chunk_iter)) {
        unsigned char *exif = malloc(chunk_iter.chunk.size);
        memcpy(exif, chunk_iter.chunk.bytes, chunk_iter.chunk.size);
        out_image->exif_data = exif;
        out_image->exif_size = chunk_iter.chunk.size;
        WebPDemuxReleaseChunkIterator(&chunk_iter);
    }
    if (WebPDemuxGetChunk(demux, "XMP ", 1, &chunk_iter)) {
        unsigned char *xmp = malloc(chunk_iter.chunk.size);
        memcpy(xmp, chunk_iter.chunk.bytes, chunk_iter.chunk.size);
        out_image->xmp_data = xmp;
        out_image->xmp_size = chunk_iter.chunk.size;
        WebPDemuxReleaseChunkIterator(&chunk_iter);
    }
    WebPDemuxDelete(demux);

    return 0;
}
