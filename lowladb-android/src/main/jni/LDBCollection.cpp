#include <jni.h>
#include <lowladb.h>
#include <TeamstudioException.h>

#include <android/log.h>

static CLowlaDBCollection::ptr &getPtr(JNIEnv *env, jobject jThis)
{
    jclass clazz = env->GetObjectClass(jThis);
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
    jlong ptr = env->GetLongField(jThis, fid);

    if (0 == ptr) {
        // Retrieve the database
        jfieldID fidDb = env->GetFieldID(clazz, "database", "Lio/lowla/lowladb/LDBDatabase;");
        jobject objDb = env->GetObjectField(jThis, fidDb);

        // Extract its pointer
        jclass clazzDb = env->GetObjectClass(objDb);
        jfieldID fidDbPtr = env->GetFieldID(clazzDb, "ptr", "J");
        jlong ptrDb = env->GetLongField(objDb, fidDbPtr);

        CLowlaDB::ptr &db = *(CLowlaDB::ptr *)ptrDb;

        __android_log_print(ANDROID_LOG_VERBOSE, "Lowla", "db is %ld", (long)db.get());
        if (nullptr == db.get()) {
            jclass clazzEx = env->FindClass("java/lang/NullPointerException");
            env->ThrowNew(clazzEx, "Bad database pointer");
            return *(CLowlaDBCollection::ptr *)nullptr;
        }

        // Now the collection name
        jfieldID fidName = env->GetFieldID(clazz, "name", "Ljava/lang/String;");
        jstring strName = (jstring)env->GetObjectField(jThis, fidName);
        const char *szUTF = env->GetStringUTFChars(strName, nullptr);

        // Create the collection
        ptr = (jlong)new CLowlaDBCollection::ptr(db->createCollection(szUTF));
        env->ReleaseStringUTFChars(strName, szUTF);

        // And store it in our ptr field for next time
        env->SetLongField(jThis, fid, ptr);
    }
    return *(CLowlaDBCollection::ptr *)ptr;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCollection_insert(JNIEnv *env, jobject jThis, jobject obj)
{
    CLowlaDBCollection::ptr &coll = getPtr(env, jThis);
    if (nullptr != env->ExceptionOccurred()) {
        return nullptr;
    }

    jclass clazz = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
    jlong ptr = env->GetLongField(obj, fid);

    CLowlaDBBson::ptr &bson = *(CLowlaDBBson::ptr *)ptr;

    try {
        CLowlaDBWriteResult::ptr wr = coll->insert(bson->data());

        jclass clazzWr = env->FindClass("io/lowla/lowladb/LDBWriteResult");
        jmethodID ctor = env->GetMethodID(clazzWr, "<init>", "(J)V");
        jobject ret = env->NewObject(clazzWr, ctor, (jlong)new CLowlaDBWriteResult::ptr(wr));
        return ret;
    }
    catch (TeamstudioException &e) {
        jclass clazzEx = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(clazzEx, e.what());
    }
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCollection_insertArray(JNIEnv *env, jobject jThis, jobjectArray arr)
{
    CLowlaDBCollection::ptr &coll = getPtr(env, jThis);

    jclass clazz = env->FindClass("io/lowla/lowladb/LDBObject");
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");

    jsize size = env->GetArrayLength(arr);
    std::vector<const char *> bsonArr;
    for (jsize i = 0 ; i < size ; ++i) {
        jobject obj = env->GetObjectArrayElement(arr, i);
        jlong ptr = env->GetLongField(obj, fid);
        CLowlaDBBson::ptr &bson = *(CLowlaDBBson::ptr *)ptr;
        bsonArr.push_back(bson->data());
    }

    try {
        CLowlaDBWriteResult::ptr wr = coll->insert(bsonArr);

        jclass clazzWr = env->FindClass("io/lowla/lowladb/LDBWriteResult");
        jmethodID ctor = env->GetMethodID(clazzWr, "<init>", "(J)V");
        jobject ret = env->NewObject(clazzWr, ctor, (jlong)new CLowlaDBWriteResult::ptr(wr));
        return ret;
    }
    catch (TeamstudioException &e) {
        jclass clazzEx = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(clazzEx, e.what());
    }
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCollection_remove(JNIEnv *env, jobject jThis, jobject obj)
{
    CLowlaDBCollection::ptr &coll = getPtr(env, jThis);

    const char *query = nullptr;
    if (nullptr != obj) {
        jclass clazz = env->GetObjectClass(obj);
        jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
        jlong ptr = env->GetLongField(obj, fid);

        CLowlaDBBson::ptr &bson = *(CLowlaDBBson::ptr *)ptr;
        query = bson->data();
    }

    CLowlaDBWriteResult::ptr wr = coll->remove(query);

    jclass clazzWr = env->FindClass("io/lowla/lowladb/LDBWriteResult");
    jmethodID ctor = env->GetMethodID(clazzWr, "<init>", "(J)V");
    jobject ret = env->NewObject(clazzWr, ctor, (jlong)new CLowlaDBWriteResult::ptr(wr));
    return ret;
}

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBCollection_update(JNIEnv *env, jobject jThis, jobject query, jobject obj,
    jboolean upsert, jboolean multi)
{
    CLowlaDBCollection::ptr &coll = getPtr(env, jThis);

    const char *queryBson = nullptr;
    if (nullptr != query) {
        jclass clazz = env->GetObjectClass(query);
        jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
        jlong ptr = env->GetLongField(query, fid);

        CLowlaDBBson::ptr &bson = *(CLowlaDBBson::ptr *)ptr;
        queryBson = bson->data();
    }

    jclass clazz = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
    jlong ptr = env->GetLongField(obj, fid);
    CLowlaDBBson::ptr &objBson = *(CLowlaDBBson::ptr *)ptr;

    try {
        CLowlaDBWriteResult::ptr wr = coll->update(queryBson, objBson->data(), upsert, multi);

        jclass clazzWr = env->FindClass("io/lowla/lowladb/LDBWriteResult");
        jmethodID ctor = env->GetMethodID(clazzWr, "<init>", "(J)V");
        jobject ret = env->NewObject(clazzWr, ctor, (jlong)new CLowlaDBWriteResult::ptr(wr));
        return ret;
    }
    catch (TeamstudioException &e) {
        jclass clazzEx = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(clazzEx, e.what());
    }
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBCollection_ensureOpen(JNIEnv *env, jobject jThis)
{
    __android_log_print(ANDROID_LOG_VERBOSE, "Lowla", "Entering Collection.ensureOpen");
    getPtr(env, jThis);
    __android_log_print(ANDROID_LOG_VERBOSE, "Lowla", "Leaving Collection.ensureOpen");
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBCollection_dealloc(JNIEnv *env, jclass clazz, jlong ptr)
{
    if (0 != ptr) {
        delete (CLowlaDBCollection::ptr *)ptr;
    }
}
