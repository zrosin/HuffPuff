#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> // sort
#include <utility> // move
#include <array> // used in sort

struct TreeNode {
	int glyph = -1;
	int frequency = 0;
	int leftptr = -1;
	int rightptr = -1;
	int parent = -1;

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
int huffStart = 513;
int huffEnd;
TreeNode huffmanTable[513];

int freqStart = 0;
std::array<TreeNode, 257> frequencyTable;

std::vector<unsigned char> fileContent;
const int bufferSize = 16;
int bufferRemainder = 0;

void checkFin(std::ifstream& fin) {
	//Check file opened
	if (!fin.is_open()) {
		std::cout << "Error reading " << fileName << " please check filename." << std::endl;
		exit(0);
	}
}

void initFreqTable() {
	for (int i = 0; i < 257; ++i)
	{
		frequencyTable[i].glyph = i;
	}
}

void findFreqStart() {
	for (int i = 0; i < 267; ++i) {
		if (frequencyTable[i].frequency != 0) {
			freqStart = i;
			return;
		}
	}
}

void readFile(std::ifstream& fin) {
	fin.seekg(0, std::ios::end);
	int size = fin.tellg();
	fin.seekg(0);
	bufferRemainder = size % bufferSize;
	
	unsigned char buffer[bufferSize];
	for (int i = 0; i < (size / bufferSize); ++i) {
		fin.read((char*)buffer, bufferSize);

		for (int j = 0; j < bufferSize; ++j) {
			frequencyTable[(int)buffer[j]].frequency += 1;
			fileContent.push_back(buffer[j]);
		}
	}


	if (bufferRemainder != 0) {
		fin.read((char*)buffer, bufferSize);

		for (int i = 0; i < bufferRemainder; i++) {
			frequencyTable[(int)buffer[i]].frequency += 1;
			fileContent.push_back(buffer[i]);
		}
	}
}

void makeHuffmanTree() {
	//create huffman table
	for (int i = freqStart; i < 257; ++i) {
		std::sort(frequencyTable.begin() + i, frequencyTable.end());
		if (i == 256) {
			huffStart -= 1;
			huffmanTable[huffStart] = std::move(frequencyTable[i]);
			break;
		}
		else {
			TreeNode mergeNode;
			mergeNode.frequency = frequencyTable[i].frequency + frequencyTable[i + 1].frequency;
			mergeNode.leftptr = huffStart - 1;
			mergeNode.rightptr = huffStart - 2;
			huffmanTable[huffStart - 1] = std::move(frequencyTable[i]);
			huffmanTable[huffStart - 2] = std::move(frequencyTable[i + 1]);
			huffStart -= 2;
			frequencyTable[i + 1] = std::move(mergeNode);
		}
	}

	huffEnd = 513 - huffStart;
	//fix huffman to be left to right
	if (huffStart != 0) {
		for (int i = 0; i < huffEnd; ++i) {
			huffmanTable[i] = std::move(huffmanTable[i + huffStart]);
			if (huffmanTable[i].glyph == -1) {
				huffmanTable[i].leftptr -= huffStart;
				huffmanTable[i].rightptr -= huffStart;
			}
		}
	}
}

int main() {
	std::ifstream fin;
	std::cin >> fileName;

	fin.open(fileName, std::ios::binary);
	
	checkFin(fin);

	initFreqTable();

	readFile(fin);
	frequencyTable[256].frequency += 1;

	std::sort(frequencyTable.begin(), frequencyTable.end());
	findFreqStart();
	
	makeHuffmanTree();

	//encode 



}