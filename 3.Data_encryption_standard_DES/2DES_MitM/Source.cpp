/*
######################################################
Introduction to Criptography - 2021 - Homework3 (Part B)
Last modification (24-Mar-2021)

Author: Gaina Robert-Adrian
Contact: gainarobertadrian@gmail.com

 Meet-in-the-Middle over 2DES 
######################################################

Resources:
1. https://profs.info.uaic.ro/~siftene/DES.pdf
2. https://profs.info.uaic.ro/~siftene/DES-Boxes.html
3. https://www.geeksforgeeks.org/data-encryption-standard-des-set-1/
4. https://ro.wikipedia.org/wiki/Data_Encryption_Standard
5. http://cacr.uwaterloo.ca/hac/about/chap7.pdf
6. https://www.freeswan.org/freeswan_trees/freeswan-1.5/doc/DES.html
7. http://sconce.ics.uci.edu/134-S11/LEC5.pdf
*/
#include <iostream>
#include<string>
#include <map>
#include <vector>
#include <time.h>
#include <algorithm>
using namespace std;

/* Initial permutation box */
int IP[] = {
	58,50,42,34,26,18,10, 2,
	60,52,44,36,28,20,12, 4,
	62,54,46,38,30,22,14, 6,
	64,56,48,40,32,24,16, 8,
	57,49,41,33,25,17, 9, 1,
	59,51,43,35,27,19,11, 3,
	61,53,45,37,29,21,13, 5,
	63,55,47,39,31,23,15, 7
};

/* Inverse of the initial permutation box (IP*IP_inverse=Ascending matrix) */
int IP_inverse[] = {
	40, 8,48,16,56,24,64,32,
	39, 7,47,15,55,23,63,31,
	38, 6,46,14,54,22,62,30,
	37, 5,45,13,53,21,61,29,
	36, 4,44,12,52,20,60,28,
	35, 3,43,11,51,19,59,27,
	34, 2,42,10,50,18,58,26,
	33, 1,41, 9,49,17,57,25
};

/* Expansion function */
int E[] = {
	32, 1, 2, 3, 4, 5,
	 4, 5, 6, 7, 8, 9,
	 8, 9,10,11,12,13,
	12,13,14,15,16,17,
	16,17,18,19,20,21,
	20,21,22,23,24,25,
	24,25,26,27,28,29,
	28,29,30,31,32, 1
};

/* Permutation box for the key */
int PC1[] = {
	57,49,41,33,25,17, 9,
	 1,58,50,42,34,26,18,
	10, 2,59,51,43,35,27,
	19,11, 3,60,52,44,36,
	63,55,47,39,31,23,15,
	 7,62,54,46,38,30,22,
	14, 6,61,53,45,37,29,
	21,13, 5,28,20,12, 4
};

/* Permutation box for the key */
int PC2[] = {
	14,17,11,24, 1, 5,
	 3,28,15, 6,21,10,
	23,19,12, 4,26, 8,
	16, 7,27,20,13, 2,
	41,52,31,37,47,55,
	30,40,51,45,33,48,
	44,49,39,56,34,53,
	46,42,50,36,29,32
};

/* 8 S-boxes of 4 rows and 16 columns */
int S[8][4][16]{
	{
		{14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7},
		{ 0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8},
		{ 4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0},
		{15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13}
	},
	{
		{15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10},
		{ 3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5},
		{ 0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15},
		{13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9}
	},
	{
		{10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8},
		{13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1},
		{13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7},
		{ 1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12}
	},
	{
		{ 7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15},
		{13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9},
		{10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4},
		{ 3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14}
	},
	{
		{ 2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9},
		{14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6},
		{ 4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14},
		{11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3}
	},
	{
		{12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11},
		{10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8},
		{ 9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6},
		{ 4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13}
	},
	{
		{ 4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1},
		{13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6},
		{ 1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2},
		{ 6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12}
	},
	{
		{13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7},
		{ 1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2},
		{ 7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8},
		{ 2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11}
	}
};

/* Fixed permutation for the f function */
int P[]{
	16, 7,20,21,
	29,12,28,17,
	 1,15,23,26,
	 5,18,31,10,
	 2, 8,24,14,
	32,27, 3, 9,
	19,13,30, 6,
	22,11, 4,25
};

string previous_key;

vector<string> key_list;

vector<string> round_keys;

/* Compute the initial permutation of the plaintext */
string computeIP(string text) {
	string auxText = "";
	for (int i = 0; i < 64; ++i) {
		auxText += text[IP[i] - 1];
	}
	return auxText;
}

/* Auxiliary function for converting hexadecimal data to binary format
(16character length hexa to 64bits)*/
string hex2bin(string text) {
	map<char, string> m;
	m['0'] = "0000";
	m['1'] = "0001";
	m['2'] = "0010";
	m['3'] = "0011";
	m['4'] = "0100";
	m['5'] = "0101";
	m['6'] = "0110";
	m['7'] = "0111";
	m['8'] = "1000";
	m['9'] = "1001";
	m['A'] = "1010";
	m['B'] = "1011";
	m['C'] = "1100";
	m['D'] = "1101";
	m['E'] = "1110";
	m['F'] = "1111";
	string binary = "";
	for (int i = 0; i < text.size(); ++i) {
		binary += m[text[i]];
	}
	return binary;
}

/*
Xor table:
a b result
0 0  0
0 1  1
1 0  1
1 1  1
*/
string Xor(string a, string b) {
	string ans = "";
	for (int i = 0; i < a.size(); ++i) {
		if (a[i] != b[i]) {
			ans += '1';
		}
		else {
			ans += '0';
		}
	}
	return ans;
}

/* After conversion to binary each 8th bith is discarded*/
string hexKey2binKey(string text) {
	string aux = hex2bin(text);
	string ans = "";
	for (int i = 0; i < aux.size(); ++i) {
		if ((i + 1) % 8 != 0) {
			ans += aux[i];
		}
	}
	return ans;
}

/* Auxiliary function for splitting a string
!!!Size must be text/2*/
pair<string, string> split(string text, int size) {
	pair <string, string> ans;
	ans.first = text.substr(0, size);
	ans.second = text.substr(size, size);
	return ans;
}

/* Auxiliary function for converting binary string to hexadecimal format
(64bits to 16hexa characters)*/
string bin2hex(string text) {
	map<string, string> m;
	m["0000"] = "0";
	m["0001"] = "1";
	m["0010"] = "2";
	m["0011"] = "3";
	m["0100"] = "4";
	m["0101"] = "5";
	m["0110"] = "6";
	m["0111"] = "7";
	m["1000"] = "8";
	m["1001"] = "9";
	m["1010"] = "A";
	m["1011"] = "B";
	m["1100"] = "C";
	m["1101"] = "D";
	m["1110"] = "E";
	m["1111"] = "F";
	string hexa = "";
	for (int i = 0; i < text.size(); i += 4) {
		string character = "";
		for (int j = 0; j < 4; ++j) {
			character += text[i + j];
		}
		hexa += m[character];
	}
	return hexa;
}

/* Shift a binary number moving the first bit from the left to the end of the string*/
string leftShift(string n) {
	string aux = n.substr(1, n.size() - 1);
	aux += n[0];
	return aux;
}

/* Compute the initial permutation of the key */
string initialKeyPermutation(string J) {
	string permutedKey = "";
	for (int i = 0; i < 56; ++i) {
		permutedKey += J[PC1[i] - 1];
	}
	key_list.push_back(permutedKey);
	return permutedKey;
}

/* Compute the roundKey for the n'th round
!!! uses the key_list array in order to get the previous CiDi string
How to compute the key: 
We have noted initial 64-bit key is transformed into a 56-bit key by discarding every 8th bit of the initial key. 
Thus, for each a 56-bit key is available. From this 56-bit key, a different 48-bit Sub Key is generated during each round using a process called as key transformation. 
For this the 56 bit key is divided into two halves, each of 28 bits. These halves are circularly shifted left by one or two positions, depending on the round.
For example, if the round number 1, 2, 9 or 16 the shift is done by only position for other rounds, the circular shift is done by two positions. 
*/
string computeRoundKey(int roundNumber) {
	pair<string, string> keyParts = split(key_list[roundNumber - 1], 28);
	string LeftKey = keyParts.first;
	string RightKey = keyParts.second;
	if (roundNumber == 1 || roundNumber == 2 || roundNumber == 9 || roundNumber == 16) {
		LeftKey = leftShift(LeftKey);
		RightKey = leftShift(RightKey);
	}
	else {
		LeftKey = leftShift(leftShift(LeftKey));
		RightKey = leftShift(leftShift(RightKey));
	}
	string shiftedKey = LeftKey + RightKey;
	key_list.push_back(shiftedKey);
	previous_key = shiftedKey;
	string roundKey = "";
	for (int i = 0; i < 48; ++i) {
		roundKey += shiftedKey[PC2[i] - 1];
	}
	round_keys.push_back(roundKey);
	return roundKey;
}

/*Split the XOR between expanded text and round key into 8 6-bit strings*/
vector<string> splitB(string B) {
	vector<string> ans;
	for (int i = 0; i < B.size(); i += 6) {
		string aux = "";
		for (int j = 0; j < 6; ++j) {
			aux += B[i + j];
		}
		ans.push_back(aux);
	}
	return ans;
}

/* Convert a int into a binary string representation
#Since there are only 16 numbers we can hardcode the values into a single switch
#Faster than the classic algorithm of conversion*/
string decToBinary(int n) {
	switch (n) {
	case 0:
		return "0000";
	case  1:
		return "0001";
	case  2:
		return "0010";
	case  3:
		return "0011";
	case  4:
		return "0100";
	case  5:
		return "0101";
	case  6:
		return "0110";
	case  7:
		return "0111";
	case  8:
		return "1000";
	case  9:
		return "1001";
	case 10:
		return "1010";
	case 11:
		return "1011";
	case 12:
		return "1100";
	case 13:
		return "1101";
	case 14:
		return "1110";
	case 15:
		return "1111";
	}
}

/* From a B-component with 6 bits b1b2b3b4b5b6:
b1b6 - column
b2b3b4b5 - row
Return an element from the S-boxes.*/
string SElement(string a, int boxNumber) {
	string columnBin = "";
	columnBin += a[0]; columnBin += a[a.size() - 1];
	string rowBin = a.substr(1, 4);
	//cout << "C: " << columnBin << " R: " << rowBin<<'\n';
	int column = stoi(columnBin, 0, 2);
	int row = stoi(rowBin, 0, 2);
	//cout << "C: " << column << " R: " << row;
	int element = S[boxNumber][column][row];
	//cout << "\nElement: " << element;
	string ans = decToBinary(element);
	return ans;
}

/*
The function f takes as input a first argument A, which is a bitstring of length
32, and a second argument J that is a bitstring of length 48, and produces as output
a bitstring of length 32. The following steps are executed:
1.The first argument A is “expanded” to a bitstring of length 48 according to
a fixed expansionfunction E. E(A) consists of the 32 bits from A, permuted
in a certain way, with 16 of the bits appearing twice.
2.Compute E(A) XOR J and write the result as the concatenation of eight 6-bit
strings B = B1 B2 B3 B4 B5 B6 B7 B8.
3. Each Bn will return an element from the S-box
4. The S-box elements concatenated will be permuted according to fixed permutation P
*/
string f(string A, int roundNumber) {
	string expandedA = "";
	for (int i = 0; i < 48; ++i) {
		expandedA += A[E[i] - 1];
	}
	//cout <<"ExpandedA: "<<expandedA<<'\n';
	string roundKey = round_keys[roundNumber - 1];
	//cout << "RoundKey:  " << roundKey << '\n';
	string B = Xor(expandedA, roundKey);
	//cout << "XOR(A,RoundKey): " << B;
	vector<string> BComponents = splitB(B);
	string C = "";
	for (int i = 0; i < BComponents.size(); ++i) {
		C += SElement(BComponents[i], i);
	}
	//cout << "\nC: " << C<<"\n";
	string ans = "";
	for (int i = 0; i < 32; ++i) {
		ans += C[P[i] - 1];
	}
	return ans;
}

/* In each DES_round:
Left_i=Right_(i-1)
Right=XOR(L_(i-1),f(R_(i-1),RoundKey)
*/
pair<string, string> DES_round(string L, string R, int roundNumber) {
	pair<string, string> aux;
	aux.first = R;
	aux.second = Xor(L, f(R, roundNumber));
	//cout <<"Left: "<< aux.first << "\nRight: " << aux.second << '\n';
	return aux;
}

/* After 16 DES round:
Right+Left - concatenated (!Notice Right+Left NOT Left+Right!)
The resulting string is permutated according to the inverse of the initial permutation*/
string encrypt(string R, string L) {
	string aux = R + L;
	string ans = "";
	for (int i = 0; i < aux.length(); ++i) {
		ans += aux[IP_inverse[i] - 1];
	}
	//cout << "\n\n\nFINAL: " << bin2hex(ans) << "\n\n\n";
	return bin2hex(ans);
}

/*Expanding the generated 8 bits key to a 64 bits one (each character is copied 8 times)*/
string expandKey(string key) {
	string ans = "";
	for (int i = 0; i < key.length(); ++i) {
		for (int j = 0; j < 7; ++j) {
			ans += key[i];
		}
		ans += (key[i] == '0') ? '1' : '0';
	}

	return ans;
}

/* Normal DES encryption
Given a text and a key compute the round keys and apply 16 DES rounds
Encrypt the R and L sides of the result*/
string DES_Encrypt(string text, string key) {
	string permutation = computeIP(hex2bin(text));
	pair<string, string> ans = split(permutation, 32);
	string L = ans.first;
	string R = ans.second;
	previous_key = initialKeyPermutation(hex2bin(key));

	for (int i = 1; i <= 16; ++i) {
		computeRoundKey(i);
	}

	for (int i = 1; i <= 16; ++i) {
		pair<string, string> aux;
		aux = DES_round(L, R, i);
		L = aux.first;
		R = aux.second;
	}
	return encrypt(R, L);
}

/*  DES decryption
Given a text and a key compute the round keys AND REVERSE THEM.
Apply 16 DES rounds
Encrypt the R and L sides of the result*/
string DES_Decrypt(string text, string key) {
	string permutation = computeIP(hex2bin(text));
	pair<string, string> ans = split(permutation, 32);
	string L = ans.first;
	string R = ans.second;
	previous_key = initialKeyPermutation(hex2bin(key));

	for (int i = 1; i <= 16; ++i) {
		computeRoundKey(i);
	}
	reverse(round_keys.begin(), round_keys.end());
	for (int i = 1; i <= 16; ++i) {
		pair<string, string> aux;
		aux = DES_round(L, R, i);
		L = aux.first;
		R = aux.second;
	}
	return encrypt(R, L);
}

/*Transform an integer into a 8bits key [0-255]*/
string keyGenerator(int n) {
	string binaryNum="00000000";
	int i = 0;
	while (n > 0) {
		if (n % 2 == 0) {
			binaryNum[7-i] = '0';
		}else{
			binaryNum[7-i] = '1';
		}
		n /= 2;
		++i;
	}
	return binaryNum;
}
 
/* Auxiliary function used for sorting the array*/
bool custom_compare(const pair<string, string>& a, const pair<string, string>& b) {
	if (a.second.compare(b.second) < 0) {
		return true;
	}
	return false;
}

/* Auxiliary comparation structure used for STL binary search*/
struct compare {
	bool operator()(const pair<string, string>& a, const string& key) {
		return (a.second.compare(key) < 0);
	}
	bool operator()(const string& key, const pair<string, string>& a) {
		return (key.compare(a.second) < 0);
	}
};

/* Meet in the middle attack
Apply DES two times with two different keys. For all 256 keys that can be generated:
1.Encrypt the plaintext (with all keys) and store pairs of (key,text_encrypted_with_key)
2.Decrypt the cryptedtext (with all keys)and store pairs of (key,text_decrypted_with_key)
3.Sort the lists of encrypted&decrypted texts 
4.Iterate through all the encrypted texts and check if one of them exists in the decrypted texts (Using Binary Search reducing the search time!!!)
5.WHEN Encrypt(plaintext,key1)=Decrypt(cryptedtext,key2) key1 and key2 where the keys used for encryption
*/
void meetInTheMiddle(string text, string key1, string key2) {
	cout << "\nExpanded key1: " << expandKey(key1);
	cout << "\nExpanded key2: " << expandKey(key2);
	string pt1 = DES_Encrypt(text,bin2hex(expandKey(key1)));
	vector<string> key_list_1 = key_list;
	key_list.clear();
	round_keys.clear();

	string pt2 = DES_Encrypt(pt1, bin2hex(expandKey(key2)));
	cout << "\ndes(X)=" << pt1 << "\ndes(des(X))=" << pt2 << '\n';
	vector<string> key_list_2 = key_list;

	vector<pair<string, string>> encrypted;
	vector<pair<string, string>> decrypted;
	/* Step 1 */
	for (int i = 0; i < 256; ++i) {
		string key = expandKey(keyGenerator(i));
		key_list.clear();
		round_keys.clear();
		encrypted.push_back(make_pair(key,DES_Encrypt(text, bin2hex(key))));
	}

	/* Step 2 */
	for (int i = 0; i < 256; ++i) {
		string key = expandKey(keyGenerator(i));
		key_list.clear();
		round_keys.clear();
		decrypted.push_back(make_pair(key, DES_Decrypt(pt2, bin2hex(key))));
	}
	
	/* Step 3 */
	sort(encrypted.begin(),encrypted.end(), custom_compare);
	sort(decrypted.begin(), decrypted.end(), custom_compare);

	/* Step 4+5 (BS+matching) */
	for (int i = 0; i < 256; ++i) {
		if (binary_search(decrypted.begin(), decrypted.end(), encrypted[i].second, compare())) {
			cout << "Key for encryption: " << encrypted[i].first << "\n";
			for (int j = 0; j < 256; ++j) {
				if (decrypted[j].second.compare(encrypted[i].second)==0) {
					cout << "Key used for decryption: " << decrypted[j].first << "\n";
				}
			}
		}
	}


}

/* Generate a random binary key of 8 bits that will be expanded */
string randomKeyInitializer() {
	string aux = "";
	for (int i = 0; i < 8; ++i) {
		bool bit = rand() % 2;
		if (bit) {
			aux += "1";
		}
		else {
			aux += "0";
		}
	}
	return aux;
}

int main() {
	string hex = "0123456789ABCDEF"; //hex

	srand(time(0));
	string binaryKey1 = randomKeyInitializer();
	string binaryKey2 = randomKeyInitializer();
	cout << "K1: " << binaryKey1 << " K2: " << binaryKey2<<'\n';
	meetInTheMiddle(hex, binaryKey1, binaryKey2);
}