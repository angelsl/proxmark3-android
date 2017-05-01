#include <jni.h>
#include <stdio.h>

#include "pm3jni.h"
#include "../dev/pm3dev.h"

JavaVM *jvm;

// entrypoint to proxmark3
int CommandReceived(char *);

void jni_throw(JNIEnv *env, const char *message) {
    jclass runtimeEx = (*env)->FindClass(env, "java/lang/RuntimeException");
    if (!runtimeEx) {
        return;
    }
    (*env)->ThrowNew(env, runtimeEx, message);
}

JNIEXPORT jint JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_jniExecCommand(JNIEnv *env, jclass class, jstring jCmd) {
    const char *cmd = (*env)->GetStringUTFChars(env, jCmd, NULL);
    if (!cmd) {
        jni_throw(env, "GetStringUTFChars returned null");
        return -1;
    }
    int ret = CommandReceived((char *) cmd);
    (*env)->ReleaseStringUTFChars(env, jCmd, cmd);
    return ret;
}

JNIEXPORT void JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_jniStdoutWorker(JNIEnv *env, jclass class) {
    jniworker_stdout(env, class);
}

JNIEXPORT void JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_jniChangeDevice(JNIEnv *env, jclass type, jstring path_) {
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    pm3dev_change(path);
    (*env)->ReleaseStringUTFChars(env, path_, path);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jvm = vm;
    return JNI_VERSION_1_6;
}
