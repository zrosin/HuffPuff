#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct TreeNode {
	int glyph;
	int leftptr = -1;
	int rightptr = -1;
};
struct Glyph {
	int glyph;
	int freq;
};

string fileName;

//Array is built right to left, so this is ptr at what should be the "0" element.
int huffStart = 512;
TreeNode huffmanTable[513];

int freqEnd = 256;
//inits every element to 0
int frequencyTable[257] = { 0 };;

ifstream fin;
const int bufferSize = 16;
int bufferRemainder = 0;

void checkFin(ifstream& fin) {
	//Check file opened
	if (!fin.is_open()) {
		cout << "Error reading " << fileName << " please check filename." << endl;
		exit(0);
	}
}

int main() {
	cin >> fileName;

	//Read entire file into a buffer variable;
	//While doing so add the frequencies to frequencyList;
	fin.open(fileName, ios::hex);
	
	//comment this out for final build.
	checkFin(fin);

	//get file size
	fin.seekg(0, ios::end);
	int size = fin.tellg();
	fin.seekg(0);
	bufferRemainder = size % bufferSize;

	vector<unsigned char [bufferSize]> fileContent((size / bufferSize) + (bool)bufferRemainder);

	unsigned char buffer[bufferSize];
	for (int i = 0; i < (size / bufferSize); i++) {
		fin.read((char*)buffer, bufferSize);

		for (int j = 0; j < bufferSize; j++) {
			frequencyTable[buffer[i]] += 1;
		}

		fileContent.push_back(buffer);
	}

	bufferRemainder = size % bufferSize;
	if (bufferRemainder != 0) {\
		fin.read((char*)buffer, bufferSize);

		for (int i = 0; i < bufferRemainder; i++) {
			frequencyTable[buffer[i]] += 1;
		}

		fileContent.push_back(buffer);
	}

	//condense freqtable and set freqend variable;

	//Quicksort

	//create huffman table

	//encode 

}