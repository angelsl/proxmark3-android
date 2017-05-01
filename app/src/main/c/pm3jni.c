#include <jni.h>
#include <stdio.h>
#include <unistd.h>

#include "pm3dev.h"

int CommandReceived(char *);

JavaVM *pm3jni_jvm;

void jni_throw(JNIEnv *env, const char *message) {
    jclass runtimeEx = (*env)->FindClass(env, "java/lang/RuntimeException");
    if (!runtimeEx) {
        return;
    }
    (*env)->ThrowNew(env, runtimeEx, message);
}

JNIEXPORT jint JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_jniExecCommand(JNIEnv *env, jclass class __attribute__((unused)), jstring jCmd) {
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
    int pfd[2];
    if (pipe(pfd)) {
        perror("jniworker_stdout: pipe failed");
        jni_throw(env, "Failed to create pipe");
        return;
    }

    // we don't bother cleaning up here
    // if it fails we just bail entirely
    if (dup2(pfd[1], STDOUT_FILENO) != STDOUT_FILENO) {
        perror("jniworker_stdout: dup2(*, 1) failed");
        jni_throw(env, "Failed to redirect stdout");
        return;
    }

    if (dup2(pfd[1], STDERR_FILENO) != STDERR_FILENO) {
        perror("jniworker_stdout: dup2(*, 2) failed");
        jni_throw(env, "Failed to redirect stderr");
        return;
    }

    close(pfd[1]);

    FILE *out = fdopen(pfd[0], "r");
    char buf[2048];
    jmethodID mid = (*env)->GetStaticMethodID(env, class, "dispatchOutput", "(Ljava/lang/String;)V");
    while (fgets(buf, sizeof(buf), out)) {
        jstring input = (*env)->NewStringUTF(env, buf);
        (*env)->CallStaticVoidMethod(env, class, mid, input);
    }
}

JNIEXPORT void JNICALL Java_angelsl_androidapp_proxmark3_interop_Proxmark3_jniChangeDevice(JNIEnv *env, jclass type __attribute__((unused)), jstring path_) {
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    pm3dev_change(path);
    (*env)->ReleaseStringUTFChars(env, path_, path);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved __attribute__((unused))) {
    pm3jni_jvm = vm;
    return JNI_VERSION_1_6;
}
