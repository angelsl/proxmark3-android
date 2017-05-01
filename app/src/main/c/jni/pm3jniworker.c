#include <unistd.h>
#include <stdio.h>

#include "pm3jni.h"

void jniworker_stdout(JNIEnv *env, jclass class) {
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
