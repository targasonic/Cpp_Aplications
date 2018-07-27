#pragma once

#include "exfat.h"
#include <iostream> 
#include <fstream>

using namespace std;

class WriteFileToImage
{
public:
	void DataRegion(char file[], int fileSize, int firstClusterOfFile);
	// ���������� ����� � ���� ������ �����

	void writeFileFatTable(int fileSize, int firstClusterOfFile);
	// ���������� ���������� � ����� � ��� �������

	void writeFileBitMap(int fileSize, int firstClusterOfFile);
	// ���������� ���������� � ����� � ��� ��� �������

	void writeFileEntry(char fileName[], unsigned int fileNumber, unsigned int fileLength, unsigned int fileFirstCluster);
	// ���������� ���������� � ����� � ���� ����������
};
