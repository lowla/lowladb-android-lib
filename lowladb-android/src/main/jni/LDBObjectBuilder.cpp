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

extern "C" JNIEXPORT jlong JNICALL Java_io_lowla_lowladb_LDBObjectBuilder_alloc(JNIEnv *env, jclass clazz)
{
    CLowlaDBBson::ptr *answer = new CLowlaDBBson::ptr(CLowlaDBBson::create());
    return (jlong)answer;
}

extern "C" JNIEXPORT void JNICALL Java_io_lowla_lowladb_LDBObjectBuilder_dealloc(JNIEnv *env, jclass clazz, jlong ptr)
{
    if (0 != ptr) {
        delete (CLowlaDBBson::ptr *)ptr;
    }
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendDouble(JNIEnv *env, jobject jThis, jstring str, jdouble value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    ptr->appendDouble(szUtf, value);
    env->ReleaseStringUTFChars(str, szUtf);
    return jThis;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendString(JNIEnv *env, jobject jThis, jstring str, jstring value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    const char *szUtfValue = env->GetStringUTFChars(value, NULL);
    ptr->appendString(szUtf, szUtfValue);
    env->ReleaseStringUTFChars(str, szUtf);
    env->ReleaseStringUTFChars(value, szUtfValue);
    return jThis;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendObject(JNIEnv *env, jobject jThis, jstring str, jobject value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    CLowlaDBBson::ptr &ptrValue = getPtr(env, value);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    ptr->appendObject(szUtf, ptrValue->data());
    env->ReleaseStringUTFChars(str, szUtf);
    return jThis;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendObjectId(JNIEnv *env, jobject jThis, jstring str, jobject value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);

    jclass clazz = env->GetObjectClass(value);
    jfieldID fid = env->GetFieldID(clazz, "data", "[B");

    jbyteArray data = (jbyteArray)env->GetObjectField(value, fid);
    jbyte *dataBytes = env->GetByteArrayElements(data, nullptr);

    ptr->appendOid(szUtf, dataBytes);
    env->ReleaseByteArrayElements(data, dataBytes, 0);
    env->ReleaseStringUTFChars(str, szUtf);

    return jThis;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendBool(JNIEnv *env, jobject jThis, jstring str, jboolean value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    ptr->appendBool(szUtf, value);
    env->ReleaseStringUTFChars(str, szUtf);
    return jThis;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendDate(JNIEnv *env, jobject jThis, jstring str, jobject value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);

    jclass clazz = env->GetObjectClass(value);
    jmethodID mid = env->GetMethodID(clazz, "getTime", "()J");

    jlong millis = env->CallLongMethod(value, mid);

    ptr->appendDate(szUtf, millis);
    env->ReleaseStringUTFChars(str, szUtf);

    return jThis;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendInt(JNIEnv *env, jobject jThis, jstring str, jint value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    ptr->appendInt(szUtf, value);
    env->ReleaseStringUTFChars(str, szUtf);
    return jThis;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectBuilder_appendLong(JNIEnv *env, jobject jThis, jstring str, jlong value)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    const char *szUtf = env->GetStringUTFChars(str, NULL);
    ptr->appendLong(szUtf, value);
    env->ReleaseStringUTFChars(str, szUtf);
    return jThis;
}

extern "C" JNIEXPORT jlong JNICALL Java_io_lowla_lowladb_LDBObjectBuilder_finishBson(JNIEnv *env, jobject jThis)
{
    CLowlaDBBson::ptr &ptr = getPtr(env, jThis);
    ptr->finish();
    // We return a new smartpointer to the same Bson object so that the LDBObject can own its own smartpointer.
    CLowlaDBBson::ptr *answer = new CLowlaDBBson::ptr(ptr);
    return (jlong)answer;
}