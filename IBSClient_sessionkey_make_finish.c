#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include "IBSClient.h"
#define EXT_GLOBAL
#include "IBSInfo.h"
#include "aes.h"
#include "AES_GCM_Module.h"



void printstring(unsigned char *string, int len)
{
	int i;

	for (i = 0; i < len; i++)
		printf("%02X", string[i]);
	printf("\n");
}

JNIEXPORT jstring JNICALL Java_IBSClient_GetID(JNIEnv *env, jobject obj)
{
	char temp[256] = { 0, };
	memcpy(temp, info_id, ID_len);
	return (*env)->NewStringUTF(env, temp);
}

JNIEXPORT jstring JNICALL Java_IBSClient_GetRand(JNIEnv *env, jobject obj)
{
	BIGNUM *rand = BN_new();
	unsigned char *bin_rand;
	unsigned char hash_rand[32];
//	char temp[32] = { 0, };
	int rand_len = 0;
	jstring ret_rand;

	BN_rand(rand, 224, 1, 1);
	rand_len = BN_num_bytes(rand);

	bin_rand = (unsigned char*)calloc(rand_len, sizeof(unsigned char));
	BN_bn2bin(rand, bin_rand);
	SHA256(bin_rand, rand_len, hash_rand);
	
//	memcpy(temp, hash_rand, 32);

	ret_rand = (*env)->NewStringUTF(env, unsign2sign(hash_rand, 32));

	BN_free(rand);
	free(bin_rand);
//	free(temp);

	return ret_rand;
}

JNIEXPORT jstring JNICALL Java_IBSClient_Sign(JNIEnv *env, jobject obj, jstring msg, jint msglen)
{

	RSA* rsa = RSA_new();
	BIGNUM
		*r = BN_new(),
		*bn_A = BN_new(),
		*g = BN_new(),
		*c = BN_new(),
		*id = BN_new(),
		*rand = BN_new(),
		*z = BN_new(),
		*er = BN_new(),
		*sk_id = BN_new(),
		*c_sqrd_sk = BN_new(),
		*r_sqrd_g = BN_new();
	unsigned char
		*bin_A,
		*bin_z,
		*catAM,
		*catSign,
		C_hashed[SHA256_DIGEST_LENGTH] = { 0, };
	unsigned int A_len, z_len = 0;
	BN_CTX* ctx;
	jstring sign_msg;
	const char *Cmsg = (*env)->GetStringUTFChars(env, msg, NULL);

	//char *Cmsg = bit2char(Cmsg_before, msglen);

	/*
	BN_bin2bn(&(info_n), 2048, rsa->n);
	BN_bin2bn(&(info_e), 24, rsa->e);
	BN_bin2bn(&(info_d), 2048, rsa->d);
	BN_bin2bn(&(info_g), 32, g);
	BN_bin2bn(&(info_sk_id), 2048, sk_id);
	*/
	BN_hex2bn(&(rsa->n), info_n);
	BN_hex2bn(&(rsa->e), info_e);
	BN_hex2bn(&(rsa->d), info_d);
	BN_hex2bn(&g, info_g);
	BN_hex2bn(&(sk_id), info_sk_id);

	/*printf("n in sign\n");
	printf("%s\n",BN_bn2hex(rsa->n) );
*/
	ctx = BN_CTX_new();
	BN_CTX_init(ctx);

	BN_generate_prime_ex(r, 224, 0, NULL, NULL, NULL);//224bit random prime
	rand_client = (unsigned char*)calloc(BN_num_bytes(r), sizeof(unsigned char));
	BN_bn2bin(r, rand_client);
	printf("\n\n");
	printstring(rand_client, BN_num_bytes(r));
	printf("\n\n");
	BN_mod_exp(r_sqrd_g, g, r, rsa->n, ctx);//g^r
	BN_mod_exp(bn_A, r_sqrd_g, rsa->e, rsa->n, ctx);//A=g^er(mod n)

	A_len = BN_num_bytes(bn_A);
	bin_A = (unsigned char*)calloc(A_len, sizeof(unsigned char));
	BN_bn2bin(bn_A, bin_A);

	/*printf("A in sign\n");
	printf("%s", BN_bn2hex(bn_A));
	printf("\n");
*/
	catAM = (unsigned char*)calloc((A_len + msglen), sizeof(unsigned char));
	memcpy(catAM, bin_A, A_len);
	memcpy(catAM + A_len, Cmsg, msglen);//catAM=(A||M)

	SHA256(catAM, A_len + msglen, C_hashed);//C=H(A||M)
	//BN_bin2bn(C_hashed, SHA256_DIGEST_LENGTH, c);
	BN_bin2bn(C_hashed, 32, c);

	BN_mod_exp(c_sqrd_sk, sk_id, c, rsa->n, ctx);//SK(ID)^c(mod n)
	BN_mod_mul(z, c_sqrd_sk, r_sqrd_g, rsa->n, ctx);//z=SK(ID)^c*g^r(mod n)

	z_len = BN_num_bytes(z);
	bin_z = (unsigned char*)calloc(z_len, sizeof(unsigned char));
	BN_bn2bin(z, bin_z);

	catSign = (unsigned char*)calloc(z_len + SHA256_DIGEST_LENGTH, sizeof(unsigned char));
	//itoa(z_len, catSign, 16);
	//itoa(SHA256_DIGEST_LENGTH, catSign + 8, 16);
	memcpy(catSign, bin_z, z_len);
	memcpy(catSign + z_len, C_hashed, SHA256_DIGEST_LENGTH);//catSign=(z||c)

	sign_msg = (*env)->NewStringUTF(env, unsign2sign(catSign, z_len + SHA256_DIGEST_LENGTH));

	BN_free(r);
	BN_free(bn_A);
	BN_free(g);
	BN_free(c);
	BN_free(id);
	BN_free(rand);
	BN_free(z);
	BN_free(er);
	BN_free(sk_id);
	BN_free(c_sqrd_sk);
	BN_free(r_sqrd_g);

	free(bin_A);
	free(bin_z);
	free(catAM);
	free(catSign);
	RSA_free(rsa);

	return sign_msg;
}

JNIEXPORT jboolean JNICALL Java_IBSClient_Verify(JNIEnv *env, jobject obj, jstring msg, jint msglen, jstring chal, jint challen)
{

	RSA* rsa = RSA_new();
	BIGNUM
		*r = BN_new(),
		*bn_A = BN_new(),
		*g = BN_new(),
		*c = BN_new(),
		*z = BN_new(),
		*e_sqrd_z = BN_new(),
		*ivsC_sqrd_h = BN_new(),
		*ivs = BN_new(),
		*bn_hash = BN_new(),
		*h = BN_new(),
		*bn_C_hashed_vrfy = BN_new(),
		*bn_key = BN_new();
	unsigned char
	  *hex_z,
	  *hex_c,
		*bin_A,
		*bin_key,
		*catAM,
		C_hashed_vrfy[SHA256_DIGEST_LENGTH],
		mhash[32],
		EM[256],
                test_n[256],
         	test_z[256],
	        test_c[32],
	        test_ze[256],
	  test_h_inv[256],
	  test_h_inv_c[256],
		bin_c[64],
		bin_z[512];
	       
	unsigned int A_len, z_len, c_len = 0;
	BN_CTX* ctx;
	jboolean bool;
	jstring output;
	const char *Cmsg = (*env)->GetStringUTFChars(env, msg, NULL);
	const char *Chalmsg = (*env)->GetStringUTFChars(env, chal, NULL);


       unsigned char AM[] = "5720B82431A445F1B9ECBA6885DA9758621D36A8D0F18AD2BFEDF97306C23DD3AB88A1601677AAE8FD1FA00012D6678FCBA68DCBE6DDB0D48D595B9D7C350274D99412F777C228A6FFADE89F5FED2348BB0A2E3CBB971692551F260D52866F7D4C3CD512048934DB7B2A6D4E932AAABD74127721EB2B2BD01C04F125B68937A6E5EC8A1058F61FAEA9822ACFBFFBB0FCF834FA01CF24E4961F3319887783E7808A8CE7AA1C0B3CE2F7CAD6C7EB68788D9921F13E03C4E746FFFE61BF67212488F5596C37D39C485D10A27073BB1CDF3D42BDE39DD3F971138A13ACA033A9579CD7C488E1300C0A48A575808BB3DC233AFFEF54826566F77F8B227A9E9FC69002111111111111111111111111111111114133443530353736363634303541464141314433373243413137453139354133463045414436354134374642323146354230354143364430444432363833303432363730303431373435434341344531394236383433423736434646344333334143444643393632463438463934303930384231423435384436444544304141";

unsigned char H_AM[SHA256_DIGEST_LENGTH] = {0,};

	
	//unsigned char *Cmsg = bit2char(Cmsg_before, msglen);
	//unsigned char *Chalmsg = bit2char(Chalmsg_before, challen);
	memcpy(bin_z, Cmsg, 512);
	memcpy(bin_c, Cmsg + 512, 64);

	//hex_z = bit2char(bin_z, 512);
	//hex_c = bit2char(bin_c, 64);

	BN_hex2bn(&z, bin_z);
	BN_hex2bn(&c, bin_c);

	BN_bn2bin(z, test_z);
	BN_bn2bin(c, test_c);
	BN_bin2bn(test_z, 256, z);
	BN_bin2bn(test_c, 32, c);

	
	BN_hex2bn(&(rsa->n), info_n);
	BN_hex2bn(&(rsa->e), info_e);
	BN_hex2bn(&(rsa->d), info_d);
	BN_hex2bn(&g, info_g);

	ctx = BN_CTX_new();
	BN_CTX_init(ctx);

	BN_bn2bin(rsa->n, test_n);

	printf(" z bin / hex : \n");
	printstring(test_z, 256);
	printstring(bin_z, 512);

	printf("\n c bin / hex : \n");
	printstring(test_c, 32);
	printstring(bin_c, 64);

	printf("\n id bin / hex : \n");
	printstring(info_id, ID_len);
	
	printf("\n n bin / hex : \n");
	printstring(test_n, 256);
	printstring(info_n, 512);
	

	/* printf("test string input22 : %s", hello);

	printf("n in veri\n");
	printf("%s\n", BN_bn2hex(rsa->n));

	*/
	BN_mod_exp(e_sqrd_z, z, rsa->e, rsa->n, ctx); //e_sqrd_z = z^e (mod n)
	BN_bn2bin(e_sqrd_z, test_ze);
	printf("\n ze bin / hex : \n");
	printstring(test_ze, 256);
	//printstring(info_n, 512);
	
	SHA256(info_id, ID_len, mhash); // mhash = H(id)
	/*printf("\nSHA256 in veri\n");
	printstring(mhash, 32);*/

	RSA_padding_add_PKCS1_type_1(EM, 256, mhash, 32);
	BN_bin2bn(EM, 256, h);

	BN_mod_inverse(ivs, h, rsa->n, ctx); //ivs = H(id)^-1 (mod n)
        BN_bn2bin(ivs, test_h_inv);
	printf("\n h_inv bin / hex : \n");
	printstring(test_h_inv, 256);
	//printstring(info_n, 512);

	BN_mod_exp(ivsC_sqrd_h, ivs, c, rsa->n, ctx); // ivsC_sqrd_h = (H(id)^-1)^c (mod n)
        BN_bn2bin(ivsC_sqrd_h, test_h_inv_c);
	printf("\n h_inv_c bin / hex : \n");
	printstring(test_h_inv_c, 256);
	
	BN_mod_mul(bn_A, e_sqrd_z, ivsC_sqrd_h, rsa->n, ctx); // bn_A = z^e * H(id)^-c (mod n)

	A_len = BN_num_bytes(bn_A);
	bin_A = (unsigned char*)calloc(A_len, sizeof(unsigned char));
	BN_bn2bin(bn_A, bin_A);

	printf("A length: %d\n", A_len);
        printf("A in veri \n");
	printstring(bin_A, A_len);
	printf("\n");

	catAM = (unsigned char*)calloc(A_len+challen, sizeof(unsigned char));
	memcpy(catAM, bin_A, A_len);
	memcpy(catAM + A_len, Chalmsg, challen);

	printf("A||M\n");
	printstring(catAM, A_len + challen);
	printf("\n");
	printf("challen: %d\n", challen);

	SHA256(catAM, A_len+challen,C_hashed_vrfy);
	BN_bin2bn(C_hashed_vrfy, SHA256_DIGEST_LENGTH, bn_C_hashed_vrfy);

	printf("ceiling: %f\n", 3*ceil((double)(A_len + challen)/2));
	printf("H(A||M) length: %d\n", BN_num_bytes(bn_C_hashed_vrfy));
	printf("\nC_hashed_vrfy in veri\n");
	printstring(C_hashed_vrfy, 32);
	printf("%s\n", BN_bn2hex(c));
	printf("Compared c\n");

	printf("rand client: ");
	
	printstring(rand_client, 28);

	printf("\n\n==============================\n\n");
	SHA256(AM, 800, H_AM);
	printf("\n test hash : ");
	printstring(H_AM, 32);
	printf("\n");
	printf("\n\n==============================\n\n");
	

	BN_free(g);
	BN_free(z);
	BN_free(e_sqrd_z);
	BN_free(ivsC_sqrd_h);
	BN_free(ivs);
	BN_free(bn_hash);
	BN_free(h);
	free(bin_A);
	free(catAM);


	if (BN_cmp(c, bn_C_hashed_vrfy) == 0) {
		BN_bin2bn(rand_client, 28, r);
		BN_mod_exp(bn_key, bn_A, r, rsa->n, ctx);

		bin_key = (unsigned char *)calloc(BN_num_bytes(bn_key), sizeof(unsigned char));
		BN_bn2bin(bn_key, bin_key);
		strncpy(key_client, bin_key, 32);

		printf("key : ");
		printstring(key_client, 32);

		
		BN_free(bn_C_hashed_vrfy);
		BN_free(c);
		BN_free(bn_A);
		BN_free(r);
		BN_free(bn_key);
		BN_CTX_free(ctx);
		RSA_free(rsa);
		free(bin_key);
		return JNI_TRUE;
	}

	else {
		BN_free(bn_C_hashed_vrfy);
		BN_free(c);
		BN_free(bn_A);
		BN_free(r);
		BN_free(bn_key);
		BN_CTX_free(ctx);
		RSA_free(rsa);
		return JNI_FALSE;
	}
	/*
	output = (*env)->NewStringUTF(env, unsign2sign(C_hashed_vrfy, SHA256_DIGEST_LENGTH));

	BN_free(bn_C_hashed_vrfy);
	BN_free(c);

	return output;
*/	
}

JNIEXPORT jstring JNICALL Java_IBSClient_Hash(JNIEnv *env, jobject obj, jstring msg, jint msglen)
{ 
	unsigned char C_hashed[SHA256_DIGEST_LENGTH] = { 0, };
 	jstring sign_msg;
	const char *Cmsg = (*env)->GetStringUTFChars(env ,msg, NULL);
	
	SHA256(Cmsg, msglen, C_hashed);

	sign_msg = (*env)->NewStringUTF(env, unsign2sign(C_hashed, SHA256_DIGEST_LENGTH));
	return sign_msg;
}
