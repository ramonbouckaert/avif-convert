//
// Created by Ramon on 02/06/2025.
//

#ifndef AVIF_CONVERT_H
#define AVIF_CONVERT_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "common.h"

#ifdef _WIN32
#ifdef AVIF_CONVERT_EXPORTS
#define AVIF_CONVERT_API __declspec(dllexport)
#else
#define AVIF_CONVERT_API __declspec(dllimport)
#endif
#else
#define AVIF_CONVERT_API
#endif

    AVIF_CONVERT_API int convert_to_avif(
        const uint8_t *input_data,
        size_t input_size,
        uint8_t **output_data,
        size_t *output_size,
        int speed,
        int lossy_quality
    );

    AVIF_CONVERT_API int is_convertible(
        const uint8_t *input_data,
        size_t input_size
    );

#ifdef __cplusplus
}
#endif
#endif //AVIF_CONVERT_H
