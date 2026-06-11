#include <jni.h>
#include <android/log.h>

#define LOG_TAG "MVulkan"

extern "C"
jint JNI_OnLoad(JavaVM* vm, void*)
{
    __android_log_print(
        ANDROID_LOG_INFO,
        LOG_TAG,
        "MVulkan Loaded");

    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_mvulkan_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject)
{
    return env->NewStringUTF(
        "MVulkan Ready");
}
