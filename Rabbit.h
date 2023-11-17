#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdlib.h>
#include <sstream>

#define N 4

unsigned int A[8] = { 0x4D34D34D, 0xD34D34D3, 0x34D34D34, 0x4D34D34D, 
					  0xD34D34D3, 0x34D34D34, 0x4D34D34D, 0xD34D34D3 };
unsigned int X[8];
unsigned int C[8];
unsigned int K[8];
unsigned int IV[4];
unsigned int G[8];
uint16_t S[8];
unsigned int b;
long long WORDSIZE = 0x100000000;

using namespace std;

template<class T>
string toString(const T& value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

int hexToDec(string hexVal)
{
	int len = hexVal.size();
	int base = 1;
	int decVal = 0;
	for (int i = len - 1; i >= 0; i--) {
		if (hexVal[i] >= '0' && hexVal[i] <= '9') {
			decVal += (int(hexVal[i]) - 48) * base;
			base = base * 16;
		}
		else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') {
			decVal += (int(hexVal[i]) - 55) * base;
			base = base * 16;
		}
	}
	return decVal;
}
string decToHex(unsigned int n)
{
	string temp = "";
	while (n != 0) { 
		int a = 0;
		char c; 
		a = n % 16;
		if (a < 10) c = a + 48; 
		else c = a + 55;
		temp += c;
		n = n / 16;
	}
	int i = 0, j = temp.size() - 1;
	while (i <= j)
	{
		swap(temp[i], temp[j]);
		i++;
		j--;
	}
	return temp;
}

void reset() {
	for (int i = 0; i < 8; i++) 
		X[i] = C[i] = S[i] = 0;
	b = 0;
}
unsigned int g(int u, int v) {
    long long square = u + v & SIZE_MAX;
	square *= square;
	return (unsigned int)(square ^ square >> 32 % WORDSIZE);
}
unsigned int rotate_left(unsigned int value, unsigned int shift) {
	return (shift % 32 == 0) ? value : ((value << shift) | (value >> (32 - shift)));
}
void counterUpdate() {
	long long temp;

	for (int i = 0; i < 8; i++) {
		temp = (C[i] % WORDSIZE) + (A[i] % WORDSIZE) + b;
		b = (unsigned int)(temp >> 32 % WORDSIZE);
		C[i] = (unsigned int)(temp % WORDSIZE);
	}
}
void nextState() {
	counterUpdate();
	for (int i = 0; i < 8; i++) {
		G[i] = g(X[i], C[i]);
	}
	X[0] = G[0] + rotate_left(G[7], 16) + rotate_left(G[6], 16);
	X[1] = G[1] + rotate_left(G[0], 8) + G[7];
	X[2] = G[2] + rotate_left(G[1], 16) + rotate_left(G[0], 16);
	X[3] = G[3] + rotate_left(G[2], 8) + G[1];
	X[4] = G[4] + rotate_left(G[3], 16) + rotate_left(G[2], 16);
	X[5] = G[5] + rotate_left(G[4], 8) + G[3];
	X[6] = G[6] + rotate_left(G[5], 16) + rotate_left(G[4], 16);
	X[7] = G[7] + rotate_left(G[6], 8) + G[5];
}
void keySetup() {
	for (int i = 0; i < 8; i++) {
		if (i % 2) {
			X[i] = (K[(i + 5) % 8] << 16) | K[(i + 4) % 8];
			C[i] = (K[i] << 16) | (K[(i + 1) % 8]);
		}
		else {
			X[i] = (K[(i + 1) % 8] << 16) | K[i];
			C[i] = (K[(i + 4) % 8] << 16) | (K[(i + 5) % 8]);
		}
	}
	nextState();
	nextState();
	nextState();
	nextState();
	for (int i = 0; i < 8; i++)
		C[i] = C[i] ^ X[(i+4) % 8];
}
void IVSetup() {
	C[0] = C[0] ^ IV[0];
	C[1] = C[1] ^ (((IV[1] >> 16) << 16) | ((IV[0] >> 16)));
	C[2] = C[2] ^ IV[1];
	C[3] = C[3] ^ ((IV[1] << 16) | ((IV[0] << 16) >> 16));
	C[4] = C[4] ^ IV[0];
	C[5] = C[5] ^ (((IV[1] >> 16) << 16) | ((IV[0] >> 16)));
	C[6] = C[6] ^ IV[1];
	C[7] = C[7] ^ ((IV[1] << 16) | ((IV[0] << 16) >> 16));
	nextState();
	nextState();
	nextState();
	nextState();
}
uint16_t* nextBlock() {
	nextState();
	S[0] = (uint16_t)(X[0] ^ (X[5] >> 16 % WORDSIZE));
	S[1] = (uint16_t)((X[0] >> 16 % WORDSIZE) ^ X[3]);
	S[2] = (uint16_t)(X[2] ^ (X[7] >> 16 % WORDSIZE));
	S[3] = (uint16_t)(X[2] >> (16 % WORDSIZE ^ X[5]));
	S[4] = (uint16_t)(X[4] ^ (X[1] >> 16 % WORDSIZE));
	S[5] = (uint16_t)((X[4] >> 16 % WORDSIZE) ^ X[7]);
	S[6] = (uint16_t)(X[6] ^ (X[3] >> 16 % WORDSIZE));
	S[7] = (uint16_t)((X[6] >> 16 % WORDSIZE) ^ X[1]);
	return S;
}
void encrypt(string preText, bool usingIV = 1) {
	string cipherText = "";
	reset();
	keySetup();
	if (usingIV) IVSetup();
	for (int i = 0; i < preText.length();) {
		nextState();
		for (int j = 0; j < 8 && i < preText.length(); j += 2, i++) {
			unsigned int temp = (preText[i] - '0') ^ X[j] ^ (X[(j + 5) % 8] >> 16) ^ (X[(j + 3) % 8] << 16);
			cipherText = (decToHex(temp));
			for (int l = 6; l >= 0; l = l - 2) cout << cipherText[l] << cipherText[l + 1] << " ";
		}
	}
	
}