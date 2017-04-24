#include <unistd.h>
#include <pthread.h>
#include <jni.h>
#include <stdio.h>

#include "cmdmain.h"

JNIEXPORT jint JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_execCommand(JNIEnv *, jclass, jstring);
JNIEXPORT jint JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_init(JNIEnv *, jclass);
JNIEXPORT void JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_redirThreadWorker(JNIEnv *, jclass, jint);
void throw_runtime_exception(JNIEnv *env, const char *message);

JNIEXPORT jint JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_execCommand(JNIEnv *env, jclass class, jstring jCmd) {
    const char *cmd = (*env)->GetStringUTFChars(env, jCmd, NULL);
    if (!cmd) {
        throw_runtime_exception(env, "GetStringUTFChars returned null");
        return -1;
    }
    int ret = CommandReceived((char *) cmd);
    (*env)->ReleaseStringUTFChars(env, jCmd, cmd);
    return ret;
}

JNIEXPORT jint JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_init(JNIEnv *env, jclass class) {
    int pfd[2];
    if (pipe(pfd)) {
        throw_runtime_exception(env, "Failed to create pipe");
        return -1;
    }
    
    // we don't bother cleaning up here
    // if it fails we just bail entirely
    if (dup2(pfd[1], STDOUT_FILENO) != STDOUT_FILENO) {
        throw_runtime_exception(env, "Failed to redirect stdout");
        return -1;
    }
    
    if (dup2(pfd[1], STDERR_FILENO) != STDERR_FILENO) {
        throw_runtime_exception(env, "Failed to redirect stderr");
        return -1;
    }
    
    close(pfd[1]);
    
    return pfd[0];
}

JNIEXPORT void JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_redirThreadWorker(JNIEnv *env, jclass class, jint rfd) {
    FILE *out = fdopen(rfd, "r");
    char buf[2049];
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
