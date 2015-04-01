#include <jni.h>
#include <lowladb.h>

static CLowlaDBWriteResult::ptr &getPtr(JNIEnv *env, jobject jThis)
{
    jclass clazz = env->GetObjectClass(jThis);
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
    jlong ptr = env->GetLongField(jThis, fid);

    return *(CLowlaDBWriteResult::ptr *)ptr;
}

extern "C" JNIEXPORT jint JNICALL
Java_io_lowla_lowladb_LDBWriteResult_getDocumentCount(JNIEnv *env, jobject jThis)
{
    CLowlaDBWriteResult::ptr &wr = getPtr(env, jThis);

    return wr->documentCount();
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBWriteResult_getDocument(JNIEnv *env, jobject jThis, jint i)
{
    CLowlaDBWriteResult::ptr &wr = getPtr(env, jThis);

    CLowlaDBBson::ptr doc = wr->document(i);

    jclass clazz = env->FindClass("io/lowla/lowladb/LDBObject");
    jmethodID ctor = env->GetMethodID(clazz, "<init>", "(J)V");
    jobject ret = env->NewObject(clazz, ctor, (jlong)new CLowlaDBBson::ptr(doc));
    return ret;
}

extern "C" JNIEXPORT void JNICALL Java_io_lowla_lowladb_LDBWriteResult_dealloc(JNIEnv *env, jclass clazz, jlong ptr)
{
    if (0 != ptr) {
        delete (CLowlaDBWriteResult::ptr *)ptr;
    }
}
