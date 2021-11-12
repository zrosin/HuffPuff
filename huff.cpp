#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> // sort
#include <utility> // move
#include <array> // used in sort
#include <bitset> // used for compressed output;

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

int huffStart = 512;
int huffEnd;
TreeNode huffmanTable[513];

int freqStart = 0;
std::array<TreeNode, 257> frequencyTable;

std::vector<unsigned char> fileContent;
const int bufferSize = 128;
int bufferRemainder = 0;

const int outputBufferSize = 32;
std::bitset<outputBufferSize> outputBuffer;

std::vector<std::vector<bool>> codes(257);

//This is neglible
void checkFin(std::ifstream& fin) {
	//Check file opened
	if (!fin.is_open()) {
		std::cout << "Error reading " << fileName << " please check filename." << std::endl;
		exit(0);
	}
}

//This is neglible
void initFreqTable() {
	for (int i = 0; i < 257; ++i)
	{
		frequencyTable[i].glyph = i;
	}
}

//This is negligible
void findFreqStart() {
	for (int i = 0; i < 267; ++i) {
		if (frequencyTable[i].frequency != 0) {
			freqStart = i;
			return;
		}
	}
}

//This is pretty fast
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

//This is really slow
void makeHuffmanTree() {
	//create huffman table
	for (int i = freqStart; i < 257; i++) {
		std::sort(frequencyTable.begin() + i, frequencyTable.end());

		if (i == 256) {
			huffmanTable[huffStart] = frequencyTable[i];
		}
		else {
			TreeNode merge;
			merge.frequency = frequencyTable[i].frequency + frequencyTable[i + 1].frequency;
			huffmanTable[huffStart] = frequencyTable[i];
			huffmanTable[huffStart - 1] = frequencyTable[i + 1];
			merge.leftptr = huffStart;
			merge.rightptr = huffStart - 1;
			huffStart -= 2;
			frequencyTable[i + 1] = merge;
		}
	}

	huffEnd = 513 - huffStart;
	for (int i = 0; i < huffEnd; ++i) {
		huffmanTable[i] = huffmanTable[i + huffStart];
		if (huffmanTable[i].glyph == -1) {
			huffmanTable[i].leftptr -= huffStart;
			huffmanTable[i].rightptr -= huffStart;
		}
	}
}

//This is about as fast as it gets, but still slow. O(n) for this vs O(n/2) for a optimal non lazy method
void mapHuffToCode() {
	for (int j = 0; j < 257; ++j) {
		for (int i = 0; i < 513; ++i) {
			if (j == huffmanTable[i].glyph) {
				std::vector<bool> temp;
				int previousNode = i;
				do {
					for (int k = 0; k < 513; ++k) {
						if (previousNode == huffmanTable[k].rightptr) {
							temp.push_back(true);
							previousNode = k;
							break;
						}
						else if (previousNode == huffmanTable[k].leftptr) {
							temp.push_back(false);
							previousNode = k;
							break;
						}
					}
					if (previousNode == 0) {
						std::reverse(temp.begin(), temp.end());
					}
						
				} while (previousNode != 0);

				codes[j] = temp;
				break;
			}
		}
	}
	
}


std::string hufFileName(std::string fileName) {
	std::string temp;
	int pos = fileName.find(".");
	if (pos != std::string::npos) {
		temp = fileName.substr(0, pos);
	}
	else {
		temp = fileName;
	}
	return temp;
}


int main() {
	std::ifstream fin;
	std::ofstream fout;
	std::cin >> fileName;

	std::string newFileName = hufFileName(fileName) + ".huf";

	fin.open(fileName, std::ios::binary);

	fout.open(newFileName, std::ios::out | std::ios::binary);

	checkFin(fin);

	initFreqTable();

	readFile(fin);
	frequencyTable[256].frequency += 1;

	std::sort(frequencyTable.begin(), frequencyTable.end());
	findFreqStart();

	makeHuffmanTree();

	mapHuffToCode();

	//output header data here
	//huffEnd shows the last used element in the huffman array
	//huffEnd + 1 is the number of elements in huffman array
	//fileName is file name

	
	int fileNameSize = fileName.size();

	fout.write((char*)&fileNameSize, sizeof fileNameSize);
	fout << fileName;
	fout.write((char*)&huffEnd, sizeof huffStart);

	int temp[3];

	for (int i = 0; i < huffEnd; i++) {
		temp[0] = huffmanTable[i].glyph;
		temp[1] = huffmanTable[i].leftptr;
		temp[2] = huffmanTable[i].rightptr;
		fout.write((char*)&temp, sizeof(int) * 3);
	}

	int fileIter = 0;
	int byteIter = 0;
	std::bitset<8> byte;
	std::vector<bool> leftovers;
	bool alreadyReversed = false;
	char c;
	while (fileIter < fileContent.size()) {
		if (leftovers.size()) {
			if (!alreadyReversed) {
				std::reverse(leftovers.begin(), leftovers.end());
			}
			
			for (int i = 0; i < 8; ++i) {
				byte[byteIter] = leftovers.back();
				leftovers.pop_back();
				++byteIter;
				if (leftovers.size() == 0) {
					alreadyReversed = false;
					break;
				}
				else {
					alreadyReversed = true;
				}
			}
		}
		while (byteIter < 8 && fileIter < fileContent.size()) {
			for (int i = 0; i < codes[fileContent[fileIter]].size(); ++i) {
				if (byteIter < 8) {
					byte[byteIter] = codes[fileContent[fileIter]][i];
					++byteIter;
				}
				else {
					leftovers.push_back(codes[fileContent[fileIter]][i]);
				}
			}
			++fileIter;
		}
		if (byteIter == 8) {
			c = (char)byte.to_ulong();
			fout.write((char*)&c, sizeof c);
			byteIter = 0;
			byte.reset();
		}
	}
	//three cases
	//leftovers in leftovers
	//leftovers in byte
	//no leftovers

	//no leftovers or leftovers in byte
	if (leftovers.size() == 0 && byteIter == 0 || leftovers.size() == 0 && byteIter > 0) {
		for (int i = 0; i < codes[256].size(); ++i) {
			if (byteIter < 8) {
				byte[byteIter] = codes[256][i];
				++byteIter;
			}
			else {
				leftovers.push_back(codes[256][i]);
			}
		}

		c = (char)byte.to_ulong();
		fout.write((char*)&c, sizeof c);
		byteIter = 0;
		byte.reset();

		if (leftovers.size() != 0) {
			std::reverse(leftovers.begin(), leftovers.end());
			while (leftovers.size()) {
				for (int i = 0; i < 8; ++i) {
					byte[byteIter] = leftovers.back();
					leftovers.pop_back();
					++byteIter;
					if (leftovers.size() == 0) {
						break;
					}
				}
				c = (char)byte.to_ulong();
				fout.write((char*)&c, sizeof c);
				byteIter = 0;
				byte.reset();
			}
		}
	}

	else { // leftovers.size() > 0 
		std:: vector<bool> tempvec;
		for (int i = 0; i < codes[256].size(); ++i) {
			tempvec.push_back(codes[256][i]);
		}
		if (!alreadyReversed) {
			std::reverse(leftovers.begin(), leftovers.end());
		}
		std::reverse(tempvec.begin(), tempvec.end());

		for (int i = 0; i < tempvec.size(); ++i) {
			leftovers.push_back(tempvec[i]);
		}

		while (leftovers.size()) {
			byte.reset();

			for (int j = 0; j < 8; ++j) {
				byte[byteIter] = leftovers.back();
				leftovers.pop_back();
				if (leftovers.size() == 0) {
					break;
				}
			}

			c = (char)byte.to_ulong();
			fout.write((char*)&c, sizeof c);
			byteIter = 0;
			byte.reset();
		}
	}
}