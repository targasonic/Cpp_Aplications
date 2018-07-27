#include "stdafx.h"
#include "exfat.h"
#include "imageinit.h"
#include "exfatfunc.h"
#include <iostream> 
#include <fstream>

using namespace std;

unsigned char volumeExFat[volumeSize];
unsigned char volumeBitMap[volumeSize];

void createVhdHeadToImage();


int _tmain(int argc, _TCHAR* argv[])
{
	// ������������� ������
	ExFatImage MetkaMetka(&volumeExFat[0]);
	MetkaMetka.printExFatParametrs();
	MetkaMetka.fullInitExfatImage();

	// ��������� ������� �����
	const int	testFileSize			= 500000;
	char		testFile[testFileSize];
	char		testFileName[]			= "TestFile.txt";
	unsigned int fileFirstCluster		= 10;
	unsigned int testFileNumber			= 1;
	for (int i = 0; i < testFileSize; i++)	testFile[i] = i;

	// ������� ����� �� �����
	MetkaMetka.writeFileToImage(testFile, testFileSize, testFileName, testFileNumber, fileFirstCluster);


	// ��������� ������� �����
	const int	testFileSize2 = 300000;
	char		testFile2[testFileSize2];
	char		testFileName2[] = "TestFile.bin";
	fileFirstCluster	= 30;
	testFileNumber		= 2;
	for (int i = 0; i < testFileSize2; i++)		testFile2[i] = 0xAA;

	// ������� ����� �� �����
	MetkaMetka.writeFileToImage(testFile2, testFileSize2, testFileName2, testFileNumber, fileFirstCluster);


	// ������ �������� �� ����� �����
	const int	testFileSize3 = 21238; //21238;
	char		testFile3[testFileSize3];
	char		testFileName3[] = "testPic.bmp";
	fileFirstCluster	= 41;
	testFileNumber		= 3;

	ifstream inputFile("testPic.bmp");
	char next;
	for (int i = 0; i < testFileSize3; i++)
	{
		next = inputFile.get();
		testFile3[i] = next;
	}
	inputFile.close();

	// ������� ����� �� �����
	MetkaMetka.writeFileToImage(testFile3, testFileSize3, testFileName3, testFileNumber, fileFirstCluster);

	
	
	// ������ ������ ����� �  �������� ����
	ofstream fout("ExFat3.bin", std::ios::binary);
	for (int i = 0; i < volumeSize; i++)	fout << volumeExFat[i];
	fout.close();

	// �������� VHD ������ �����
//	createVhdHeadToImage();

	int a;
	cin >> a;
	return 0;
}






void createVhdHeadToImage() {

	ofstream imageOut("ExFatVhd.vhd", std::ios::binary);
	ifstream imageEntry("ExFatEntry", std::ios::binary);

	const int		entrySize = 65536;
	unsigned char	entryImage[entrySize];

	for (int i = 0; i < entrySize; i++)		entryImage[i] = imageEntry.get();
	for (int i = 0; i < entrySize; i++)		imageOut << entryImage[i];
	for (int i = 0; i < volumeSize; i++)	imageOut << volumeExFat[i];

	imageOut.close();
	imageEntry.close();
}
