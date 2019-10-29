/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class IBSClient */

#ifndef _Included_IBSClient
#define _Included_IBSClient
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     IBSClient
 * Method:    GetID
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_IBSClient_GetID
  (JNIEnv *, jobject);

/*
 * Class:     IBSClient
 * Method:    GetRand
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_IBSClient_GetRand
  (JNIEnv *, jobject);

/*
 * Class:     IBSClient
 * Method:    Sign
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_IBSClient_Sign
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     IBSClient
 * Method:    Verify
 * Signature: (Ljava/lang/String;ILjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_IBSClient_Verify
  (JNIEnv *, jobject, jstring, jint, jstring, jint);

JNIEXPORT jstring JNICALL Java_IBSClient_Hash
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     IBSClient
 * Method:    AES_GCM_enc
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_IBSClient_AES_1GCM_1enc
  (JNIEnv *, jobject, jstring, jint, jstring);

/*
 * Class:     IBSClient
 * Method:    AES_GCM_dec
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_IBSClient_AES_1GCM_1dec
  (JNIEnv *, jobject, jstring, jint, jstring);

#ifdef __cplusplus
}
#endif
#endif
