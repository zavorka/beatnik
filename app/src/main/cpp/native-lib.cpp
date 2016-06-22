#include <jni.h>
#include <string>

extern "C"
jstring
Java_re_bass_beatnik_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
double
Java_re_bass_beatnik_MainActivity_processAudio(
    JNIEnv* env,
    jobject object, /* this */
    jobject input
) {
    float* inputPtr = (float*) env->GetDirectBufferAddress(input);

    return 0.0;
}