#include "Rabbit.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <vector>
#include <stdio.h>
#include <string>

#define _CRT_SECURE_NO_DEPRECATE

using namespace std;
string s;
string rawK[16];
string rawIV[8];
bool useIV = 0;
int main(int argc, char const* argv[])
{
	freopen("input.in", "r", stdin);
	freopen("output.out", "w", stdout);
	getline(cin, s);
	for (int i = 0; i < 16; i++) 
		cin >> rawK[i];
	for (int i = 0; i < 16; i = i + 2) {
		K[i/2] = hexToDec(rawK[i] + rawK[i + 1]);
	}
	cin.ignore();
	getline(cin, s);
	for (int i = 0; i < 8; i++)
		cin >> rawIV[i];
	for (int i = 0; i < 4; i = i + 2) {
		IV[i / 2] = hexToDec(rawIV[i] + rawIV[i + 1]);
	}
	cin.ignore();
	getline(cin, s);
	cin >> useIV;
	cin.ignore();
	getline(cin, s);
	getline(cin, s);
	encrypt(s, useIV);
	return 0;
}