#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <map>
#include <string>
#include <ctime>
#include <Windows.h>
using namespace std;

class State {
public:
	int i;
	int j;
	unsigned char S[256];
	State(int i, int j, unsigned char S[]) {
		this->i = i;
		this->j = j;
		for (int i = 0; i < 256; ++i) {
			this->S[i] = S[i];
		}
	}
};

State RC4_permutation_initializer(vector<unsigned char> key) {
	int j = 0;
	unsigned char S0[256];
	for (int i = 0; i < 256; ++i) {
		S0[i] = i;
	}
	for (int i = 0; i < 256; ++i) {
		j = (j + S0[i] + key[i % key.size()]) % 256;
		swap(S0[i], S0[j]);
	}
	State a(0, 0, S0);
	return a;
}

vector<unsigned char> bits;
State RC4_transformation(State a) {
	(++a.i) % 256;
	a.j = (a.j + a.S[a.i]) % 256;
	swap(a.S[a.i], a.S[a.j]);
	bits.push_back((int)a.S[(a.S[a.i] + a.S[a.j]) % 256]);
	return a;
}

vector<unsigned char> convertToVector(string s, int l) {
	vector<unsigned char> ans;
	for (int i = 0; i < l; ++i) {
		ans.push_back(s[i]);
	}
	return ans;
}

string bitByBit(unsigned char a) {
	string ans = "";
	for (int i = 7; i >= 0; --i) {
		ans = ans + (((a >> i) & 1) == 1 ? "1" : "0");
	}
	return ans;
}

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
	string ans = "";
	for (int i = 0; i < text.size(); i += 4) {
		string character = "";
		for (int j = 0; j < 4; ++j) {
			character += text[i + j];
		}
		ans += m[character];
	}
	return ans;
}

int binaryToDecimal(int n) {
	int num = n;
	int dec_value = 0;

	int base = 1;

	int aux = num;
	while (aux) {
		int digit = aux % 10;
		aux /= 10;

		dec_value += digit * base;

		base *= 2;
	}
	return dec_value;
}

vector<unsigned char> random_string_initializer() {
	vector<unsigned char> ans;
	for (int i = 0; i < 8; ++i) {
		ans.push_back(rand() % 256);
	}
	return ans;
}

vector<unsigned char> random_string_initializer(int length) {
	vector<unsigned char> ans;
	for (int i = 0; i < length; ++i) {
		ans.push_back(rand() % 256);
	}
	return ans;
}

string plaintext_generator(int length) {
	string ans = "";
	vector<unsigned char> aux = random_string_initializer(length);
	for (int i = 0; i < length; ++i) {
		ans += aux[i];
	}
	return ans;
}

int main() {
//-------------------------------------------------RC4--------------------------------------------//
	string key = "ab69bl34";
	string plaintext = "This is a plaintext example";
	cout << "Plaintext: " << plaintext << '\n';
	string encrypted = "";
	vector<unsigned char> keyVector = convertToVector(key, 8);
	State bit = RC4_permutation_initializer(keyVector);
	for (int i = 0; i < plaintext.size(); ++i) {
		bit = RC4_transformation(bit);
		encrypted += bitByBit((unsigned char)(bits[i] ^ plaintext[i]));
	}
	cout << "Binary encrypted text: " << encrypted << '\n';
	string hexEnc = bin2hex(encrypted);
	cout << "Hexa encrypted text: " << hexEnc << '\n';

	vector<unsigned char> decEnc;
	for (int i = 0; i < encrypted.size(); i += 8) {
		decEnc.push_back((unsigned char)binaryToDecimal(stoi(encrypted.substr(i, 8))));
	}

	for (int i = 0; i < decEnc.size(); ++i) {
		cout << (int)decEnc[i] << ' ';
	}
	cout << '\n';

	string decryptedBin = "";

	for (int i = 0; i < decEnc.size(); ++i) {
		decryptedBin += bitByBit((unsigned char)(bits[i] ^ decEnc[i]));
	}
	cout << "Binary decrypted text: " << decryptedBin << '\n';
	decEnc.clear();
	for (int i = 0; i < decryptedBin.size(); i += 8) {
		decEnc.push_back((unsigned char)binaryToDecimal(stoi(decryptedBin.substr(i, 8))));
	}

	cout << "Decrypted text: ";
	for (int i = 0; i < decEnc.size(); ++i) {
		cout << decEnc[i];
	}
	cout << '\n';

	/*-------------------------------------------------RC4_TESTS---------------------------------------*/
	int RUNS = 1000000;
	srand(time(0));
	int counter = 0;
	for (int it = 0; it < RUNS; ++it) {
		bits.clear();
		string plaintext = "b";
		string encrypted = "";
		vector<unsigned char> keyVector;
		for (int i = 0; i < 8; ++i) {
			keyVector.push_back(rand() % 256);
		}
		State bit = RC4_permutation_initializer(keyVector);
		for (int i = 0; i < 2; ++i) {
			bit = RC4_transformation(bit);
			bitByBit((unsigned char)(bits[i] ^ plaintext[i]));
			if (i == 1) {
				if (bits[i] == 0) {
					++counter;
				}
			}
		}
	}
	cout << counter<<'\n';
	cout << (double)1 / 128 << " " << (double)counter / RUNS;

}
