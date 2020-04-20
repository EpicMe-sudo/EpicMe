#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "aes.h"

#define MUL2(a) ((a<<1)^(a&0x80?0x1b:0x00))
#define MUL3(a) ((MUL2(a))^(a))
#define MUL4(a) MUL2(MUL2(a))
#define MUL8(a) MUL2(MUL2(MUL2(a)))
#define MUL9(a) ((MUL8(a))^(a))
#define MULB(a) ((MUL8(a))^(MUL2(a))^(a))
#define MULD(a) ((MUL8(a))^(MUL4(a))^(a))
#define MULE(a) ((MUL8(a))^(MUL4(a))^(MUL2(a)))
#define ShiftOneRow(a,b,c,d) { u32 t=a; a=b; b=c; c=d; d=t;}
#define ShiftTwoRow(a,b,c,d) {ShiftOneRow(a,b,c,d); ShiftOneRow(a,b,c,d);}
#define ShiftThreeRow(a,b,c,d) {ShiftTwoRow(a,b,c,d); ShiftOneRow(a,b,c,d);}

u8 MULJH(u8 a, u8 b) {
	u8 r = 0;
	u8 index = 0x01;
	for (int i = 0;i < 8;i++) {
		r = (a & index ? b : r);
		b = MUL2(b);
		index *= 2;
	}
	return r;
}

u8 MUL(u8 a, u8 b) {
	u8 r = 0;
	u8 tmp = b;
	u32 i;
	for (i = 0;i < 8;i++) {
		if (a & 1) r ^= tmp;
		tmp = MUL2(tmp);
		a >>= 1;
	}
	return r;
}

u8 INV(u8 a) {

	u8 r = a;
	r = MUL(r, r); //a2
	r = MUL(r, a); //a3
	r = MUL(r, r); //a6
	r = MUL(r, a); //a7
	r = MUL(r, r); //a14
	r = MUL(r, a); //a15
	r = MUL(r, r); //a30
	r = MUL(r, a); //a31
	r = MUL(r, r); //a62
	r = MUL(r, a); //a63
	r = MUL(r, r); //a126
	r = MUL(r, a); //a127
	r = MUL(r, r); //a254
	return r;
}

u8 GenSbox(u8 a) {

	u8 r = 0;
	u8 tmp;
	tmp = INV(a);
	if (tmp & 1) r ^= 0x1f;
	if (tmp & 2)r ^= 0x3e;
	if (tmp & 4)r ^= 0x7c;
	if (tmp & 8)r ^= 0xf8;
	if (tmp & 16) r ^= 0xf1;
	if (tmp & 32)r ^= 0xe3;
	if (tmp & 64)r ^= 0xc7;
	if (tmp & 128)r ^= 0x8f;
	return r ^ 0x63;
}

u8 GenSboxJH(u8 a) {
	
	u8 r = 0;
	u8 tmp;
	tmp = INV(a);
	r = tmp ^ ((tmp << 1) ^ (tmp >> 7)) ^ ((tmp << 2) ^ (tmp >> 6)) ^ ((tmp << 3) ^ (tmp >> 5)) ^ ((tmp << 4) ^ (tmp >> 4)) ^ 0x63;
	// Calculate Rijndael Affine Matrix "tmp^(tmp<<<1)^(tmp<<<2)^(tmp<<<3)^(tmp<<<4)" in this case "<<<" means bitwise circular shift.
	return r;
}

u8 GenInvSboxJH(u8 a) {
	u8 r = 0;
	u8 tmp = a;
	tmp = ((tmp << 1) ^ (tmp >> 7)) ^ ((tmp << 3) ^ (tmp >> 5)) ^ ((tmp << 6) ^ (tmp >> 2)) ^ 0x05;
	r = INV(tmp);
	return r;
}

void AddRoundKey(u8 temp[16], u8 RK[16]){

	for (int i = 0;i < 16;i++) temp[i] ^= RK[i];
	
}

void SubBytes(u8 temp[16]) {

	for (int i = 0;i < 16;i++) temp[i] = Sbox[temp[i]];

}

void ShiftRows(u8 temp[16]) {

	ShiftOneRow(temp[1], temp[5], temp[9], temp[13]);
	ShiftTwoRow(temp[2], temp[6], temp[10], temp[14]);
	ShiftThreeRow(temp[3], temp[7], temp[11], temp[15]);

}

void MixColumns(u8 temp[16]) {

	u8 Con[16];
	for (int i = 0;i < 16;i +=4) {
		Con[i] = MUL2(temp[i]) ^ MUL3(temp[i + 1]) ^ temp[i + 2] ^ temp[i + 3];
		Con[i + 1] = temp[i] ^ MUL2(temp[i + 1]) ^ MUL3(temp[i + 2]) ^ temp[i + 3];
		Con[i + 2] = temp[i] ^ temp[i + 1] ^ MUL2(temp[i + 2]) ^ MUL3(temp[i + 3]);
		Con[i + 3] = MUL3(temp[i]) ^ temp[i + 1] ^ temp[i + 2] ^ MUL2(temp[i + 3]);
	}
	temp[0] = Con[0]; temp[1] = Con[1];temp[2] = Con[2];temp[3] = Con[3];
	temp[4] = Con[4]; temp[5] = Con[5];temp[6] = Con[6];temp[7] = Con[7];
	temp[8] = Con[8]; temp[9] = Con[9];temp[10] = Con[10];temp[11] = Con[11];
	temp[12] = Con[12]; temp[13] = Con[13];temp[14] = Con[14];temp[15] = Con[15];
}

//AES_ENC FUNCTION
void AES_ENC(u8 PT[16], u8 RK[], u8 CT[16], int keysize) {
	
	int Nr = keysize / 32 + 6;
	int i;
	u8 temp[16];

	for (i = 0;i < 16;i++) temp[i] = PT[i];

	AddRoundKey(temp, RK); //temp의 16byte와 RK의 첫 16byte를 xor하여 temp에 결과를 담는 함수

	for (i = 0;i < Nr - 1;i++) {
		SubBytes(temp);
		ShiftRows(temp);
		//CheckFile
		printf("TextCheck[%d]:\n",10-i);
		for (int j = 0;j < 16;j++) printf("%02x ", temp[j]);
		printf("\n\n");
		//Please Erase me after the work done!
		MixColumns(temp);
		AddRoundKey(temp, RK + 16 * (i + 1));
	}

	SubBytes(temp);
	ShiftRows(temp);
	//CheckFile
	printf("TextCheck[1]:\n");
	for (int j = 0;j < 16;j++) printf("%02x ", temp[j]);
	//Please Erase me after the work done!
	printf("\n\n");
	AddRoundKey(temp, RK + 16 * (i + 1));

	for (i = 0;i < 16;i++) CT[i] = temp[i];

}


u32 u4byte_in(u8* x) {

	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];

}

void u4byte_out(u8* x, u32 y) {

	x[0] = (y >> 24) & 0xff;
	x[1] = (y >> 16) & 0xff;
	x[2] = (y >> 8) & 0xff;
	x[3] = y & 0xff;
}

void AES_ENC_Optimization(u8 PT[16], u32 W[], u8 CT[16], int keysize) {

	int Nr = keysize / 32 + 6;
	int i;
	u32 s0, s1, s2, s3, t0, t1, t2, t3;

	// round 0
	s0 = u4byte_in(PT)^W[0];
	s1 = u4byte_in(PT+4)^W[1];
	s2 = u4byte_in(PT + 8)^W[2];
	s3 = u4byte_in(PT + 12)^W[3];

	// round 1 
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[4];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[5];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[6];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[7];

	// round 2 
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[8];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[9];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[10];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[11];

	// round 3 
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[12];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[13];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[14];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[15];

	// round 4 
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[16];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[17];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[18];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[19];

	// round 5 
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[20];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[21];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[22];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[23];

	// round 6 
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[24];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[25];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[26];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[27];

	// round 7 
	t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[28];
	t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[29];
	t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[30];
	t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[31];

	// round 8 
	s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[32];
	s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[33];
	s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[34];
	s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[35];

	if (Nr == 10) {

		// round 9 
		t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[36];
		t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[37];
		t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[38];
		t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[39];

		// round 10 
		s0 = (Te2[t0 >> 24] & 0xff000000) ^ (Te3[(t1 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t2 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t3) & 0xff] & 0x000000ff) ^ W[40];
		s1 = (Te2[t1 >> 24] & 0xff000000) ^ (Te3[(t2 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t3 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t0) & 0xff] & 0x000000ff) ^ W[41];
		s2 = (Te2[t2 >> 24] & 0xff000000) ^ (Te3[(t3 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t0 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t1) & 0xff] & 0x000000ff) ^ W[42];
		s3 = (Te2[t3 >> 24] & 0xff000000) ^ (Te3[(t0 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t1 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t2) & 0xff] & 0x000000ff) ^ W[43];

	}
	else if (Nr == 12) {

		// round 9 
		t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[36];
		t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[37];
		t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[38];
		t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[39];

		// round 10 
		s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[40];
		s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[41];
		s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[42];
		s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[43];

		// round 11 
		t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[44];
		t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[45];
		t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[46];
		t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[47];

		// round 12 
		s0 = (Te2[t0 >> 24] & 0xff000000) ^ (Te3[(t1 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t2 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t3) & 0xff] & 0x000000ff) ^ W[48];
		s1 = (Te2[t1 >> 24] & 0xff000000) ^ (Te3[(t2 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t3 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t0) & 0xff] & 0x000000ff) ^ W[49];
		s2 = (Te2[t2 >> 24] & 0xff000000) ^ (Te3[(t3 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t0 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t1) & 0xff] & 0x000000ff) ^ W[50];
		s3 = (Te2[t3 >> 24] & 0xff000000) ^ (Te3[(t0 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t1 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t2) & 0xff] & 0x000000ff) ^ W[51];

	}

	else {

		// round 9 
		t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[36];
		t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[37];
		t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[38];
		t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[39];

		// round 10 
		s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[40];
		s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[41];
		s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[42];
		s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[43];

		// round 11 
		t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[44];
		t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[45];
		t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[46];
		t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[47];

		// round 12 
		s0 = Te0[t0 >> 24] ^ Te1[(t1 >> 16) & 0xff] ^ Te2[(t2 >> 8) & 0xff] ^ Te3[t3 & 0xff] ^ W[48];
		s1 = Te0[t1 >> 24] ^ Te1[(t2 >> 16) & 0xff] ^ Te2[(t3 >> 8) & 0xff] ^ Te3[t0 & 0xff] ^ W[49];
		s2 = Te0[t2 >> 24] ^ Te1[(t3 >> 16) & 0xff] ^ Te2[(t0 >> 8) & 0xff] ^ Te3[t1 & 0xff] ^ W[50];
		s3 = Te0[t3 >> 24] ^ Te1[(t0 >> 16) & 0xff] ^ Te2[(t1 >> 8) & 0xff] ^ Te3[t2 & 0xff] ^ W[51];

		// round 13 
		t0 = Te0[s0 >> 24] ^ Te1[(s1 >> 16) & 0xff] ^ Te2[(s2 >> 8) & 0xff] ^ Te3[s3 & 0xff] ^ W[52];
		t1 = Te0[s1 >> 24] ^ Te1[(s2 >> 16) & 0xff] ^ Te2[(s3 >> 8) & 0xff] ^ Te3[s0 & 0xff] ^ W[53];
		t2 = Te0[s2 >> 24] ^ Te1[(s3 >> 16) & 0xff] ^ Te2[(s0 >> 8) & 0xff] ^ Te3[s1 & 0xff] ^ W[54];
		t3 = Te0[s3 >> 24] ^ Te1[(s0 >> 16) & 0xff] ^ Te2[(s1 >> 8) & 0xff] ^ Te3[s2 & 0xff] ^ W[55];

		// round 14 
		s0 = (Te2[t0 >> 24] & 0xff000000) ^ (Te3[(t1 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t2 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t3) & 0xff] & 0x000000ff) ^ W[56];
		s1 = (Te2[t1 >> 24] & 0xff000000) ^ (Te3[(t2 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t3 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t0) & 0xff] & 0x000000ff) ^ W[57];
		s2 = (Te2[t2 >> 24] & 0xff000000) ^ (Te3[(t3 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t0 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t1) & 0xff] & 0x000000ff) ^ W[58];
		s3 = (Te2[t3 >> 24] & 0xff000000) ^ (Te3[(t0 >> 16) & 0xff] & 0x00ff0000) ^ (Te0[(t1 >> 8) & 0xff] & 0x0000ff00) ^ (Te1[(t2) & 0xff] & 0x000000ff) ^ W[59];

	}
	u4byte_out(CT, s0);
	u4byte_out(CT+4, s1);
	u4byte_out(CT+8, s2);
	u4byte_out(CT+12, s3);

}

void AES_KeyWordToByte(u32 W[], u8 RK[]) {

	for (int i = 0;i < 44;i++) {
		u4byte_out(RK + 4 * i, W[i]);
	}

}

u32 Rcons[10] = { 0x01000000, 0x02000000,0x04000000,0x08000000,0x10000000,0x20000000,0x40000000,0x80000000,0x1b000000,0x36000000 };

#define RotWord(x) ((x<<8)|(x>>24))

/*
u32 SubWord(u32 x) {

	return (Sbox[(u8)(x >> 24) & 0xff] << 24) | (Sbox[(u8)(x >> 16) & 0xff] << 16) | (Sbox[(u8)(x >> 8) & 0xff]) | (Sbox[(u8)(x) & 0xff]);

}
*/

#define SubWord(x) (Sbox[(u8)(x >> 24) & 0xff] << 24) | (Sbox[(u8)(x >> 16) & 0xff] << 16) | (Sbox[(u8)(x >> 8) & 0xff]<<8) | (Sbox[(u8)(x) & 0xff])

void RoundkeyGeneration128(u8 MK[], u8 RK[]) {

	u32 W[44];
	u32 T;

	W[0] = u4byte_in(MK); //W[0]=MK[0] || MK[1] || MK[2] || MK[3]
	W[1] = u4byte_in(MK + 4);
	W[2] = u4byte_in(MK + 8);
	W[3] = u4byte_in(MK + 12);

	for (int i = 0;i < 10;i++) {
		//T = G_func(W[4 * i + 3]);
		T = W[4 * i + 3];
		T = RotWord(T);
		T = SubWord(T);
		T = T ^ Rcons[i];
		
		W[4 * i + 4] = W[4 * i] ^ T;
		W[4 * i + 5] = W[4 * i + 1] ^ W[4 * i + 4];
		W[4 * i + 6] = W[4 * i + 2] ^ W[4 * i + 5];
		W[4 * i + 7] = W[4 * i + 3] ^ W[4 * i + 6];
	}
	 AES_KeyWordToByte(W, RK); //This is for 8bit
}

void RoundkeyGeneration128_Optimazation(u8 MK[], u32 W[]) {

	u32 T;

	W[0] = u4byte_in(MK); //W[0]=MK[0] || MK[1] || MK[2] || MK[3]
	W[1] = u4byte_in(MK + 4);
	W[2] = u4byte_in(MK + 8);
	W[3] = u4byte_in(MK + 12);

	for (int i = 0;i < 10;i++) {
		//T = G_func(W[4 * i + 3]);
		T = W[4 * i + 3];
		T = RotWord(T);
		T = SubWord(T);
		T = T ^ Rcons[i];

		W[4 * i + 4] = W[4 * i] ^ T;
		W[4 * i + 5] = W[4 * i + 1] ^ W[4 * i + 4];
		W[4 * i + 6] = W[4 * i + 2] ^ W[4 * i + 5];
		W[4 * i + 7] = W[4 * i + 3] ^ W[4 * i + 6];
	}
}

void AES_KeySchedule(u8 MK[], u8 RK[], int keysize) {

	if (keysize == 128)RoundkeyGeneration128(MK, RK);
	//if (keysize==192)RoundkeyGeneration192(MK,RK);
	//if (keysize==256)RoundkeyGeneration256(MK,RK);
}
 
void AES_KeySchedule_Optimization(u8 MK[], u32 W[], int keysize) {

	if (keysize == 128)RoundkeyGeneration128_Optimazation(MK, W);
	//if (keysize==192)RoundkeyGeneration192(MK,W);
	//if (keysize==256)RoundkeyGeneration256(MK,W);

}

void RoundkeyGeneration128_Inversed(u8 MK[], u8 RK[],u32 W[]) {

	u32 T;

	W[43-3] = u4byte_in(MK); //W[0]=MK[0] || MK[1] || MK[2] || MK[3]
	W[43-2] = u4byte_in(MK + 4);
	W[43-1] = u4byte_in(MK + 8);
	W[43-0] = u4byte_in(MK + 12);

	for (int i = 0;i < 10;i++) {
		//T = G_func(W[4 * i + 3]);
		T = W[43 - 4 * i - 0];
		T = RotWord(T);
		T = SubWord(T);
		T = T ^ Rcons[i];

		W[43-4 * i -7] = W[43-4*i-3] ^ T;
		W[43 - 4 * i - 6] = W[43 - 4 * i - 2] ^ W[43 - 4 * i - 7];
		W[43 - 4 * i - 5] = W[43 - 4 * i - 1] ^ W[43 - 4 * i - 6];
		W[43 - 4 * i - 4] = W[43 - 4 * i - 0] ^ W[43 - 4 * i - 5];
	}
	AES_KeyWordToByte(W, RK); //This is for 8bit
}

void AES_MixColmned_AddRoundKey_Inversed_Optimization(u8 MK[],u32 InvW[],int keysize) {
	u8 RK[240] = { 0x00, };
	if (keysize == 128) {
		RoundkeyGeneration128_Inversed(MK, RK, InvW);
		for (int i = 1;i < 10;i++) {
			InvW[4 * i] = InvMixKey0[RK[16 * i]] ^ InvMixKey1[RK[16 * i + 1]] ^ InvMixKey2[RK[16 * i + 2]] ^ InvMixKey3[RK[16 * i + 3]];
			InvW[4 * i + 1] = InvMixKey0[RK[16 * i + 4]] ^ InvMixKey1[RK[16 * i + 5]] ^ InvMixKey2[RK[16 * i + 6]] ^ InvMixKey3[RK[16 * i + 7]];
			InvW[4 * i + 2] = InvMixKey0[RK[16 * i + 8]] ^ InvMixKey1[RK[16 * i + 9]] ^ InvMixKey2[RK[16 * i + 10]] ^ InvMixKey3[RK[16 * i + 11]];
			InvW[4 * i + 3] = InvMixKey0[RK[16 * i + 12]] ^ InvMixKey1[RK[16 * i + 13]] ^ InvMixKey2[RK[16 * i + 14]] ^ InvMixKey3[RK[16 * i + 15]];
		}
	}
}


void AES_DEC_Optimization(u8 CT[16], u32 InvW[], u8 DEC[16], int keysize) {

	int Nr = keysize / 32 + 6;
	int i;
	u32 s0, s1, s2, s3, t0, t1, t2, t3;

	// round 0
	s0 = u4byte_in(CT) ^ InvW[0];
	s1 = u4byte_in(CT + 4) ^ InvW[1];
	s2 = u4byte_in(CT + 8) ^ InvW[2];
	s3 = u4byte_in(CT + 12) ^ InvW[3];

	// round 1 
	t0 = InvTe0[s0 >> 24] ^ InvTe1[(s3 >> 16) & 0xff] ^ InvTe2[(s2 >> 8) & 0xff] ^ InvTe3[s1 & 0xff] ^ InvW[4];
	t1 = InvTe0[s1 >> 24] ^ InvTe1[(s0 >> 16) & 0xff] ^ InvTe2[(s3 >> 8) & 0xff] ^ InvTe3[s2 & 0xff] ^ InvW[5];
	t2 = InvTe0[s2 >> 24] ^ InvTe1[(s1 >> 16) & 0xff] ^ InvTe2[(s0 >> 8) & 0xff] ^ InvTe3[s3 & 0xff] ^ InvW[6];
	t3 = InvTe0[s3 >> 24] ^ InvTe1[(s2 >> 16) & 0xff] ^ InvTe2[(s1 >> 8) & 0xff] ^ InvTe3[s0 & 0xff] ^ InvW[7];

	// round 2 
	s0 = InvTe0[t0 >> 24] ^ InvTe1[(t3 >> 16) & 0xff] ^ InvTe2[(t2 >> 8) & 0xff] ^ InvTe3[t1 & 0xff] ^ InvW[8];
	s1 = InvTe0[t1 >> 24] ^ InvTe1[(t0 >> 16) & 0xff] ^ InvTe2[(t3 >> 8) & 0xff] ^ InvTe3[t2 & 0xff] ^ InvW[9];
	s2 = InvTe0[t2 >> 24] ^ InvTe1[(t1 >> 16) & 0xff] ^ InvTe2[(t0 >> 8) & 0xff] ^ InvTe3[t3 & 0xff] ^ InvW[10];
	s3 = InvTe0[t3 >> 24] ^ InvTe1[(t2 >> 16) & 0xff] ^ InvTe2[(t1 >> 8) & 0xff] ^ InvTe3[t0 & 0xff] ^ InvW[11];

	// round 3
	t0 = InvTe0[s0 >> 24] ^ InvTe1[(s3 >> 16) & 0xff] ^ InvTe2[(s2 >> 8) & 0xff] ^ InvTe3[s1 & 0xff] ^ InvW[12];
	t1 = InvTe0[s1 >> 24] ^ InvTe1[(s0 >> 16) & 0xff] ^ InvTe2[(s3 >> 8) & 0xff] ^ InvTe3[s2 & 0xff] ^ InvW[13];
	t2 = InvTe0[s2 >> 24] ^ InvTe1[(s1 >> 16) & 0xff] ^ InvTe2[(s0 >> 8) & 0xff] ^ InvTe3[s3 & 0xff] ^ InvW[14];
	t3 = InvTe0[s3 >> 24] ^ InvTe1[(s2 >> 16) & 0xff] ^ InvTe2[(s1 >> 8) & 0xff] ^ InvTe3[s0 & 0xff] ^ InvW[15];

	// round 4
	s0 = InvTe0[t0 >> 24] ^ InvTe1[(t3 >> 16) & 0xff] ^ InvTe2[(t2 >> 8) & 0xff] ^ InvTe3[t1 & 0xff] ^ InvW[16];
	s1 = InvTe0[t1 >> 24] ^ InvTe1[(t0 >> 16) & 0xff] ^ InvTe2[(t3 >> 8) & 0xff] ^ InvTe3[t2 & 0xff] ^ InvW[17];
	s2 = InvTe0[t2 >> 24] ^ InvTe1[(t1 >> 16) & 0xff] ^ InvTe2[(t0 >> 8) & 0xff] ^ InvTe3[t3 & 0xff] ^ InvW[18];
	s3 = InvTe0[t3 >> 24] ^ InvTe1[(t2 >> 16) & 0xff] ^ InvTe2[(t1 >> 8) & 0xff] ^ InvTe3[t0 & 0xff] ^ InvW[19];

	// round 5 
	t0 = InvTe0[s0 >> 24] ^ InvTe1[(s3 >> 16) & 0xff] ^ InvTe2[(s2 >> 8) & 0xff] ^ InvTe3[s1 & 0xff] ^ InvW[20];
	t1 = InvTe0[s1 >> 24] ^ InvTe1[(s0 >> 16) & 0xff] ^ InvTe2[(s3 >> 8) & 0xff] ^ InvTe3[s2 & 0xff] ^ InvW[21];
	t2 = InvTe0[s2 >> 24] ^ InvTe1[(s1 >> 16) & 0xff] ^ InvTe2[(s0 >> 8) & 0xff] ^ InvTe3[s3 & 0xff] ^ InvW[22];
	t3 = InvTe0[s3 >> 24] ^ InvTe1[(s2 >> 16) & 0xff] ^ InvTe2[(s1 >> 8) & 0xff] ^ InvTe3[s0 & 0xff] ^ InvW[23];

	// round 6 
	s0 = InvTe0[t0 >> 24] ^ InvTe1[(t3 >> 16) & 0xff] ^ InvTe2[(t2 >> 8) & 0xff] ^ InvTe3[t1 & 0xff] ^ InvW[24];
	s1 = InvTe0[t1 >> 24] ^ InvTe1[(t0 >> 16) & 0xff] ^ InvTe2[(t3 >> 8) & 0xff] ^ InvTe3[t2 & 0xff] ^ InvW[25];
	s2 = InvTe0[t2 >> 24] ^ InvTe1[(t1 >> 16) & 0xff] ^ InvTe2[(t0 >> 8) & 0xff] ^ InvTe3[t3 & 0xff] ^ InvW[26];
	s3 = InvTe0[t3 >> 24] ^ InvTe1[(t2 >> 16) & 0xff] ^ InvTe2[(t1 >> 8) & 0xff] ^ InvTe3[t0 & 0xff] ^ InvW[27];

	// round 7
	t0 = InvTe0[s0 >> 24] ^ InvTe1[(s3 >> 16) & 0xff] ^ InvTe2[(s2 >> 8) & 0xff] ^ InvTe3[s1 & 0xff] ^ InvW[28];
	t1 = InvTe0[s1 >> 24] ^ InvTe1[(s0 >> 16) & 0xff] ^ InvTe2[(s3 >> 8) & 0xff] ^ InvTe3[s2 & 0xff] ^ InvW[29];
	t2 = InvTe0[s2 >> 24] ^ InvTe1[(s1 >> 16) & 0xff] ^ InvTe2[(s0 >> 8) & 0xff] ^ InvTe3[s3 & 0xff] ^ InvW[30];
	t3 = InvTe0[s3 >> 24] ^ InvTe1[(s2 >> 16) & 0xff] ^ InvTe2[(s1 >> 8) & 0xff] ^ InvTe3[s0 & 0xff] ^ InvW[31];

	// round 8 
	s0 = InvTe0[t0 >> 24] ^ InvTe1[(t3 >> 16) & 0xff] ^ InvTe2[(t2 >> 8) & 0xff] ^ InvTe3[t1 & 0xff] ^ InvW[32];
	s1 = InvTe0[t1 >> 24] ^ InvTe1[(t0 >> 16) & 0xff] ^ InvTe2[(t3 >> 8) & 0xff] ^ InvTe3[t2 & 0xff] ^ InvW[33];
	s2 = InvTe0[t2 >> 24] ^ InvTe1[(t1 >> 16) & 0xff] ^ InvTe2[(t0 >> 8) & 0xff] ^ InvTe3[t3 & 0xff] ^ InvW[34];
	s3 = InvTe0[t3 >> 24] ^ InvTe1[(t2 >> 16) & 0xff] ^ InvTe2[(t1 >> 8) & 0xff] ^ InvTe3[t0 & 0xff] ^ InvW[35];

	if (Nr == 10) {
		
		// round 9
		t0 = InvTe0[s0 >> 24] ^ InvTe1[(s3 >> 16) & 0xff] ^ InvTe2[(s2 >> 8) & 0xff] ^ InvTe3[s1 & 0xff] ^ InvW[36];
		t1 = InvTe0[s1 >> 24] ^ InvTe1[(s0 >> 16) & 0xff] ^ InvTe2[(s3 >> 8) & 0xff] ^ InvTe3[s2 & 0xff] ^ InvW[37];
		t2 = InvTe0[s2 >> 24] ^ InvTe1[(s1 >> 16) & 0xff] ^ InvTe2[(s0 >> 8) & 0xff] ^ InvTe3[s3 & 0xff] ^ InvW[38];
		t3 = InvTe0[s3 >> 24] ^ InvTe1[(s2 >> 16) & 0xff] ^ InvTe2[(s1 >> 8) & 0xff] ^ InvTe3[s0 & 0xff] ^ InvW[39];

		// round 10 
		s0 = ((u32)InvSbox[t0 >> 24] << 24) ^ ((u32)InvSbox[(t3 >> 16) & 0xff] << 16) ^ ((u32)InvSbox[(t2 >> 8) & 0xff] << 8) ^ ((u32)InvSbox[t1 & 0xff]) ^ InvW[40];
		s1 = ((u32)InvSbox[t1 >> 24] << 24) ^ ((u32)InvSbox[(t0 >> 16) & 0xff] << 16) ^ ((u32)InvSbox[(t3 >> 8) & 0xff] << 8) ^ ((u32)InvSbox[t2 & 0xff]) ^ InvW[41];
		s2 = ((u32)InvSbox[t2 >> 24] << 24) ^ ((u32)InvSbox[(t1 >> 16) & 0xff] << 16) ^ ((u32)InvSbox[(t0 >> 8) & 0xff] << 8) ^ ((u32)InvSbox[t3 & 0xff]) ^ InvW[42];
		s3 = ((u32)InvSbox[t3 >> 24] << 24) ^ ((u32)InvSbox[(t2 >> 16) & 0xff] << 16) ^ ((u32)InvSbox[(t1 >> 8) & 0xff] << 8) ^ ((u32)InvSbox[t0 & 0xff]) ^ InvW[43];
	}
	else if (Nr == 12) {


	}

	else {


	}
	u4byte_out(DEC, s0);
	u4byte_out(DEC + 4, s1);
	u4byte_out(DEC + 8, s2);
	u4byte_out(DEC + 12, s3);

}

void ECB_Encryption(char* inputfile, char* outputfile, u32 W[]) {

	FILE* rfp, * wfp;
	u8* inputbuf, * outputbuf, r;
	u32 DataLen;

	rfp=fopen(inputfile, "rb");
	if (rfp == NULL) {
		perror("fopen_s Failed!\n");
	}

	fseek(rfp, 0, SEEK_END);
	DataLen = ftell(rfp); //the size of inputfile(in bytes)
	fseek(rfp, 0, SEEK_SET);

	r = DataLen % 16;
	r = 16 - r; //the number of bytes sould be PKCS #7 padded

	inputbuf = calloc(DataLen + r, sizeof(u8));
	outputbuf = calloc(DataLen + r, sizeof(u8));
	fread(inputbuf, 1, DataLen, rfp);
	fclose(rfp);
	memset(inputbuf + DataLen, r, r);

	for (int i = 0;i < (DataLen + r) / 16;i++) {
		AES_ENC_Optimization(inputbuf + 16 * i, W, outputbuf + 16 * i, 128);
	}

	wfp=fopen(outputfile, "wb");
	if (wfp == NULL) {
		perror("fopen_s Failed!\n");
	}
	fwrite(outputbuf, 1, DataLen + r, wfp);
	fclose(wfp);
}

void XOR16Bytes(u8 S[16], u8 RK[16]) { //S=S xor RK

	S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] ^= RK[3];
	S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] ^= RK[7];
	S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] ^= RK[11];
	S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15];

}

void CBC_Encryption(char* inputfile, char* outputfile, u32 W[]) {

	FILE* rfp, * wfp;
	u8* inputbuf, * outputbuf, r;
	u8 IV[16] = { 0x00, };
	u32 DataLen;

	rfp=fopen(inputfile, "rb");
	if (rfp == NULL) {
		perror("fopen_s Failed!\n");
	}

	fseek(rfp, 0, SEEK_END);
	DataLen = ftell(rfp); //the size of inputfile(in bytes)
	fseek(rfp, 0, SEEK_SET);

	r = DataLen % 16;
	r = 16 - r; //the number of bytes sould be PKCS #7 padded

	inputbuf = calloc(DataLen + r, sizeof(u8));
	outputbuf = calloc(DataLen + r, sizeof(u8));
	fread(inputbuf, 1, DataLen, rfp);
	fclose(rfp);
	memset(inputbuf + DataLen, r, r);

	XOR16Bytes(inputbuf, IV);
	AES_ENC_Optimization(inputbuf, W, outputfile, 128);

	for (int i = 1;i < (DataLen + r) / 16;i++) {
		XOR16Bytes(inputbuf + 16 * i, outputbuf + 16 * (i - 1));
		AES_ENC_Optimization(inputbuf + 16 * i, W, outputbuf + 16 * i, 128);
	} 
	
	wfp=fopen(outputfile, "wb");
	if (wfp == NULL) {
		perror("fopen_s Failed!\n");
	}
	fwrite(outputbuf, 1, DataLen + r, wfp);
	fclose(wfp);

}

void CBC_Decryption(char* inputfile, char* outputfile, u32 InvW[]) {

	FILE* rfp, * wfp;
	u8* inputbuf, * outputbuf, check;
	u8 IV[16] = { 0x00, };
	u32 DataLen;

	rfp=fopen(inputfile, "rb");
	if (rfp == NULL) {
		perror("fopen_s Failed!\n");
	}

	fseek(rfp, 0, SEEK_END);
	DataLen = ftell(rfp); //the size of inputfile(in bytes)
	fseek(rfp, 0, SEEK_SET);

	inputbuf = calloc(DataLen, sizeof(u8));
	outputbuf = calloc(DataLen, sizeof(u8));
	fread(inputbuf, 1, DataLen, rfp);
	fclose(rfp);
	
	AES_DEC_Optimization(inputbuf, InvW, outputbuf, 128);
	XOR16Bytes(outputbuf, IV);

	for (int i = 1;i < DataLen / 16;i++) {
		AES_DEC_Optimization(inputbuf + 16 * i, InvW, outputbuf + 16 * i, 128);
		XOR16Bytes(outputbuf + 16 * i, inputbuf + 16 * (i - 1));
	}

	check = outputbuf[DataLen - 1];

	wfp=fopen(outputfile, "wb");
	if (wfp == NULL) {
		perror("fopen_s Failed!\n");
	}
	fwrite(outputbuf, 1, DataLen - check, wfp);
	fclose(wfp);

}

int main(int argc,char* argv[]) {

	
	u8 a, b, c, d;
	int i;
	u8 temp;
	u8 PT[16] = { 0x00,0x11,0X22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff };
	u8 MK[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	u8 CT[16] = { 0x00, };
	u8 DEC[16] = { 0x00, };
	u8 RK[240] = { 0x00, };
	u32 W[60] = { 0x00, }; 
	u32 InvW[60] = { 0x00, };
	const char* ECB = "ecb";
	clock_t start, finish;

	int keysize = 128;

	if (strcmp(argv[1], "ecb") == 0) {
		printf("Passing Gate 2...\n");
		AES_KeySchedule_Optimization(MK, W, keysize);
		ECB_Encryption(argv[2], argv[3], W);
	}
	else if (strcmp(argv[1], "cbc") == 0) {
		AES_KeySchedule_Optimization(MK, W, keysize);
		CBC_Encryption(argv[2], argv[3], W);
	}
	
	if (strcmp(argv[1], "cbcdec") == 0) {
		AES_MixColmned_AddRoundKey_Inversed_Optimization(MK, InvW, keysize);
		CBC_Decryption(argv[2], argv[3], InvW);
	}

	////////////////////////////////////////////////////////////////////////////////////////////

	/*
	AES_KeySchedule(MK, RK, keysize); // 1round:RK 0~15, 2round 16~31
	start = clock();
	for (i = 0;i < 100000;i++)AES_ENC(PT, RK, PT, keysize);
	finish = clock();
	printf("AES:\n");
	for (i = 0;i < 16;i++)printf("%02x ", PT[i]);
	printf("\n");
	printf("Computation time : %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
	printf("\n=================================\n\n");
	*/
	
	////////////////////////////////////////////////////////////////////////////////////////////

	/*
	printf("AES PlainText:\n");
	for (i = 0;i < 16;i++)printf("%02x ", PT[i]);
	printf("\n\n");

	//
	AES_KeySchedule(MK, RK, keysize);
	AES_ENC(PT, RK, CT, keysize);

	printf("AES:\n");
	for (i = 0;i < 16;i++) printf("%02x ", CT[i]);
	printf("\n\n");
	
	//
	AES_KeySchedule_Optimization(MK, W, keysize);
	AES_ENC_Optimization(PT, W, CT, keysize);

	printf("AES Optimization:\n");
	for (i = 0;i < 16;i++)printf("%02x ", CT[i]);
	printf("\n\n");

	//
	AES_MixColmned_AddRoundKey_Inversed_Optimization(MK, InvW, keysize);
	AES_DEC_Optimization(CT, InvW, DEC, keysize);

	printf("AES DEC Optimization:\n");
	for (i = 0;i < 16;i++)printf("%02x ", DEC[i]);
		
	//MixColumned AddRoundKey Table (Inverse AES)
	
	*/

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	printf("u32 InvMixKey0[256] = {\n");
	for(int i = 0;i < 256;i++) {
		printf("0x%02x%02x%02x%02x, ", (u8)MULE(i), (u8)MUL9(i), (u8)MULD(i), (u8)MULB(i));
		if (i % 8 == 7) printf("\n");
	}
	printf("};\n");

	printf("u32 InvMixKey1[256] = {\n");
	for (int i = 0;i < 256;i++) {
		printf("0x%02x%02x%02x%02x, ", (u8)MULB(i), (u8)MULE(i), (u8)MUL9(i), (u8)MULD(i));
		if (i % 8 == 7) printf("\n");
	}
	printf("};\n");

	printf("u32 InvMixKey2[256] = {\n");
	for (int i = 0;i < 256;i++) {
		printf("0x%02x%02x%02x%02x, ", (u8)MULD(i), (u8)MULB(i), (u8)MULE(i), (u8)MUL9(i));
		if (i % 8 == 7) printf("\n");
	}
	printf("};\n");

	printf("u32 InvMixKey3[256] = {\n");
	for (int i = 0;i < 256;i++) {
		printf("0x%02x%02x%02x%02x, ", (u8)MUL9(i), (u8)MULD(i), (u8)MULB(i), (u8)MULE(i));
		if (i % 8 == 7) printf("\n");
	}
	printf("};\n");
	*/
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////

		
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//ShiftRow SubByte MixColumn Optimization (Inverse AES)
	/*
	printf("u32 InvTe0[256] = {\n");
	for(int i = 0;i < 256;i++) {
		temp = InvSbox[i];
		printf("0x%02x%02x%02x%02x, ", (u8)MULE(temp), (u8)MUL9(temp), (u8)MULD(temp), (u8)MULB(temp));
		if (i % 8 == 7) printf("\n");
	}
	printf("};");

	printf("u32 InvTe1[256] = {\n");
	for (int i = 0;i < 256;i++) {
		temp = InvSbox[i];
		printf("0x%02x%02x%02x%02x, ", (u8)MULB(temp), (u8)MULE(temp), (u8)MUL9(temp), (u8)MULD(temp));
		if (i % 8 == 7) printf("\n");
	}
	printf("};");

	printf("u32 InvTe2[256] = {\n");
	for (int i = 0;i < 256;i++) {
		temp = InvSbox[i];
		printf("0x%02x%02x%02x%02x, ", (u8)MULD(temp), (u8)MULB(temp), (u8)MULE(temp), (u8)MUL9(temp));
		if (i % 8 == 7) printf("\n");
	}
	printf("};");

	printf("u32 InvTe3[256] = {\n");
	for (int i = 0;i < 256;i++) {
		temp = InvSbox[i];
		printf("0x%02x%02x%02x%02x, ", (u8)MUL9(temp), (u8)MULD(temp), (u8)MULB(temp), (u8)MULE(temp));
		if (i % 8 == 7) printf("\n");
	}
	printf("};");
	*/
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Sbox ShiftRow MixColumn 고속화
	/*
	printf("u32 Te0[256]={\n");
	for (i = 0;i < 256;i++) {
		temp = Sbox[i];
		printf("0x%02x%02x%02x%02x, ",(u8)MUL2(temp), temp, temp, (u8)MUL3(temp));
		if (i % 8 == 7)printf("\n");
	}
	printf("};");
	

	printf("u32 Te1[256]={\n");
	for (i = 0;i < 256;i++) {
		temp = Sbox[i];
		printf("0x%02x%02x%02x%02x, ", (u8)MUL3(temp),(u8)MUL2(temp), temp, temp);
		if (i % 8 == 7)printf("\n");
	}
	printf("};");

	printf("\n");

	printf("u32 Te2[256]={\n");
	for (i = 0;i < 256;i++) {
		temp = Sbox[i];
		printf("0x%02x%02x%02x%02x, ",temp,(u8)MUL3(temp),(u8)MUL2(temp), temp);
		if (i % 8 == 7)printf("\n");
	}
	printf("};");

	printf("\n");

	printf("u32 Te3[256]={\n");
	for (i = 0;i < 256;i++) {
		temp = Sbox[i];
		printf("0x%02x%02x%02x%02x, ", temp, temp, (u8)MUL3(temp),(u8)MUL2(temp));
		if (i % 8 == 7)printf("\n");
	}
	printf("};");
	*/

	////////////////////////////////////////////////////////////////////////////////////////

	/* 
	b = 0X38;
	c = MUL(a, b);
	a = 0xab;
	d = 8;

	//printf("%02x * %02x = %02x", a, b, c);
	printf("Sbox(%02x) = %02x, %02x\n", a, GenSboxJH(a), Sbox[a]);
	printf("Sbox[256]={\n");
	for (i = 0;i < 256;i++) {
		printf("0x%02x, ", GenSboxJH((u8)i));
		if (i % 16 == 15) printf("\n");
	}
	printf("}\n");

	printf("InvSbox[256]={\n");
	for (i = 0;i < 256;i++) {
		printf("0x%02x, ", GenInvSboxJH((u8)i));
		if (i % 16 == 15) printf("\n");
	}
	printf("}\n");
	*/

	return 0;
}