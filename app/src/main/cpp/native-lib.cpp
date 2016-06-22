#include <jni.h>
#include <string>
#include <android/log.h>

extern "C"
void
Java_re_bass_beatnik_MainActivity_init(
        JNIEnv* env,
        jobject /* this */) {

}

extern "C"
double
Java_re_bass_beatnik_MainActivity_processAudio(
    JNIEnv* env,
    jobject object, /* this */
    jobject input
) {
    float* inputPtr = (float*) env->GetDirectBufferAddress(input);

    //__android_log_write(ANDROID_LOG_DEBUG, "processAudio", "Hello from native processAudio method!");
    return 0.0;
}