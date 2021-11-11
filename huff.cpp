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

int huffStart = 513;
int huffEnd;
TreeNode huffmanTable[513];

int freqStart = 0;
std::array<TreeNode, 257> frequencyTable;

std::vector<unsigned char> fileContent;
const int bufferSize = 128;
int bufferRemainder = 0;

const int outputBufferSize = 1024;
bool outputBuffer[outputBufferSize];

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

//This is about as fast as it gets, but still slow. O(n) for this vs O(n/2) for a optimal non lazy method
void mapHuffToCode() {
	for (int j = 0; j < 257; ++j) {
		for (int i = 0; i < 513; ++i) {
			if (j == huffmanTable[i].glyph) {
				std::vector<bool> temp;
				
				//this case is disgusting
				if (huffEnd == 0) {
					temp.push_back(true);
					codes[j] = temp;
					return;
				}
				//this is disgusting too
				else {
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
						//flip with this code if codes come out perfectly backwards. I'm too tired to thunk about this.
						/*
						if (previousNode == 0) {
							std::reverse(temp.begin(), temp.end());
						}
						*/
						
					} while (previousNode != 0);
				}

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
	
	fout.write((char*)fileName.size(), sizeof(int));
	fout << fileName;
	fout.write((char*)((int)huffEnd + 1), sizeof(int));

	int temp[3];

	for (int i = 0; i <= huffEnd; i++) {
		temp[0] = huffmanTable[i].glyph;
		temp[1] = huffmanTable[i].leftptr;
		temp[2] = huffmanTable[i].rightptr;
		fout.write((char*)temp, sizeof(int) * 3);
	}

	//encode and output
	int inputIter = 0;
	int outputIter = 0;
	std::vector<bool> leftOvers;
	int outputOverEight;
	int leftOversSize = 0;

	for (int i = 0; i < fileContent.size() / (outputBufferSize - 9); ++i) {
		while (fileContent.size() > (outputBufferSize - 9)) {
			for (int j = 0; j < codes[fileContent[inputIter]].size(); ++j) {
				outputBuffer[outputIter] = codes[fileContent[inputIter]][j];
				outputIter++;
			}
			++inputIter;
		}
		outputIter = 0;

		//output outputBuffer until outputIter
		if (leftOversSize == 0) {
			outputOverEight = (outputIter / sizeof(char*));
			fout.write((char*)outputBuffer, outputOverEight);

			leftOversSize = outputIter % sizeof(char*);
			leftOvers.resize(leftOversSize);

			if (leftOversSize != 0) {
				for (int i = 0; i < leftOversSize; i++) {
					leftOvers[i] = outputBuffer[(outputOverEight * 8) + i];
				}
			}
		}
		else {
			outputOverEight = (outputIter / sizeof(char*));
			fout.write((char*)(leftOvers, outputBuffer), outputOverEight);

			leftOversSize = outputIter % sizeof(char*);
			leftOvers.resize(leftOversSize);

			if (leftOversSize != 0) {
				for (int i = 0; i < leftOversSize; i++) {
					leftOvers[i] = outputBuffer[(outputOverEight * 8) + i];
				}
			}
		}
	}

	int remainder = fileContent.size() - inputIter;

	if (remainder != 0) {
		for (int i = 0; i < remainder; ++i) {
			for (int j = 0; j < codes[fileContent[inputIter]].size(); ++j) {
				outputBuffer[outputIter] = codes[fileContent[inputIter]][j];
				outputIter++;
			}
			++inputIter;
		}
		//output outputBuffer until outputIter
		if (leftOversSize == 0) {
			outputOverEight = (outputIter / sizeof(char*));
			fout.write((char*)outputBuffer, outputOverEight);

			leftOversSize = outputIter % sizeof(char*);
			leftOvers.resize(leftOversSize);

			if (leftOversSize != 0) {
				for (int i = 0; i < leftOversSize; i++) {
					leftOvers[i] = outputBuffer[(outputOverEight * 8) + i];
				}
			}
		}
		else {
			outputOverEight = (outputIter / sizeof(char*));
			fout.write((char*)(leftOvers, outputBuffer), outputOverEight);

			leftOversSize = outputIter % sizeof(char*);
			leftOvers.resize(leftOversSize);

			if (leftOversSize != 0) {
				for (int i = 0; i < leftOversSize; i++) {
					leftOvers[i] = outputBuffer[(outputOverEight * 8) + i];
				}
			}
		}
	}

	//dont forget to output codes[256], and to fill any remaining bits with 0
	bool eofOutput[16];
	int eofIter = 0;

	if (leftOversSize == 0) {
		for (int i = 0; i < 16; i++) {
			if (i > codes[256].size()) {
				eofOutput[i] = false;
			}
			else {
				eofOutput[i] = codes[256][i];
				eofIter++;
			}
		}
	}
	else {
		for (int i = 0; i < 16; i++) {
			if (i < leftOversSize) {
				eofOutput[i] = leftOvers[i];
				eofIter++;
			}
			else if ((i - leftOversSize) < codes[256].size()) {
				eofOutput[i] = codes[256][i - leftOversSize];
				eofIter++;
			}
			else {
				eofOutput[i] = false;
			}
		}
	}
	fout.write((char*)eofOutput, (eofIter / 8) + 1);
}