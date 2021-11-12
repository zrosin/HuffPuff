#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

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

TreeNode huffmanTable[513];
int huffEnd;
std::vector<std::vector<bool>> codes(257);

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


int main() {
	std::ifstream fin;
	std::ofstream fout;

	std::string fileNameInput;
	char huffFileName [100];
	int fileNameSize = 0;

	std::cin >> fileNameInput;

	fin.open(fileNameInput, std::ios::binary);

	fin.read((char*)&fileNameSize, sizeof fileNameSize);

	std::cout << fileNameSize << std::endl;

	fin.read((char*)&huffFileName, fileNameSize);
	huffFileName[fileNameSize] = '\0';

	fout.open(huffFileName, std::ios::binary);

	std::cout << huffFileName << std::endl;

	fin.read((char*)&huffEnd, sizeof(int));

	std::cout << huffEnd << std::endl;

	int temp[3];

	for (int i = 0; i < huffEnd; i++) {
		fin.read((char*)temp, sizeof(int) * 3);
		huffmanTable[i].glyph = temp[0];
		huffmanTable[i].leftptr = temp[1];
		huffmanTable[i].rightptr = temp[2];
	}

	//i think this was wrong. need to think about it.
	//huffEnd++;

	int myCurrentNode = 0;
	char outputBuffer[1024];
	int outputIter = 0;
	bool tempBit;
	bool eof = true;
	std::bitset<8> bits;
	char c;

	do {
		fin.read((char*)&c, sizeof c);
		bits = c;

		for (int i = 0; i < 8 && eof; i++) {
			if (bits[i] == 0) {
				myCurrentNode = huffmanTable[myCurrentNode].leftptr;
			}
			else {
				myCurrentNode = huffmanTable[myCurrentNode].rightptr;
			}
			
			if (huffmanTable[myCurrentNode].glyph != -1) {
				if (huffmanTable[myCurrentNode].glyph == 256) {
					eof = false;
				}
				else {
					outputBuffer[outputIter] = huffmanTable[myCurrentNode].glyph;
					outputIter++;
					myCurrentNode = 0;
				}
			}
		}

		if (outputIter > 1014) {
			fout.write((char*)&outputBuffer, outputIter);
			outputIter = 0;
		}
	} while (eof);

	if (outputIter > 0) {
		fout.write((char*)&outputBuffer, outputIter);
	}
}
