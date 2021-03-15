/*
######################################################
Introduction to Criptography - 2021 - Homework1
Last modification (1-Mar-2021)

Author: Gaina Robert-Adrian
Contact: gainarobertadrian@gmail.com

Cryptanalysis of Classical Cryptosystems (S(26),V(26))
Cracking V(26) using the index of coincidence and mutual index of coincidence tests
######################################################

Resources:
1. https://drive.google.com/file/d/1xFP5J6zAxpYgmNpTAbs9C7K8NOwRJRlI/view
2. https://drive.google.com/file/d/1aJb0Iks5Rteo8aUGDuJBiSLxHfM6Hwe9/view
3. https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher
4. https://www.geeksforgeeks.org/vigenere-cipher/
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

vector<char> encripted_vignere;
vector<int> encripted_vignere_values;
vector<double> normal_frecq_letters = {0.08167,0.01492,0.02782,0.04253,0.12702,0.02228,0.02015,0.06094,0.06966,0.00153,0.00772,0.04025,0.02406,0.06749,0.07507,0.01929,0.00095,0.05987,0.06327,0.09056,0.02758,0.00978,0.02360,0.00150,0.01974,0.00074};
vector<vector<int>> partitions;

int keylenpractice;

double abs_diff(double a, double b) {
	if (a > b)
		return a - b;
	return b - a;
}

void check_letter_frec() {
	char letter = 'a';
	for (int i = 0; i < normal_frecq_letters.size(); ++i)
		cout << (char)(letter + i) << " frec " << normal_frecq_letters[i]<<'\n';
}

void transform_file() {
	char ch;
	fstream fin("original_file.txt");
	fstream fout("plain_file.txt");
	while (fin >> ch) {
		if (ch >= 'A' && ch <= 'Z')
		{
			fout << char(ch);
			cout << char(ch);
		}
		else if (ch >= 'a' && ch <= 'z')
		{
			fout << char(ch - 32);
			cout << char(ch - 32);
		}
	}
}

void vignere_cript(vector<int>& key_values)
{
	fstream fin("plain_file.txt");
	fstream fout("vignere_cripted.txt");
	int it = 0;
	char ch;
	while (fin >> ch) {
		char cripted = (char)((ch - 65 + key_values[it++ % key_values.size()]) % 26 + 65);
		fout << cripted;
		encripted_vignere.push_back(cripted);
		encripted_vignere_values.push_back(cripted - 65);
	}
}

void push_cripted()
{
	fstream fin("vignere_cripted.txt");
	int it = 0;
	char ch;
	while (fin >> ch) {
		encripted_vignere.push_back(ch);
		encripted_vignere_values.push_back(ch - 65);
	}
}

double compute_coincidence(vector<int>& partition) {
	vector<int> letter_fr(26);
	for (int i = 0; i < partition.size(); ++i)
		++letter_fr[partition[i]];
	double IC = 0;
	for (int i = 0; i < letter_fr.size(); ++i)
		IC += (double)((letter_fr[i] * (letter_fr[i] - 1))) / (partition.size() * (partition.size() - 1));
	return IC;
}

int key_size() {
	int m = 1;
	for (int i = m; i < encripted_vignere.size(); ++i)
	{
		vector<vector<int>> substrings(i);
		for (int j = 0; j < encripted_vignere_values.size(); ++j)
		{
			substrings[j % i].push_back(encripted_vignere_values[j]);
		}
		vector<double> coincidence_index(i);
		for (int j = 0; j <substrings.size(); ++j)
		{
			coincidence_index[j] = compute_coincidence(substrings[j]);
		}
		bool possible_length = true;
		for (int j = 0; j < coincidence_index.size(); ++j)
		{
			if (coincidence_index[j] < 0.055 || coincidence_index[j] >(double) 0.075)
			{
				possible_length = false;
				break;
			}
		}
		if (possible_length == true) {
			cout << "\nPossible key size: " << i<<'\n';
			partitions = substrings;
			return i;
		}

	}
	return -1;
}

bool cmp(pair<int,double>& a,
	pair<int, double>& b){
	return a.second < b.second;
}

int key_size_by_mean() {
	int m = 1;
	int best_bad_length = -1;
	double best_bad_mean = 1;
	double best_bad_abs = 1;
	vector<pair<int,double>> diff_mean;
	for (int i = m; i < encripted_vignere.size(); ++i)
	{
		vector<vector<int>> substrings(i);
		for (int j = 0; j < encripted_vignere_values.size(); ++j)
		{
			substrings[j % i].push_back(encripted_vignere_values[j]);
		}
		vector<double> coincidence_index(i);
		double coincidence_total_difference = 0;
		for (int j = 0; j < substrings.size(); ++j)
		{
			coincidence_index[j] = compute_coincidence(substrings[j]);
			coincidence_total_difference += abs_diff(coincidence_index[j],0.065);
		}
		coincidence_total_difference /= i;
		diff_mean.push_back(make_pair(i,coincidence_total_difference));
	}
	sort(diff_mean.begin(), diff_mean.end(),cmp);
	vector<vector<int>> substrings(diff_mean[0].first);
	for (int j = 0; j < encripted_vignere_values.size(); ++j)
	{
		substrings[j % diff_mean[0].first].push_back(encripted_vignere_values[j]);
	}
	partitions = substrings;
	return diff_mean[0].first;
}

void compute_partition(int length)
{
	vector<vector<int>> substrings(length);
	for (int j = 0; j < encripted_vignere_values.size(); ++j)
	{
		substrings[j % length].push_back(encripted_vignere_values[j]);
	}
	partitions = substrings;
}

vector<char> key_finder(int key_length) {
	vector<char> key(key_length);

	for (int i = 0; i < key_length; ++i)
	{
		int s = 0;
		vector<double> letter_fr(26);
		for (int j = 0; j < partitions[i].size(); ++j) {
			++letter_fr[partitions[i][j]];
		} /// Frequence for i'th component of the original text

		vector<double> mic_values(26);
		double sum = 0;
		while (s != 26) /// Calculate MIC for ALL possible shifts of the i'th component
		{
			for (int k = 0; k < 26; ++k) {
				mic_values[s] += normal_frecq_letters[k] * (double)(letter_fr[(k+s)%26]) / partitions[i].size();
			}
			++s;
		}

		int best_shift = 0;
		double best_shift_value = 1;
		for (int j = 0; j < mic_values.size(); ++j) {
			if (abs_diff(0.65, mic_values[j]) < best_shift_value) {
				best_shift_value = abs_diff(0.65, mic_values[j]);
				best_shift = j;
			}
		}
		key[i] = best_shift;
	}
	return key;
}

int main() {

	transform_file();

	cout << "\nText converted!\nGive the key for encoding:";

	vector<char> key;
	vector<int> key_values;
	char value[256];

	cin >> value;
	keylenpractice = strlen(value);
	for (int i = 0; i < strlen(value); ++i)
	{
		key.push_back(toupper(value[i]));
		key_values.push_back(key[i] - 65);
		cout << key_values[i] << ' ';
	}

	vignere_cript(key_values);

	cout << "\nText encripted!\n";
	for (int i = 0; i < encripted_vignere.size(); ++i)
	{
		cout << encripted_vignere[i];
	}

	cout << "\nCOMPUTING KEY SIZE\n";
	int key_length = -1;//key_size();
	
	//check_letter_frec();

	vector<char> key_found;

	if (key_length != -1)
	{
		cout << "Key length:" << key_length << "\n\n";
		key_found=key_finder(key_length);
	}
	else
	{
		cout << "The key length can't be found with the fast approach. Trying to find the key with lowest variation:\n";
		key_length=key_size_by_mean();
		cout << "Key length by lowest variation:" << key_length << "\n\n";
		key_found=key_finder(key_length);
	}

	cout << "Initial key:\n";
	for (int i = 0; i < key_found.size(); ++i)
	{
		cout <<(char) (key_found[i] + 65);
	}
	cout << '\n';

	bool repetitive = 1;
	for (int i = key_length+key_length; i < encripted_vignere.size(); i += key_length)
	{
		if (repetitive != 0) {
			compute_partition(i);
			vector<char> found_key=key_finder(i);
			for (int j = 0; j < found_key.size() - key_length; ++j)
			{
				if (found_key[j] != found_key[j + key_length])
				{
					cout << "Next key possible:\n";
					for (int j = 0; j < found_key.size(); ++j)
					{
						cout << (char)(found_key[j] + 65);
					}
					cout << '\n';

					repetitive = 0;
					break;
				}
			}
		}
	}
}