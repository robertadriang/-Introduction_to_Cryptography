/*
######################################################
Introduction to Criptography - 2021 - Homework5
Last modification (12-May-2021)

Author: Gaina Robert-Adrian
Contact: gainarobertadrian@gmail.com

RSA system:
1. RSA encryption+decryption
2. RSA CRT decryption
3. Wiener attack
######################################################

Resources:
1. https://profs.info.uaic.ro/~tr/tr03-02.pdf
2. https://profs.info.uaic.ro/~siftene/Wiener.pdf
3. https://libntl.org/ (Big number library)
4. http://cacr.uwaterloo.ca/hac/about/chap8.pdf
*/
#include <NTL/ZZ.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <windows.h>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;
using namespace NTL;

ZZ encrypted_number;

class RSA {
private:
	ZZ p, q, phi, d;

	string numberToText(ZZ number) {
		ZZ copy = number;
		string ans = "";
		while (copy > 0) {
			char chr = 0;
			int aux = 1;
			for (int i = 0; i < 8; ++i) {
				chr += (copy % 2) * aux;
				copy /= 2;
				aux *= 2;
			}
			ans.push_back(chr);
		}
		reverse(ans.begin(), ans.end());
		return ans;
	}

	ZZ textToNumber(string text) {
		ZZ number(0);
		/*Transform the string into a number*/
		for (int i = 0; i < text.length(); ++i) {
			for (int j = 7; j >= 0; --j) {
				if ((text[i] >> j) & 1) {
					number *= 2;
					++number;
				}
				else {
					number *= 2;
				}
			}
		}
		return number;
	}

	vector<ZZ> continuedFractionExpansion(ZZ n, ZZ e) {
		vector<ZZ> ans;
		ZZ r;
		while (n != 0) {
			ans.push_back(e / n);
			r = e % n;
			e = n;
			n = r;
		}
		return ans;
	}

	vector<pair<ZZ, ZZ>> getConvergents(vector <ZZ> fractionExpansion) {
		vector<pair<ZZ, ZZ>> ans;
		ZZ alpha = fractionExpansion[0];
		ZZ beta(1);
		
		ans.push_back(make_pair(alpha, beta));

		alpha = fractionExpansion[0] * fractionExpansion[1] + 1;
		beta = fractionExpansion[1];
		ans.push_back(make_pair(alpha, beta));

		for (int i = 2; i < fractionExpansion.size(); ++i) {
			alpha = fractionExpansion[i] * alpha + ans[i - 2].first;
			beta = fractionExpansion[i] * beta + ans[i - 2].second;
			ans.push_back(make_pair(alpha, beta));
		}
		return ans;
	}

public:
	ZZ e, n;

	ZZ getP() {
		return p;
	}

	ZZ getQ() {
		return q;
	}

	ZZ getD() {
		return d;
	}

	pair<ZZ,ZZ> generateKeys(int length) {
		GenPrime(p, length);
		GenPrime(q, length);
		
		n = p * q;
		phi = (p - 1) * (q - 1);

		e = 2;
		while (GCD(e, phi) != 1) {
			//GenPrime(e, length - length / 2);
			GenPrime(e, 32);
		}

		InvMod(d, e, phi);

		cout << "p:" << p << "\n\n";
		cout << "q:" << q << "\n\n";
		cout << "n:" << n << "\n\n";
		cout << "phi:" << phi << "\n\n";
		cout << "e:" << e << "\n\n";
		cout << "d:" << d << "\n\n";

		return make_pair(e, n);
	}

	string encrypt(string text,pair<ZZ,ZZ> public_key) {
		ZZ number = textToNumber(text);

		cout <<"[encrypt]Converted number:"<< number << "\n";
		cout << "[encrypt]PublicKey<e,n>: <" << public_key.first << "," << public_key.second << ">\n";

		ZZ result = PowerMod(number, public_key.first, public_key.second);
		encrypted_number = result;

		cout <<"[encrypt]Encrypted number: "<< result<< "\n";
		string cypertext = numberToText(result);
		cout << "[encrypt]Encrypted text: " << cypertext << "\n\n";

		return cypertext;
	}

	string decrypt(string encrypted) {
		auto start = high_resolution_clock::now();
		ZZ decrypted = PowerMod(textToNumber(encrypted), d, n);
		auto stop = high_resolution_clock::now();
		string plaintext = numberToText(decrypted);
		cout << "[decrypt]Decrypted number: " << decrypted << "\n";
		cout << "[decrypt]Decrypted text: " << plaintext << "\n";
		auto duration = duration_cast<microseconds>(stop - start);

		cout << "[decrypt]Time taken by standard decryption: "
			<< duration.count() << " microseconds" << "\n\n";
		return plaintext;
	}

	string decryptCRT(ZZ n, ZZ p, ZZ q, ZZ d, ZZ number) {
		if (p > q) {
			swap(p, q);
		}
		cout << "[decryptCRT]Number to be decrypted:" << number << '\n';
		auto start = high_resolution_clock::now();
		ZZ n1 = d % (p - 1);
		ZZ n2 = d % (q - 1);
		ZZ inv = InvMod(p, q);

		ZZ x1 = PowerMod(number % p, n1, p);
		ZZ x2 = PowerMod(number % q, n2, q);
		ZZ mul = (x2 - x1) * (inv % q) % q;
		ZZ x = x1 + p * mul;
		auto stop = high_resolution_clock::now();
		string ans = numberToText(x);
		auto duration = duration_cast<microseconds>(stop - start);

		cout << "[decryptCRT]Decrypted number:" << x << '\n';
		cout << "[decryptCRT]Decrypted text:" << ans << "\n";
		cout << "[decryptCRT]Time taken by CRT decryption: "
			<< duration.count() << " microseconds" << "\n\n";
		return ans;
	}

	pair<ZZ, ZZ> generateWienerKeys() {
		ZZ p, q;
		while (true) {
			GenPrime(p, 512);
			GenPrime(q, 512);
			if (p < q) {
				swap(p, q);
			}
			if (q < p && p < 2 * q) {
				break;
			}
			cout << "I" << '\n';
		}
		ZZ n = p * q;
		ZZ phi = (p - 1) * (q - 1);

		ZZ d;
		while (true) {
			GenPrime(d, 32);
			if (d < SqrRoot(SqrRoot(n)) / 3)
				break;
		}
		ZZ e;
		InvMod(e, d, phi);

		cout << "P:" << p << "\n\n";
		cout << "Q:" << q << "\n\n";
		cout << "N:" << n << "\n\n";
		cout << "Phi:" << phi << "\n\n";
		cout << "E:" << e << "\n\n";
		cout << "D:" << d << "\n\n";

		return make_pair(e, n);
		
	}

	void wienerAttack(ZZ n, ZZ e) {
		vector<ZZ> fractionExpansion = continuedFractionExpansion(n, e);
		vector<pair<ZZ, ZZ>> convergents = getConvergents(fractionExpansion);

		ZZ d;

		for (auto fraction : convergents) {
			if (fraction.first == 0) { // If alpha is 0 alpha*beta can't be n
				continue;
			}

			if ((e * fraction.second - 1) % fraction.first != 0) { //e*d-1=l*phi(n) === (e*d)-1 mod l = 0 
				continue;
			}
			ZZ phi = (e * fraction.second - 1) / fraction.first; //(e*d-1)/l=phi(n)
			/* Equation of rank 2 is: y^2 - (n+1-phi(n)*y - n = 0 */
			ZZ a(1);
			ZZ b = (n + 1 - phi) * -1;
			ZZ c = n;

			ZZ delta = b * b - 4 * a * c;
			if (delta <= 0) { 
				continue;
			}

			ZZ sqrtDelta;
			SqrRoot(sqrtDelta, delta);
			if (sqrtDelta * sqrtDelta != delta) {
				continue;
			}
			b *= -1;
			ZZ x1 = (b + sqrtDelta) / (2 * a);
			ZZ x2 = (b - sqrtDelta) / (2 * a);
			if (x1 * x2 == n) {
				cout <<"D="<< fraction.second<< " found\n";;
				InvMod(d, e, phi);
				cout << "Real D=" << d << "\n";
				break;
			}
		}
	}
};

int main() {
	RSA instance = RSA();
	cout << "~~~~~~~~~~~~~~~~~~~~~~Key generation start~~~~~~~~~~~~~~~~~~~~~\n";
	pair<ZZ,ZZ> key=instance.generateKeys(512);
	cout << "~~~~~~~~~~~~~~~~~~~~~~Key generation stop~~~~~~~~~~~~~~~~~~~~~\n\n\n";

	string text = "plaintext_example";
	cout << "~~~~~~~~~~~~~~~~~~~~~~Encrypt start~~~~~~~~~~~~~~~~~~~~~\n";
	string cypertext=instance.encrypt(text,key);
	cout << "~~~~~~~~~~~~~~~~~~~~~~Encrypt stop~~~~~~~~~~~~~~~~~~~~~\n\n\n";

	cout << "~~~~~~~~~~~~~~~~~~~~~~Decrypt start~~~~~~~~~~~~~~~~~~~~~\n";
	string plaintext = instance.decrypt(cypertext);
	string plaintext2 = instance.decryptCRT(instance.n, instance.getP(), instance.getQ(), instance.getD(), encrypted_number);
	cout << "~~~~~~~~~~~~~~~~~~~~~~Decrypt stop~~~~~~~~~~~~~~~~~~~~~\n\n\n";
	cout << "C: " << cypertext << "\nP: " << plaintext <<"\nP2: "<<plaintext2<< "\n\n";

	cout << "~~~~~~~~~~~~~~~~~~~~~~Key generation Wiener start~~~~~~~~~~~~~~~~~~~~~\n";
	key=instance.generateWienerKeys();
	cout << "~~~~~~~~~~~~~~~~~~~~~~Key generation Wiener stop~~~~~~~~~~~~~~~~~~~~~\n\n\n";

	cout << "~~~~~~~~~~~~~~~~~~~~~~Wiener attack~~~~~~~~~~~~~~~~~~~~~\n";
	instance.wienerAttack(key.second, key.first);
	cout << "~~~~~~~~~~~~~~~~~~~~~~Wiener attack~~~~~~~~~~~~~~~~~~~~~\n";
	return 0;
}