#include <jni.h>
#include <lowladb.h>

#include <android/log.h>

static CLowlaDBBson::ptr &getPtr(JNIEnv *env, jobject jThis)
{
    jclass clazz = env->GetObjectClass(jThis);
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
    jlong ptr = env->GetLongField(jThis, fid);

    return *(CLowlaDBBson::ptr *)ptr;
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBObject_dealloc(JNIEnv *env, jclass clazz, jlong ptr)
{
    if (0 != ptr) {
        delete (CLowlaDBBson::ptr *)ptr;
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_io_lowla_lowladb_LDBObject_containsField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    jboolean answer = ptr->containsKey(szUtf);
    env->ReleaseStringUTFChars(str, szUtf);
    return answer;
}

extern "C" JNIEXPORT jdouble JNICALL
Java_io_lowla_lowladb_LDBObject_doubleForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    jdouble answer;
    if (!ptr->doubleForKey(szUtf, &answer)) {
        answer = 0.0;
    }
    env->ReleaseStringUTFChars(str, szUtf);
    return answer;
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_lowla_lowladb_LDBObject_stringForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    const char *answer;
    if (!ptr->stringForKey(szUtf, &answer)) {
        answer = "";
    }
    env->ReleaseStringUTFChars(str, szUtf);
    return env->NewStringUTF(answer);
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObject_objectForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    CLowlaDBBson::ptr answer;
    ptr->objectForKey(szUtf, &answer);
    env->ReleaseStringUTFChars(str, szUtf);
    if (answer) {
        jclass clazz = env->GetObjectClass(jThis);
        jmethodID ctor = env->GetMethodID(clazz, "<init>", "(J)V");
        jobject ret = env->NewObject(clazz, ctor, (jlong)new CLowlaDBBson::ptr(answer));
        return ret;
    }
    else {
        return nullptr;
    }
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObject_objectIdForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    char answer[CLowlaDBBson::OID_SIZE];
    if (!ptr->oidForKey(szUtf, answer)) {
        env->ReleaseStringUTFChars(str, szUtf);
        return nullptr;
    }
    env->ReleaseStringUTFChars(str, szUtf);
    jbyteArray data = env->NewByteArray(CLowlaDBBson::OID_SIZE);
    env->SetByteArrayRegion(data, 0, CLowlaDBBson::OID_SIZE, (jbyte *)answer);

    jclass clazz = env->FindClass("io/lowla/lowladb/LDBObjectId");
    jmethodID ctor = env->GetMethodID(clazz, "<init>", "([B)V");
    jobject ret = env->NewObject(clazz, ctor, data);
    return ret;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_io_lowla_lowladb_LDBObject_boolForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    bool answer;
    if (!ptr->boolForKey(szUtf, &answer)) {
        answer = false;
    }
    env->ReleaseStringUTFChars(str, szUtf);
    return answer;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObject_dateForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    int64_t answer;
    if (!ptr->dateForKey(szUtf, &answer)) {
        env->ReleaseStringUTFChars(str, szUtf);
        return nullptr;
    }
    env->ReleaseStringUTFChars(str, szUtf);

    jclass clazz = env->FindClass("java/util/Date");
    jmethodID ctor = env->GetMethodID(clazz, "<init>", "(J)V");
    jobject ret = env->NewObject(clazz, ctor, answer);
    return ret;
}

extern "C" JNIEXPORT jint JNICALL
Java_io_lowla_lowladb_LDBObject_intForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    jint answer;
    if (!ptr->intForKey(szUtf, &answer)) {
        answer = 0;
    }
    env->ReleaseStringUTFChars(str, szUtf);
    return answer;
}

extern "C" JNIEXPORT jlong JNICALL
Java_io_lowla_lowladb_LDBObject_longForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    jlong answer;
    if (!ptr->longForKey(szUtf, &answer)) {
        answer = 0;
    }
    env->ReleaseStringUTFChars(str, szUtf);
    return answer;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObject_arrayForField(JNIEnv *env, jobject jThis, jstring str)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    CLowlaDBBson::ptr answer;
    ptr->arrayForKey(szUtf, &answer);
    env->ReleaseStringUTFChars(str, szUtf);
    if (answer) {
        jclass clazz = env->GetObjectClass(jThis);
        jmethodID ctor = env->GetMethodID(clazz, "<init>", "(J)V");
        jobject ret = env->NewObject(clazz, ctor, (jlong)new CLowlaDBBson::ptr(answer));
        return ret;
    }
    else {
        return nullptr;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_lowla_lowladb_LDBObject_asJson(JNIEnv *env, jobject jThis)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    return env->NewStringUTF(lowladb_bson_to_json(ptr->data()).c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_io_lowla_lowladb_LDBObject_equals(JNIEnv *env, jobject jThis, jobject o)
{
    if (!o) {
        return false;
    }

    jclass clazzThis = env->GetObjectClass(jThis);
    jclass clazzOther = env->GetObjectClass(o);

    if (!env->IsSameObject(clazzThis, clazzOther)) {
        return false;
    }

    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    CLowlaDBBson::ptr &ptrOther = getPtr(env, o);

    if (ptr->size() != ptrOther->size()) {
        return false;
    }

    return 0 == memcmp(ptr->data(), ptrOther->data(), ptr->size());
}

extern "C" JNIEXPORT jint JNICALL
Java_io_lowla_lowladb_LDBObject_hashCode(JNIEnv *env, jobject jThis)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);

    // Simple hashcode for byte array: seed is 173, for each char multiply result by 37 and add new value
    jint answer = 173;
    const unsigned char *data = (const unsigned char *)ptr->data();
    int size = ptr->size();
    for (int i = 0 ; i < size ; ++i) {
        answer = answer * 37 + data[i];
    }
    return answer;
}
