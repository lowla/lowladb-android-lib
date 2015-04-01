#include <jni.h>
#include <lowladb.h>
#include <TeamstudioException.h>

#include <android/log.h>

static CLowlaDBCursor::ptr &getPtr(JNIEnv *env, jobject jThis)
{
    jclass clazz = env->GetObjectClass(jThis);
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
    jlong ptr = env->GetLongField(jThis, fid);
    return *(CLowlaDBCursor::ptr *)ptr;
}

extern "C" JNIEXPORT jlong JNICALL
Java_io_lowla_lowladb_LDBCursor_count(JNIEnv *env, jobject jThis)
{
    CLowlaDBCursor::ptr &cursor = getPtr(env, jThis);

    return cursor->count();
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCursor_limit(JNIEnv *env, jobject jThis, jint limit)
{
    CLowlaDBCursor::ptr &cursor = getPtr(env, jThis);

    CLowlaDBCursor::ptr answer = cursor->limit(limit);

    jclass clazz = env->GetObjectClass(jThis);
    jmethodID ctor = env->GetMethodID(clazz, "<init>", "(J)V");
    return env->NewObject(clazz, ctor, (jlong)new CLowlaDBCursor::ptr(answer));
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCursor_sort(JNIEnv *env, jobject jThis, jobject sort)
{
    CLowlaDBCursor::ptr &cursor = getPtr(env, jThis);

    jclass clazzObj = env->FindClass("io/lowla/lowladb/LDBObject");
    jfieldID fid = env->GetFieldID(clazzObj, "ptr", "J");
    jlong objPtr = env->GetLongField(sort, fid);

    CLowlaDBBson::ptr &sortBson = *(CLowlaDBBson::ptr *)objPtr;

    CLowlaDBCursor::ptr answer = cursor->sort(sortBson->data());

    jclass clazz = env->GetObjectClass(jThis);
    jmethodID ctor = env->GetMethodID(clazz, "<init>", "(J)V");
    return env->NewObject(clazz, ctor, (jlong)new CLowlaDBCursor::ptr(answer));
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCursor_showPending(JNIEnv *env, jobject jThis)
{
    CLowlaDBCursor::ptr &cursor = getPtr(env, jThis);

    CLowlaDBCursor::ptr answer = cursor->showPending();

    jclass clazz = env->GetObjectClass(jThis);
    jmethodID ctor = env->GetMethodID(clazz, "<init>", "(J)V");
    return env->NewObject(clazz, ctor, (jlong)new CLowlaDBCursor::ptr(answer));
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCursor_nextRecord(JNIEnv *env, jobject jThis)
{
    CLowlaDBCursor::ptr &cursor = getPtr(env, jThis);

    CLowlaDBBson::ptr answer = cursor->next();

    if (answer) {
        jclass clazzObj = env->FindClass("io/lowla/lowladb/LDBObject");
        jmethodID ctor = env->GetMethodID(clazzObj, "<init>", "(J)V");
        return env->NewObject(clazzObj, ctor, (jlong)new CLowlaDBBson::ptr(answer));
    }
    else {
        return nullptr;
    }
}

extern "C" JNIEXPORT jlong JNICALL
Java_io_lowla_lowladb_LDBCursor_alloc(JNIEnv *env, jclass clazz, jlong collectionPtr, jlong queryPtr)
{
    CLowlaDBCollection::ptr &coll = *(CLowlaDBCollection::ptr *)collectionPtr;
    const char *query = nullptr;
    if (0 != queryPtr) {
        CLowlaDBBson::ptr &queryObj = *(CLowlaDBBson::ptr *)queryPtr;
        query = queryObj->data();
    }
    CLowlaDBCursor::ptr *answer = new CLowlaDBCursor::ptr(CLowlaDBCursor::create(coll, query));
    return (jlong)answer;
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBCursor_dealloc(JNIEnv *env, jclass clazz, jlong ptr)
{
    if (0 != ptr) {
        delete (CLowlaDBCursor::ptr *)ptr;
    }
}
