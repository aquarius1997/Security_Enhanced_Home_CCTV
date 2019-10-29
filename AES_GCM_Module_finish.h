#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>




//X dot Y alogrithm


////////////////////수정 -> aes.h/////////////
void mulBlock(unsigned char X[], unsigned char Y[], unsigned char R[], unsigned char Z[])
{
	//unsigned char X[16]= {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	//unsigned char Y[16]= {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	//unsigned char Z[16]= {0,};
	unsigned char V[16] = { 0, };
	unsigned char tmp = 0x00;
	//unsigned char R[16]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; // Random block;
	int i = 0, j = 0, k = 0;

	for (i = 0; i< 16; i++) V[i] = Y[i];

	for (i = 0; i < 128; i++) {

		if ((char)(X[i / 8] >> (7 - (i % 8)))&(0x01) == 0x01)
			for (j = 0; j < 16; j++) Z[j] = (char)(Z[j] ^ V[j]);

		tmp = V[15];
		for (j = 0; j < 16; j++)
			V[j] = (char)(V[j] >> 1) | (char)(V[j] << 7);
		for (j = 0; j < 15; j++)
			V[14 - j + 1] = (char)(V[14 - j] & 0x80) | (char)(V[14 - j + 1] & 0x7f);
		V[0] = (char)(V[0] & 0x7f);// 논리시프트?
		if ((char)(tmp & 0x01) == 0x01) {
			for (j = 0; j < 16; j++)
				V[j] = (char)(V[j] ^ R[j]);
		}

	}//return Z_128;

	/*
	//  printf("%d\n", k);
	for(k= 0; k< 16; k++)
	printf("%02X ", V[k]);
	printf("\n");
	*/
}

void ghash(unsigned char* A, unsigned char* C, unsigned char* H, unsigned int A_len, unsigned int C_len, unsigned char* hash)
{
	unsigned char R[16] = { 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char Y[16] = { 0, };
	unsigned char block[16] = { 0, };
	unsigned char test[2048] = { 0, };
	unsigned char hello[16] = { 0, };
	int i = 0, j = 0, k = 0, u = 0, v = 0;

	/*for (i = 0; i * 16<A_len; i++);
	u = i * 16 - A_len;
	for (i = 0; i * 16<C_len; i++);
	v = i * 16 - C_len;

	printf("%d\n", A_len);

	for (i = 0; i< A_len; i++) test[i] = A[i];
	for (i = 0; i< u; i++) test[A_len + i] = 0x00;
	for (i = 0; i< C_len; i++) test[A_len + u + i] = C[i];
	for (i = 0; i< v; i++) test[A_len + u + C_len + i] = 0x00;*/

	u = 16 - A_len % 16;
	v = 16 - C_len % 16;

	memcpy(test, A, A_len);
	memset(test + A_len, 0, u);
	memcpy(test + A_len + u, C, C_len);
	memset(test + A_len + u + C_len, 0, v);

	for (i = 1, j = 1, k = 0; A_len * 8 > j * 256; i++, j *= 256); k = i;
	for (i = 0; i< 8 - k; i++) test[A_len + u + C_len + v + i] = 0x00;
	for (i = 0; i< k; i++) test[A_len + u + C_len + v + 8 - k + i] = (A_len * 8 >> (k - i - 1) * 8);

	for (i = 1, j = 1, k = 0; C_len * 8 > j * 256; i++, j *= 256); k = i;
	for (i = 0; i< 8 - k; i++) test[A_len + u + C_len + v + 8 + i] = 0x00;
	for (i = 0; i< k; i++) test[A_len + u + C_len + v + 8 + 8 - k + i] = (C_len * 8 >> (k - i - 1) * 8);

	/*
	printf("GHASH input array : ");
	for(i= 0; i< A_len+u+C_len+v+8+8; i++)
	//hash[i]= Y[i];
	printf("%02X ", test[i]);
	printf("\nGHASH input size : %d\n",  A_len+u+C_len+v+8+8);
	*/

	//ghash
	for (j = 0; j< 16; j++) block[j] = test[j] ^ hello[j];
	mulBlock(block, H, R, Y);
	//  for(i= 0; i< ((sizeof(test)/16)-1); i++){
	for (i = 0; i< (((A_len + u + C_len + v + 8 + 8) / 16) - 1); i++) {
		for (j = 0; j< 16; j++) block[j] = (char)(test[16 * (i + 1) + j] ^ Y[j]);
		for (j = 0; j< 16; j++) Y[j] = 0x00;
		mulBlock(block, H, R, Y);
	}

	//printf("ghash : ");
	for (i = 0; i< 16; i++)
		hash[i] = Y[i];
	//printf("%02X ", Y[i]);
	//printf("\n");
	ghash_count++;

}


void inc32(unsigned char* IV, unsigned int IV_len, unsigned char* ICB, unsigned char* A, unsigned char* C, unsigned int A_len, unsigned int C_len)
{
	unsigned int s = 0, i = 0, j = 0;
	unsigned char Jout[16] = { 0, };

	if (inc_count == 0) {
		if (IV_len == 12) {
			for (i = 0; i< 12; i++) ICB[i] = IV[i];
			ICB[15] += 0x01;
			inc_count++;
		}
		else {
			if (IV_len % 16 == 0) s = 16 * (IV_len / 16) - IV_len;
			else s = 16 * (IV_len / 16 + 1) - IV_len;

			/*
			J= (char *)calloc(IV_len+ s+ 8+ 8, sizeof(char));
			for(i= 0; i< IV_len; i++) J[i]= IV[i];
			for(i= 0; i< s+8; i++) J[IV_len+i]= 0x00;
			for(i= 0; i< 7; i++) J[IV_len +s +8 +i]= 0x00;
			J[IV_len +s +15]= (char)IV_len;
			*/
			//first = (char *)calloc(IV_len, sizeof(char));
			//for (i = 0; i< IV_len; i++) first[i] = IV[i];

			//ghash() with ICB here;
			ghash(NULL, IV, H, 0, IV_len, Jout);

			for (i = 0; i< 16; i++) ICB[i] = Jout[i];
			inc_count++;
		}
	}
	else {
		if ((unsigned char)(IV[IV_len - 1] + 0x01) == 0x00) {
			inc32(IV, IV_len - 1, ICB, NULL, NULL, 0, 0);
			IV[IV_len - 1] = 0x00;
		}
		else {
			IV[IV_len - 1] = (unsigned char)(IV[IV_len - 1] + 0x01);
		}
	}
}

void GCTR_Encryption(unsigned char* plainText, unsigned char* cipherText, unsigned char* ekey, int arrlen, unsigned char* IV)
{
	int i = 0, j = 0, k = 0, last_length = 0;
	unsigned char* pt = plainText;
	unsigned char* ct = cipherText;
	unsigned long* key = (unsigned long*)ekey;
	unsigned char block[16] = { 0, };
	unsigned char ct_im[16] = { 0, };
	unsigned int hello = 0;

	if (arrlen % 16 == 0) {
		hello = 0;
		last_length = 16;
	}
	else {
		hello = 1;
		last_length = arrlen % 16;
	}

	if (GCTR_count == 0) {
		for (i = 0; i<16; i++) block[i] = IV[i];

		aes_encrypt(block, ct, key, 128);
	}

	// block XOR operation with pre-encrypted block & Encryption using AES_128
	else {
		for (i = 0; i< arrlen / 16 + hello; i++) {
			if (ghash_count != 2)
				inc32(IV, 16, NULL, NULL, NULL, 0, 0);
			for (j = 0; j< 16; j++) {
				block[j] = block[j] & 0x00;
				block[j] = IV[j];
				ct_im[j] = ct_im[j] & 0x00;
			}
			//Encrypt(block, ct_im, key);
			aes_encrypt(block, ct_im, key, 128);

			if (i == arrlen / 16) {
				for (k = 0; k < last_length; k++) {
					ct[16 * i + k] = ct_im[k] ^ pt[16 * i + k];
				}
			}
			else {
				for (k = 0; k < 16; k++)
					ct[16 * i + k] = ct_im[k] ^ pt[16 * i + k];
			}
		}
	}
	GCTR_count++;
}


char* unsign2sign(unsigned char* msg, int msglen)
{
	char *signmsg;
	int i;

	signmsg = malloc(msglen * 2 + 1);
	for (i = 0; i<msglen; i++) {
		sprintf(&signmsg[i * 2], "%02X", msg[i]);
	}

	return signmsg;
}

unsigned char* bit2char(unsigned char* before, int len)
{
	unsigned char* after = (char *)calloc(len / 2, sizeof(char));
	int i;
	unsigned char pre, nxt;

	for (i = 0; i < len / 2; i++){
		if ((before[i * 2] >= '0') && (before[i * 2] <= '9')) pre = (unsigned char)(before[i * 2] - 0x30);
		else pre = (unsigned char)(before[i * 2] - 0x37);

		if ((before[i * 2 + 1] >= '0') && (before[i * 2 + 1] <= '9')) nxt = (unsigned char)(before[i * 2 + 1] - 0x30);
		else nxt = (unsigned char)(before[i * 2 + 1] - 0x37);

		after[i] = (unsigned char)((unsigned char)(pre << 4) | nxt);
		//after[i] = ( before[i * 2] -30 ) << 4 | ( before[i * 2 + 1] - 30 );
	}
	return after;
}

/*void inc32(unsigned char* IV, unsigned int IV_len, unsigned char* ICB)*/
JNIEXPORT jstring JNICALL Java_IBSServer_AES_1GCM_1enc(JNIEnv *env, jobject obj, jstring Plaintext, jint textlen)
{  
   unsigned char *plaintext; 
   unsigned char *ciphertext;
   //unsigned char key[16] = { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 };
   unsigned char IV[8] = { 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad };
   unsigned char ICB[16] = { 0, };
   unsigned char J[16] = { 0, };
   unsigned char A[20] = { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xda, 0xd2 };
   unsigned char hash[16] = { 0, };
   unsigned char T[16] = { 0, };
   unsigned char empty_arr[16] = { 0, };
   unsigned int ekey[44] = { 0, };
   unsigned int A_len = 0;
   unsigned int C_len = 0;
   unsigned int IV_len = 0;
   unsigned int arrlen = 0;
   int i = 0, j = 0, k = 0, u = 0, v = 0, block = 7;
   
   plaintext = (*env)->GetStringUTFChars(env, Plaintext, NULL);
   ciphertext = (unsigned char *)calloc(textlen, sizeof(unsigned char));

      KeyExpansion(key_server, (unsigned long *)ekey, 128);//키 확장

      GCTR_Encryption(empty_arr, H, (unsigned char*)ekey, /*(unsigned int)sizeof(key)*/16, empty_arr);//H값 생성

      IV_len = sizeof(IV);
      inc32(IV, IV_len, ICB, A, ciphertext, A_len, C_len);//inc32 func. ICB(=J0)값 생성
      for (i = 0; i < 16; i++) J[i] = ICB[i];
      if (IV_len == 12) ghash_count++;

	  GCTR_Encryption(plaintext, ciphertext, (unsigned char*)ekey, (unsigned int)/*sizeof(plaintext)*/textlen, ICB);
  
      A_len = sizeof(A);
	  C_len = textlen;//sizeof(ciphertext);
      ghash(A, ciphertext, H, A_len, C_len, hash);//GHASH(H, A, C)

      GCTR_Encryption(hash, T, (unsigned char*)ekey, (unsigned int)sizeof(hash), J);

      GCTR_count = 0;
      ghash_count = 0;
      inc_count = 0;

   return (*env)->NewStringUTF(env, unsign2sign(ciphertext, textlen));
}

JNIEXPORT jstring JNICALL Java_IBSClient_AES_1GCM_1enc(JNIEnv *env, jobject obj, jstring Plaintext, jint textlen, jstring SessionKey)
{
	
	unsigned char *plaintext; 
	unsigned char *ciphertext;
    //unsigned char key[16] = { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 };
	unsigned char *sessionkey;	//옜
	unsigned char IV[8] = { 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad };
	unsigned char ICB[16] = { 0, };
	unsigned char J[16] = { 0, };
	unsigned char A[20] = { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xda, 0xd2 };
	unsigned char hash[16] = { 0, };
	unsigned char T[16] = { 0, };
	unsigned char empty_arr[16] = { 0, };
	unsigned int ekey[44] = { 0, };
	unsigned int A_len = 0;
	unsigned int C_len = 0;
	unsigned int IV_len = 0;
	unsigned int arrlen = 0;
	int i = 0, j = 0, k = 0, u = 0, v = 0, block = 7;
	

	plaintext = (*env)->GetStringUTFChars(env, Plaintext, NULL);
	sessionkey = (*env)->GetStringUTFChars(env, SessionKey, NULL); //
	ciphertext = (unsigned char *)calloc(textlen, sizeof(unsigned char));

//	KeyExpansion(key_client, (unsigned long *)ekey, 128);//키 확장
	
	KeyExpansion(sessionkey, (unsigned long*) ekey, 128); //

	GCTR_Encryption(empty_arr, H, (unsigned char*)ekey, /*(unsigned int)sizeof(key)*/16, empty_arr);//H값 생성

	IV_len = sizeof(IV);
	inc32(IV, IV_len, ICB, A, ciphertext, A_len, C_len);//inc32 func. ICB(=J0)값 생성
	for (i = 0; i < 16; i++) J[i] = ICB[i];
	if (IV_len == 12) ghash_count++;

	GCTR_Encryption(plaintext, ciphertext, (unsigned char*)ekey, (unsigned int)/*sizeof(plaintext)*/textlen, ICB);

	A_len = sizeof(A);
	C_len = textlen;// sizeof(ciphertext);
	ghash(A, ciphertext, H, A_len, C_len, hash);//GHASH(H, A, C)

	GCTR_Encryption(hash, T, (unsigned char*)ekey, (unsigned int)sizeof(hash), J);

	GCTR_count = 0;
	ghash_count = 0;
	inc_count = 0;

	return (*env)->NewStringUTF(env, unsign2sign(ciphertext, textlen));
}


///////
JNIEXPORT jstring JNICALL Java_IBSClient_AES_1GCM_1dec(JNIEnv *env, jobject obj, jstring Ciphertext, jint Textlen)
{
	unsigned char *plaintext;
	unsigned char *ciphertext; 
	//unsigned char key[16] = { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 };
	unsigned char IV[8] = { 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad };
	unsigned char ICB[16] = { 0, };
	unsigned char J[16] = { 0, };
	unsigned char A[20] = { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xda, 0xd2 };
	unsigned char hash[16] = { 0, };
	unsigned char T[16] = { 0x36, 0x12, 0xd2, 0xe7, 0x9e, 0x3b, 0x07, 0x85, 0x56, 0x1b, 0xe1, 0x4a, 0xac, 0xa2, 0xfc, 0xcb };
	unsigned char T_cmp[17] = { 0, };
	unsigned char empty_arr[16] = { 0, };
	unsigned int ekey[44] = { 0, };
	unsigned int A_len = sizeof(A);
	unsigned int C_len = 0;//sizeof(ciphertext);
	unsigned int IV_len = 16;
	unsigned int arrlen = 0;
	int i = 0, j = 0, k = 0, u = 0, v = 0, f = 0;
	int textlen = Textlen / 2;
	unsigned char *tempstr;

	tempstr = (*env)->GetStringUTFChars(env, Ciphertext, NULL);
	plaintext = (unsigned char *)calloc(textlen, sizeof(unsigned char));
	ciphertext = bit2char(tempstr, Textlen);


	printf("ciphertext : ");
	for (f = 0; f<textlen; f++){
	  printf("%02X ",ciphertext[f]);
	}
	printf("\n\n");
	
	
	KeyExpansion(key_client, (unsigned long*)ekey, 128);

	GCTR_Encryption(empty_arr, H, (unsigned char*)ekey, /*(unsigned int)sizeof(key)*/16, empty_arr);

	for (f= 0; f< 16; f++){
	  printf("%02X ", H[f]);
	}
	printf("\n\n");

	
	IV_len = sizeof(IV);
	inc32(IV, IV_len, ICB, A, ciphertext, A_len, C_len);

	if (IV_len == 12) ghash_count++;

	C_len = (unsigned int)textlen;//sizeof(ciphertext);
	ghash(A, ciphertext, H, A_len, C_len, hash);
	for (f= 0; f< 16; f++){
	  printf("%02X ",hash[f]);
	}
	printf("\n\n");
	
	GCTR_Encryption(hash, T_cmp, (unsigned char*)ekey, 16, J);

	for (f= 0; f< 16; f++){
	  printf("%02X ",T_cmp[f]);
	}
	printf("\n\n");

	
	ghash_count--;

	GCTR_Encryption(ciphertext, plaintext, (unsigned char*)ekey, (unsigned int)/*sizeof(ciphertext)*/textlen , ICB);
	
	
	printf("plaintext : ");
	for (f = 0; f<textlen; f++){
	  printf("%02X ",plaintext[f]);
	}
	printf("\n\n");

	
	
	GCTR_count = 0;
	ghash_count = 0;
	inc_count = 0;

	return (*env)->NewStringUTF(env, unsign2sign(plaintext, textlen));
}

JNIEXPORT jstring JNICALL Java_IBSServer_AES_1GCM_1dec(JNIEnv *env, jobject obj, jstring Ciphertext, jint Textlen)
{
	unsigned char *plaintext;
	unsigned char *ciphertext; 
	//unsigned char key[16] = { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 };
	unsigned char IV[8] = { 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad };
	unsigned char ICB[16] = { 0, };
	unsigned char J[16] = { 0, };
	unsigned char A[20] = { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xda, 0xd2 };
	unsigned char hash[16] = { 0, };
	unsigned char T[16] = { 0x36, 0x12, 0xd2, 0xe7, 0x9e, 0x3b, 0x07, 0x85, 0x56, 0x1b, 0xe1, 0x4a, 0xac, 0xa2, 0xfc, 0xcb };
	unsigned char T_cmp[17] = { 0, };
	unsigned char empty_arr[16] = { 0, };
	unsigned int ekey[44] = { 0, };
	unsigned int A_len = sizeof(A);
	unsigned int C_len = 0;// sizeof(ciphertext);
	unsigned int IV_len = 16;
	unsigned int arrlen = 0;
	int i = 0, j = 0, k = 0, u = 0, v = 0;
	int textlen = Textlen / 2;
	unsigned char *tempstr;
	
	tempstr = (*env)->GetStringUTFChars(env, Ciphertext, NULL);
	plaintext = (unsigned char *)calloc(textlen, sizeof(unsigned char));
	ciphertext = bit2char(tempstr, Textlen);

	
	
	KeyExpansion(key_server, (unsigned long*)ekey, 128);

	GCTR_Encryption(empty_arr, H, (unsigned char*)ekey, /*(unsigned int)sizeof(key)*/16, empty_arr);

	IV_len = sizeof(IV);
	inc32(IV, IV_len, ICB, A, ciphertext, A_len, C_len);

	if (IV_len == 12) ghash_count++;

	C_len = (unsigned int)textlen;//sizeof(ciphertext);
	ghash(A, ciphertext, H, A_len, C_len, hash);

	GCTR_Encryption(hash, T_cmp, (unsigned char*)ekey, 16, J);

	ghash_count--;

	GCTR_Encryption(ciphertext, plaintext, (unsigned char*)ekey, (unsigned int)/*sizeof(ciphertext)*/textlen, ICB);


	
	GCTR_count = 0;
	ghash_count = 0;
	inc_count = 0;

	return (*env)->NewStringUTF(env, unsign2sign(plaintext, textlen));
}
