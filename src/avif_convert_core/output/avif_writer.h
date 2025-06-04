//
// Created by Ramon on 02/06/2025.
//

#ifndef AVIF_WRITER_H
#define AVIF_WRITER_H
#include "../common.h"

int write_avif(const LoadedImage *img, avifRWData *out_data, int speed, int lossy_quality);

#endif //AVIF_WRITER_H
