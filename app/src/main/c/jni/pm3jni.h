#include <jni.h>

#ifndef PROXMARK3_PM3JNI_H
#define PROXMARK3_PM3JNI_H
void jni_throw(JNIEnv *env, const char *message);
void jniworker_stdout(JNIEnv *env, jclass class);
#endif //PROXMARK3_PM3JNI_H
