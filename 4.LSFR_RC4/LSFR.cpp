/*
######################################################
Introduction to Criptography - 2021 - Homework5
Last modification (28-Jun-2021)

Author: Gaina Robert-Adrian
Contact: gainarobertadrian@gmail.com

1. LSFR Generator + comparison tests with bbs and jacobi
######################################################

Resources:
1. https://www.eng.auburn.edu/~strouce/class/elec6250/LFSRs.pdf
2. https://libntl.org/ (Big number library)
3. http://cacr.uwaterloo.ca/hac/about/chap6.pdf
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

/*
const int RUNS = 5;

class blum_blum_shub {
private:
	 ZZ p;
	 ZZ q;
	 ZZ m;
	 ZZ seed;
	 ZZ random;
	 int length;

	 ZZ primeCongruentTo3Mod4(int length) {
		 ZZ aux;
		 GenPrime(aux, length);
		 while (aux % 4 != 3) {
			 GenPrime(aux, length);
		 }
		 return aux;
	 }

	 ZZ seedGenerator(ZZ& p, ZZ& q) {
		 ZZ aux;
		 ZZ m = p * q;
		 aux = RandomBnd(m);
		 while (aux % p == 0 || aux % q == 0) {
			 aux = RandomBnd(m);
		 }
		 return aux;
	 }

public:
	blum_blum_shub(int& length) {
		this->length = length;
		p = primeCongruentTo3Mod4(length);
		q = primeCongruentTo3Mod4(length);
		m = p * q;
		seed = seedGenerator(p, q);
		random = seed;
	}

	ZZ random_bbs() {
		random = PowerMod(random, 2, m);
		return random;
	}
		
	void printAll() {
		cout << "------------------------------------------------------------------------------\n";
		cout << "P:" << p << '\n';
		cout << "Q:" << q << '\n';
		cout << "M:" << m << '\n';
		cout << "Seed:" << seed << '\n';
		cout << "Random:" << random << '\n';
		cout << "------------------------------------------------------------------------------\n";
		cout << "\n\n\n";
	}

};


//1.Modulus division by a power-of-2-number can be optimized.
//		 a % (power_of_2) == a & (power_of_2-1)
// 2. Division and multiplication by powers of two can also be optimized by shifting bits.
//		a / (power_of_2) == a >>= n where 2^n= power_of_2
 
ZZ jacobi_calculator(ZZ a, ZZ n) {
	if (n % 2 == 0) {
		cout << "Second parameter must be odd";
		return (ZZ)0;
	}
	ZZ b = a % n;
	ZZ c = n;
	ZZ s = (ZZ)1;
	while (b >= 2) {

		while ((b & 3) == 0) {						// b % 4    
			b >>= 2;								// b /= 4
		}
		if ((b & 1) == 0) {							// b % 2
			if ((c & 7) == 3 || (c & 7) == 5) {		// c % 8
				s = -s;
			}
			b >>= 1;								// b /= 2
		}
		if (b == 1) break;
		if ((b & 3) == 3 && (c & 3) == 3) {			// b % 4
			s = -s;
		}
		if (c % b == 0) {
			b = 0;
			break;
		}
		ZZ aux = b;
		b = c % b;
		c = aux;
	}
	return s * b;
}

class jacobi {
private:
	ZZ p;
	ZZ q;
	ZZ m;
	ZZ seed;
	ZZ random;
	vector<ZZ> m_components;
	int length;
	int l;
	vector<bool> generated_binary;
public:
	jacobi(int length, int l) {
		this->length = length;
		this->l = l;
		GenPrime(p, length);
		GenPrime(q, length);
		while (q % 2!=1) {
			GenPrime(q, length);
		}
		m = p * q;
	}

	jacobi(int NR_OF_PRIMES, int PRIME_LENGTH,bool second) {
		m = 1;
		for (int i = 0; i < NR_OF_PRIMES; ++i) {
			ZZ aux;
			GenPrime(aux, PRIME_LENGTH);
			m_components.push_back(aux);
			m *= aux;
		}
		l = PRIME_LENGTH * NR_OF_PRIMES;
	}

	vector<bool> generate_random(int LENGTH) {
		vector<ZZ> a_components;
		for (int i = 0; i < m_components.size(); ++i) {
			ZZ aux;
			aux = RandomBnd(m);
			a_components.push_back(aux);
		}
		ZZ seed = a_components[0];
		ZZ p = m_components[0];
		for (int i = 1; i < m_components.size(); ++i) {
			CRT(seed, p, a_components[i], m_components[i]);
		}
		seed = seed % m;
		if (seed < 0)
			seed += m;

		vector<bool> aux_vector;
		for (int i = 0; i < LENGTH; ++i) {
			ZZ aux = jacobi_calculator(seed + (ZZ)i, m);
			if (aux == -1)
				aux_vector.push_back(0);
			else if (aux == 1)
				aux_vector.push_back(1);
		}
		generated_binary = aux_vector;
		return generated_binary;
	}

	vector<bool> generate_random() {
		vector<bool> aux_vector;
		GenPrime(seed, RandomBnd(NumBits(m)));
		for (int i = 0; i < l; ++i) {
			ZZ aux = jacobi_calculator(seed + (ZZ)i, m);
			if (aux == -1)
				aux_vector.push_back(0);
			else if (aux == 1)
				aux_vector.push_back(1);
		}
		generated_binary = aux_vector;
		return generated_binary;
	}
};

vector<bool> convertToBinary(ZZ& number) {
	unsigned int number_size = NumBits(number);
	vector<bool> binary_representation(number_size);
	for (int i = 0; i < number_size; ++i) {
		binary_representation[number_size - (i + 1)] = bit(number, i);
	}
	return binary_representation;
}

void printBinary(const vector<bool>& v) {
	cout << "Binary format: ";
	for (int i = 0; i < v.size(); ++i)
		cout << v[i];
	cout << "\n\n";
}

void elementaryTest(const vector<bool>& v) {
	cout << "Elementary test:\n";
	int nr_of_0 = 0;
	for (int i = 0; i < v.size(); ++i) {
		if (v[i] == 0)
			++nr_of_0;
	}
	double percentage_of_0 = 100 * (double)nr_of_0 / v.size();
	cout << "Number of 0: " << nr_of_0<<'\n';
	cout << "Number of 1: " << v.size() - nr_of_0<<'\n';
	cout << "Percentages:\n0=" << percentage_of_0 << "%\n";
	cout << "1=" << 100 - percentage_of_0 << "%\n\n";
}

pair<string,string> printToFiles(const vector<bool>& v,const int& iteration,string generator) {
	string filename= generator + "_" + to_string(iteration)+".txt";
	string default_file = generator + "_Only1_" + to_string(iteration) + ".txt";
	ofstream fout(filename);
	ofstream f(default_file);
	for (int i = 0; i < v.size(); ++i)
	{
		fout << v[i];
		f << 1;
	}
	return make_pair(filename, default_file);
}

int getSize(ifstream& file) {
	file.seekg(0, ios::end);
	int size = file.tellg();
	file.seekg(0);
	return size;
}

void compressionTest(const vector<bool>& v,const int& i,string generator) {
	cout << "File compression test:\n";
	pair <string,string> files=printToFiles(v, i,generator);
	pair<string, string> zip_files = make_pair(files.first.substr(0, files.first.size() - 4)+".zip", files.second.substr(0, files.second.size() - 4)+".zip");
	system(("powershell Compress-Archive -Force " + files.first+ " " + zip_files.first).c_str());
	system(("powershell Compress-Archive -Force " + files.second + " " + zip_files.second).c_str());

	//Default file size equals the generated one
	ifstream gin(files.first, ios::binary);
	ifstream zip_gin(zip_files.first, ios::binary);
	ifstream zip_din(zip_files.second, ios::binary);

	int g_size=getSize(gin);
	int zip_g_size=getSize(zip_gin);
	int zip_d_size=getSize(zip_din);


	double g_comp_ratio = ((double)zip_g_size / (double)g_size) * 100;
	double s_comp_ratio= ((double)zip_d_size / (double)g_size) * 100;

	printf("Initial files size: %d\nArchived file zie: %d\nDefault archived size: %d\n", g_size, zip_g_size, zip_d_size);
	printf("Generated file compression ratio: %.2f%%\n",g_comp_ratio);
	printf("Default file compression ratio: %.2f%%\n", s_comp_ratio);
}

double bbs_test(int bit_length) {
	clock_t start, end;
	blum_blum_shub generator(bit_length);
	ZZ a;
	generator.printAll();
	double averageTime = 0;
	for (int i = 0; i < RUNS; ++i) {
		cout << "------------------------------------------------------------------------------\n";
		cout << "Iteration:" << i << "\t\t*bbs*\n";
		start = clock();
		a = generator.random_bbs();
		Sleep(10);
		end = clock();
		averageTime += (double)(end - start) - 10;
		cout << "Time taken:" << (double)(end - start)-10 << " Clock cicles.\n";
		cout << "Generated number=" << a << "\n\n";
		vector<bool> binary;
		binary = convertToBinary(a);
		printBinary(binary);
		elementaryTest(binary);
		compressionTest(binary,i,"bbs");
		cout << "------------------------------------------------------------------------------\n";
		cout << "\n\n\n";
	}
	return averageTime / RUNS;
}

double jacobi_test(int p_q_size,int nr_bit_length) {
	clock_t start, end;
	jacobi j(p_q_size, nr_bit_length);
	vector<bool> binary;
	double averageTime = 0;
	for (int i = 0; i < RUNS; ++i) {
		cout << "------------------------------------------------------------------------------\n";
		cout << "Iteration:" << i << "\t*jacobi*\n";
		start = clock();
		binary = j.generate_random();
		Sleep(10);
		end = clock();
		averageTime += (double)(end - start) - 10;
		cout << "Time taken:" << (double)(end - start)-10 << " Clock cicles.\n";
		printBinary(binary);
		elementaryTest(binary);
		compressionTest(binary, i,"jcb");
		cout << "------------------------------------------------------------------------------\n";
		cout << "\n\n\n";
	}
	return averageTime / RUNS;
}

double jacobi_test_b(int NR_OF_PRIMES, int PRIME_LENGTH,int RANDOM_LENGTH) {
	clock_t start, end;
	jacobi j(NR_OF_PRIMES,PRIME_LENGTH,1);
	vector<bool> binary;
	double averageTime = 0;
	for (int i = 0; i < RUNS; ++i) {
		cout << "------------------------------------------------------------------------------\n";
		cout << "Iteration:" << i << "\t*jacobi multiple components*\n";
		start = clock();
		binary = j.generate_random(RANDOM_LENGTH);
		Sleep(10);
		end = clock();
		averageTime += (double)(end - start) - 10;
		cout << "Time taken:" << (double)(end - start)-10 << " Clock cicles.\n";
		printBinary(binary);
		elementaryTest(binary);
		compressionTest(binary, i, "jcb_comp");
		cout << "------------------------------------------------------------------------------\n";
		cout << "\n\n\n";
	}
	return averageTime / RUNS;
}

void jacobi_symbol_test(int bit_length,int runs) {
	blum_blum_shub generator(bit_length);
	ZZ a = generator.random_bbs();
	ZZ b = generator.random_bbs();
	while (b % 2 != 1) { b = generator.random_bbs(); }
	for (int i = 0; i < runs; ++i)
	{
		ZZ aux1, aux2;
		if ((aux1=jacobi_calculator(a, b)) != (aux2=Jacobi(a, b))) {
			cout << "Something is wrong";
			cout << aux1 << " " << aux2<<'\n';
			cout << "A:" << a << " B:" << b;
			return;
		}
		a = generator.random_bbs();
		b = generator.random_bbs();
		while (b % 2 != 1) { b = generator.random_bbs(); }
	}
	printf("%d symbols calculated correctly\n", runs);
}

unsigned long long int function(unsigned long long int n) {
	return ((n >> 34) & 1) ^ ((n >> 1) & 1);
}

unsigned long long int modifyBit(unsigned long long int n, unsigned long long int p, unsigned long long int b)
{
	unsigned long long int mask = 1 << p;
	return ((n & ~mask) | (b << p));
}

vector<bool> lsfr_generator(unsigned long long int seed, int nr_len) {

	vector<bool> binary;
	unsigned long long int initial_state = seed;
	vector<unsigned long long int> generated;
	unsigned long long int aux = initial_state;
	int i = 0;
	while (i != nr_len) {
		++i;
		binary.push_back(aux & 1);
		aux = modifyBit((aux >> 1), 34, function(aux));
		generated.push_back(aux);
		if (i % 100 == 0) {
			//cout << i << endl;
			sort(generated.begin(), generated.end());
			for (int j = 0; j < generated.size()-1; ++j) {
				if (generated[j] == generated[j + 1]) {
					cout<< "something is not quite right";
					for (int k = 0; k < generated.size(); ++k) {
						cout << generated[k] << '\n';
					}
					return binary;
				}
			}
		}
	}
	return binary;
}

unsigned long long int initialize() {
	unsigned long long int limit = pow(2, 29) + 1;
	return rand() % limit;
}

double lsfr_test(int bit_length) {
	clock_t start, end;
	double averageTime = 0;
	for (int i = 0; i < RUNS; ++i) {
		cout << "------------------------------------------------------------------------------\n";
		cout << "Iteration:" << i << "\t\t*lsfr*\n";
		start = clock();
		vector<bool> binary= lsfr_generator(initialize(), bit_length);
		Sleep(10);
		end = clock();
		averageTime += (double)(end - start) - 10;
		cout << "Time taken:" << (double)(end - start) - 10 << " Clock cicles.\n";

		printBinary(binary);
		elementaryTest(binary);
		compressionTest(binary, i, "lsfr");
		cout << "------------------------------------------------------------------------------\n";
		cout << "\n\n\n";
	}
	return averageTime / RUNS;
}

int main()
{
	double lsfrTime, bbsTime, jacobiTime, jacobiBTime;
	lsfrTime=lsfr_test(2048);
	bbsTime=bbs_test(1024);
	jacobiTime=jacobi_test(1024,2048);
	jacobiBTime=jacobi_test_b(5,512,2048);
	cout << "\n\n\n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	cout << "Time statistics:" << '\n';
	cout << "\tLSFR clocks/generation: " << lsfrTime << '\n';
	cout << "\tBBS clocks/generation: " << bbsTime << '\n';
	cout << "\tJACOBI clocks/generation: " << jacobiTime << '\n';
	cout << "\tJACOBI B clocks/generation: " << jacobiBTime << '\n';
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}*/
