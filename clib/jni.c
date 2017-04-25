#include <unistd.h>
#include <jni.h>
#include <stdio.h>

#include "cmdmain.h"
#include "device.h"

JNIEXPORT jint JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_execCommand(JNIEnv *, jclass, jstring);
JNIEXPORT void JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_redirThreadWorker(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_nativeChangeDevice(JNIEnv *, jclass, jstring);
void throw_runtime_exception(JNIEnv *env, const char *message);

jint Java_angelsl_androidapp_proxmark3_interop_Proxmark3_execCommand(JNIEnv *env, jclass class, jstring jCmd) {
    const char *cmd = (*env)->GetStringUTFChars(env, jCmd, NULL);
    if (!cmd) {
        throw_runtime_exception(env, "GetStringUTFChars returned null");
        return -1;
    }
    int ret = CommandReceived((char *) cmd);
    (*env)->ReleaseStringUTFChars(env, jCmd, cmd);
    return ret;
}

void Java_angelsl_androidapp_proxmark3_interop_Proxmark3_redirThreadWorker(JNIEnv *env, jclass class) {
    int pfd[2];
    if (pipe(pfd)) {
        throw_runtime_exception(env, "Failed to create pipe");
        return;
    }
    
    // we don't bother cleaning up here
    // if it fails we just bail entirely
    if (dup2(pfd[1], STDOUT_FILENO) != STDOUT_FILENO) {
        throw_runtime_exception(env, "Failed to redirect stdout");
        return;
    }
    
    if (dup2(pfd[1], STDERR_FILENO) != STDERR_FILENO) {
        throw_runtime_exception(env, "Failed to redirect stderr");
        return;
    }
    
    close(pfd[1]);
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);
    
    FILE *out = fdopen(pfd[0], "r");
    char buf[2048];
    jmethodID mid = (*env)->GetStaticMethodID(env, class, "dispatchOutput", "(Ljava/lang/String;)V");
    while (fgets(buf, sizeof(buf), out)) {
        jstring input = (*env)->NewStringUTF(env, buf);
        (*env)->CallStaticVoidMethod(env, class, mid, input);
    }
}

void throw_runtime_exception(JNIEnv *env, const char *message) {
    jclass runtimeEx = (*env)->FindClass(env, "java/lang/RuntimeException");
    if (!runtimeEx) {
        return;
    }
    (*env)->ThrowNew(env, runtimeEx, message);
}

void Java_angelsl_androidapp_proxmark3_interop_Proxmark3_nativeChangeDevice(JNIEnv *env, jclass type, jstring path_) {
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    device_change(path);
    (*env)->ReleaseStringUTFChars(env, path_, path);
}
