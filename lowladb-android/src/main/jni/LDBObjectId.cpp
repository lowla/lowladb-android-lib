#include <jni.h>
#include <lowladb.h>

extern "C" JNIEXPORT jobject JNICALL
Java_io_lowla_lowladb_LDBObjectId_generate(JNIEnv *env, jclass jClazz)
{
    char buffer[CLowlaDBBson::OID_SIZE];
    CLowlaDBBson::oidGenerate(buffer);

    jbyteArray answer = env->NewByteArray(CLowlaDBBson::OID_SIZE);
    env->SetByteArrayRegion(answer, 0, CLowlaDBBson::OID_SIZE, (jbyte*)buffer);

    jmethodID ctor = env->GetMethodID(jClazz, "<init>", "([B)V");
    jobject ret = env->NewObject(jClazz, ctor, answer);
    return ret;
}

extern "C" JNIEXPORT jstring JNICALL
Java_io_lowla_lowladb_LDBObjectId_toHexString(JNIEnv *env, jobject jThis)
{
    jclass clazz = env->GetObjectClass(jThis);
    jfieldID fid = env->GetFieldID(clazz, "data", "[B");
    jbyteArray data = (jbyteArray)env->GetObjectField(jThis, fid);

    jbyte *dataBytes = env->GetByteArrayElements(data, nullptr);
    char buffer[CLowlaDBBson::OID_STRING_SIZE];
    CLowlaDBBson::oidToString((const char *)dataBytes, buffer);
    env->ReleaseByteArrayElements(data, (jbyte *)dataBytes, 0);
    return env->NewStringUTF(buffer);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_io_lowla_lowladb_LDBObjectId_oidFromString(JNIEnv *env, jclass jClazz, jstring str)
{
    char oid[CLowlaDBBson::OID_SIZE];
    const char *szUTF = env->GetStringUTFChars(str, nullptr);
    CLowlaDBBson::oidFromString(oid, szUTF);
    env->ReleaseStringUTFChars(str, szUTF);

    jbyteArray answer = env->NewByteArray(CLowlaDBBson::OID_SIZE);
    env->SetByteArrayRegion(answer, 0, CLowlaDBBson::OID_SIZE, (jbyte*)oid);
    return answer;
}
