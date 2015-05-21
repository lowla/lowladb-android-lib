#include <jni.h>
#include <stdio.h>
#include <cstdlib>
#include <integration.h>
#include <lowladb.h>

#include <android/log.h>
#include <JniUtils.h>

static jbyteArray doHttp(JNIEnv *env, const utf16string &url, CLowlaDBBson::ptr body) {
    jclass clazz = env->FindClass("io/lowla/lowladb/LDBClient");
    jmethodID mid = env->GetStaticMethodID(clazz, "doHttp", "(Ljava/lang/String;[B)[B");

    jstring jurl = env->NewStringUTF(url.c_str());
    jbyteArray jbody = nullptr;
    if (body) {
        utf16string json = lowladb_bson_to_json(body->data());
        const char *bytes = json.c_str();
        jbody = env->NewByteArray(strlen(bytes));
        env->SetByteArrayRegion(jbody, 0, strlen(bytes), (const jbyte *)bytes);
    }

    __android_log_print(ANDROID_LOG_VERBOSE, "Lowla", "Making http request to %s", url.c_str());

    return (jbyteArray)env->CallStaticObjectMethod(clazz, mid, jurl, jbody);
}

static jstring getExceptionMessage(JNIEnv *env, jobject ex) {
    jclass clazz = env->FindClass("java/lang/Throwable");
    jmethodID mid = env->GetMethodID(clazz, "getMessage", "()Ljava/lang/String;");

    jobject str = env->CallObjectMethod(ex, mid);
    return (jstring)str;
}

static jstring clearException(JNIEnv *env) {
    jthrowable ex = env->ExceptionOccurred();
    if (nullptr == ex) {
        return nullptr;
    }
    env->ExceptionClear();
    jstring msg = getExceptionMessage(env, ex);
    env->DeleteLocalRef(ex);
    return msg;
}

static void invokeNotify(JNIEnv *env, jobject notify, const char *status, jstring msg) {
    jclass clazz = env->FindClass("io/lowla/lowladb/LDBClient$SyncNotifier");
    jmethodID mid = env->GetMethodID(clazz, "notify", "(Lio/lowla/lowladb/LDBClient$LDBSyncStatus;Ljava/lang/String;)V");
    jclass clazzEnum = env->FindClass("io/lowla/lowladb/LDBClient$LDBSyncStatus");
    jfieldID fid = env->GetStaticFieldID(clazzEnum, status, "Lio/lowla/lowladb/LDBClient$LDBSyncStatus;");
    jobject statusEnum = env->GetStaticObjectField(clazzEnum, fid);

    env->CallVoidMethod(notify, mid, statusEnum, msg);
}

static bool pullChunk(JNIEnv *env, const utf16string &server, CLowlaDBPullData::ptr pd, jobject notify) {
    // Create the pull request before checking for isComplete because the act of
    // building the request may complete the pull (e.g. if we already have all the documents)
    CLowlaDBBson::ptr pullRequestBson = lowladb_create_pull_request(pd);

    __android_log_print(ANDROID_LOG_VERBOSE, "Lowla", "sequence is %d", pd->getSequenceForNextRequest());
    SysSetProperty("sequence", utf16string::valueOf(pd->getSequenceForNextRequest()));

    if (pd->isComplete()) {
        return false;
    }

    if (!pullRequestBson) {
        lowladb_apply_json_pull_response("[]", pd);
        return true;
    }

    utf16string url = server + "/_lowla/pull";
    jbyteArray response = doHttp(env, url, pullRequestBson);
    if (!env->ExceptionCheck()) {
        const char *json = (const char *)env->GetByteArrayElements(response, nullptr);
        lowladb_apply_json_pull_response(json, pd);
        env->ReleaseByteArrayElements(response, (jbyte *)json, 0);
    }
    return true;
}

static void pull(JNIEnv *env, const utf16string &server, jobject notify) {
    invokeNotify(env, notify, "PULL_STARTED", nullptr);

    utf16string url = server + "/_lowla/changes?seq=" + SysGetProperty("sequence", "0");

    jbyteArray response = doHttp(env, url, nullptr);
    if (env->ExceptionCheck()) {
        jstring msg = clearException(env);

        invokeNotify(env, notify, "PULL_ENDED", nullptr);
        invokeNotify(env, notify, "ERROR", msg);
        return;
    }

    const char *json = (const char *)env->GetByteArrayElements(response, nullptr);

    CLowlaDBBson::ptr responseBson = lowladb_json_to_bson(json, env->GetArrayLength(response));
    env->ReleaseByteArrayElements(response, (jbyte *)json, 0);
    if (responseBson) {
        CLowlaDBPullData::ptr pd = lowladb_parse_syncer_response(responseBson->data());
        bool keepGoing = pullChunk(env, server, pd, notify);
        while (keepGoing && !env->ExceptionCheck()) {
            keepGoing = pullChunk(env, server, pd, notify);
        }
        if (env->ExceptionCheck()) {
            jstring msg = clearException(env);

            invokeNotify(env, notify, "PULL_ENDED", nullptr);
            invokeNotify(env, notify, "ERROR", msg);
        }
        else {
            invokeNotify(env, notify, "PULL_ENDED", nullptr);
            invokeNotify(env, notify, "OK", nullptr);
        }
    }
    else {
        invokeNotify(env, notify, "PULL_ENDED", nullptr);
        invokeNotify(env, notify, "ERROR", env->NewStringUTF("Unable to parse syncer response"));
    }
}

static void pushChunk(JNIEnv *env, const utf16string &server, CLowlaDBPushData::ptr pd, jobject notify) {
    CLowlaDBBson::ptr pushRequestBson = lowladb_create_push_request(pd);

    utf16string url = server + "/_lowla/push";
    jbyteArray response = doHttp(env, url, pushRequestBson);
    if (!env->ExceptionCheck()) {
        const char *json = (const char *)env->GetByteArrayElements(response, nullptr);
        lowladb_apply_json_push_response(json, pd);
        env->ReleaseByteArrayElements(response, (jbyte *)json, 0);
    }
}

static bool push(JNIEnv *env, const utf16string &server, jobject notify) {
    invokeNotify(env, notify, "PUSH_STARTED", nullptr);

    CLowlaDBPushData::ptr pd = lowladb_collect_push_data();
    while (!pd->isComplete() && !env->ExceptionCheck()) {
        pushChunk(env, server, pd, notify);
    }

    // If there's a pending exception, we need to clear it before we can notify PUSH_ENDED
    if (env->ExceptionCheck()) {
        jstring msg = clearException(env);

        invokeNotify(env, notify, "PUSH_ENDED", nullptr);
        invokeNotify(env, notify, "ERROR", msg);
        return false;
    }
    else {
        invokeNotify(env, notify, "PUSH_ENDED", nullptr);
        return true;
    }
}

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

extern "C" JNIEXPORT void JNICALL
Java_io_lowla_lowladb_LDBClient_sync(JNIEnv *env, jobject jClass, jstring server, jobject notify)
{
    const char *szUtf = env->GetStringUTFChars(server, nullptr);
    utf16string strServer(szUtf);
    env->ReleaseStringUTFChars(server, szUtf);

    if (push(env, strServer, notify)) {
        pull(env, strServer, notify);
    }
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
