#include <jni.h>
#include <lowladb.h>

#include <android/log.h>

extern "C" JNIEXPORT jobjectArray JNICALL
Java_io_lowla_lowladb_LDBDatabase_collectionNames(JNIEnv *env, jobject jThis)
{
    jobjectArray answer;

    jclass clazz = env->GetObjectClass(jThis);
    jfieldID fid = env->GetFieldID(clazz, "ptr", "J");
    jlong ptr = env->GetLongField(jThis, fid);

    CLowlaDB::ptr *pdb = (CLowlaDB::ptr *)ptr;
    if (nullptr == pdb || nullptr == pdb->get()) {
        answer = (jobjectArray)env->NewObjectArray(0, env->FindClass("java/lang/String"), env->NewStringUTF(""));
    }
    else {
        std::vector<utf16string> names;
        (*pdb)->collectionNames(&names);

        answer = (jobjectArray)env->NewObjectArray(names.size(),
            env->FindClass("java/lang/String"),
            env->NewStringUTF(""));

        for (int i = 0 ; i < names.size() ; ++i) {
            env->SetObjectArrayElement(answer, i, env->NewStringUTF(names[i].c_str()));
        }
    }

    return answer;
}

extern "C" JNIEXPORT jlong JNICALL
Java_io_lowla_lowladb_LDBDatabase_alloc(JNIEnv *env, jclass clazz, jstring dbName)
{
    const char *szUtf = env->GetStringUTFChars(dbName, NULL);
    CLowlaDB::ptr *answer = new CLowlaDB::ptr(CLowlaDB::open(szUtf));
    __android_log_print(ANDROID_LOG_VERBOSE, "Lowla", "alloced db ptr is %ld", (long)answer->get());
    env->ReleaseStringUTFChars(dbName, szUtf);
    return (jlong)answer;
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBDatabase_dealloc(JNIEnv *env, jclass clazz, jlong ptr)
{
    if (0 != ptr) {
        delete (CLowlaDB::ptr *)ptr;
    }
}
