//
// Created by Ramon on 02/06/2025.
//

#ifndef AVIF_CONVERT_JNI_H
#define AVIF_CONVERT_JNI_H
#include <jni.h>

JNIEXPORT jbyteArray JNICALL Java_com_io_bouckaert_avif_convert_convertToAvif(
    JNIEnv *env,
    jclass clazz,
    jbyteArray input
);
#endif //AVIF_CONVERT_JNI_H
