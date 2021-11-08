#include <iostream>
#include <fstream>
#include <vector>

struct TreeNode {
	int glyph = -1;
	int frequency = 0;
	int leftptr = -1;
	int rightptr = -1;

	bool operator<(TreeNode& other) {
		return (this->frequency < other.frequency);
	}
	bool operator>(TreeNode& other) {
		return (this->frequency > other.frequency);
	}
	bool operator==(TreeNode& other) {
		return (this->frequency == other.frequency);
	}
};

std::string fileName;

//Array is built right to left, so this is ptr at what should be the "0" element.
int huffStart = 512;
TreeNode huffmanTable[513];

int freqEnd = 256;
TreeNode frequencyTable[257] = { 0 };

std::ifstream fin;
const int bufferSize = 16;
int bufferRemainder = 0;

void checkFin(std::ifstream& fin) {
	//Check file opened
	if (!fin.is_open()) {
		cout << "Error reading " << fileName << " please check filename." << endl;
		exit(0);
	}
}

int main() {
	std::cin >> fileName;

	//Read entire file into a buffer variable;
	//While doing so add the frequencies to frequencyList;
	fin.open(fileName, std::ios::hex);
	
	//comment this out for final build.
	checkFin(fin);

	//get file size
	fin.seekg(0, std::ios::end);
	int size = fin.tellg();
	fin.seekg(0);
	bufferRemainder = size % bufferSize;

	std::vector<unsigned char [bufferSize]> fileContent((size / bufferSize) + (bool)bufferRemainder);

	for (int i = 0; i < fileContent.size(); ++i)
	{
		frequencyTable[i].glyph = i;
	}

	unsigned char buffer[bufferSize];
	for (int i = 0; i < (size / bufferSize); ++i) {
		fin.read((char*)buffer, bufferSize);

		for (int j = 0; j < bufferSize; ++j) {
			frequencyTable[buffer[i]].frequency += 1;
		}

		fileContent.push_back(buffer);
	}

	bufferRemainder = size % bufferSize;
	if (bufferRemainder != 0) {\
		fin.read((char*)buffer, bufferSize);

		for (int i = 0; i < bufferRemainder; i++) {
			frequencyTable[buffer[i]].frequency += 1;
		}

		fileContent.push_back(buffer);
	}

	//condense freqtable and set freqend variable;
	for (int i = 0; i < 257 && frequencyTable[i].frequency != 0; ++i) {

	}

	//Quicksort

	//create huffman table

	//encode 



}