#include <jni.h>
#include <stdio.h>
#include <lowladb.h>

#include <JniUtils.h>

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    return Liblowladb_JNI_OnLoad(vm, reserved);
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_lowla_lowladb_LDBClient_getVersion(JNIEnv *env, jclass)
{
    char szBuf[60];
    sprintf(szBuf, "0.0.1 (liblowladb %s)", lowladb_get_version().c_str());
    return env->NewStringUTF(szBuf);
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBClient_dropDatabase(JNIEnv *env, jobject jThis, jstring dbName)
{
    const char *szUtf = env->GetStringUTFChars(dbName, NULL);
    lowladb_db_delete(szUtf);
    env->ReleaseStringUTFChars(dbName, szUtf);
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBClient_loadJson(JNIEnv *env, jobject jThis, jstring json)
{
    const char *szUtf = env->GetStringUTFChars(json, NULL);
    lowladb_load_json(szUtf);
    env->ReleaseStringUTFChars(json, szUtf);
}

void listener(void *user, const char *ns) {
    JNIEnvWrapper env;
    jclass clazz = env.FindClass("io/lowla/lowladb/LDBClient");
    jmethodID mid = env.GetStaticMethodID(clazz, "fireCollectionChanged", "(Ljava/lang/String;)V");
    jstring str = env.NewStringUTF(ns);
    env.CallStaticVoidMethod(clazz, mid, str);
    env.DeleteLocalRef(str);
    env.DeleteLocalRef(clazz);
}

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBClient_enableNotifications(JNIEnv *env, jclass clazz, jboolean enable)
{
    if (enable) {
        lowladb_add_collection_listener(listener, nullptr);
    }
    else {
        lowladb_remove_collection_listener(listener);
    }
}
