#include "stdafx.h"
#include "exfat.h"
#include "writefile.h"
#include <iostream> 
#include <fstream>

using namespace std;


void WriteFileToImage::DataRegion(char file[], int fileSize, int firstClusterOfFile)
{
	int offsetFileStart = (clusterHeapOffset + sectorsPerCluster * (firstClusterOfFile - 2)) * bytePerSector;
	for (int i = 0; i < fileSize; i++) {
		volumeExFat[offsetFileStart + i] = file[i];
	}
}

void WriteFileToImage::writeFileFatTable(int fileSize, int firstClusterOfFile)
{
	int numberOfClusterForFile 	= (fileSize / (sectorsPerCluster * bytePerSector)) + 1;		// ������� ����� ���������, ������� �������� ����
	int offsetFileStart 		= fatOffset * bytePerSector + firstClusterOfFile * 4 + 4;				// ������� �������� FAT TABLE  � ����� ��������� �����

	for (int i = 0; i < numberOfClusterForFile - 1; i++) {

		unsigned int numberNextCluster = firstClusterOfFile + i + 1;
		volumeExFat[offsetFileStart + 4 * i] 		= numberNextCluster % 256;
		numberNextCluster 					 		= numberNextCluster / 256;
		volumeExFat[offsetFileStart + 4 * i + 1] 	= numberNextCluster % 256;
		numberNextCluster 							= numberNextCluster / 256;
		volumeExFat[offsetFileStart + 4 * i + 2] 	= numberNextCluster % 256;
		numberNextCluster 							= numberNextCluster / 256;
		volumeExFat[offsetFileStart + 4 * i + 3]	= numberNextCluster;
	}
	unsigned int numberNextCluster = 0xFFFFFFFF;
	int offsetLastCluster 							= offsetFileStart + 4 * (numberOfClusterForFile - 1);
	volumeExFat[offsetLastCluster] 					= numberNextCluster % 256;
	numberNextCluster 								= numberNextCluster / 256;
	volumeExFat[offsetLastCluster + 1] 				= numberNextCluster % 256;
	numberNextCluster 								= numberNextCluster / 256;
	volumeExFat[offsetLastCluster + 2] 				= numberNextCluster % 256;
	numberNextCluster 								= numberNextCluster / 256;
	volumeExFat[offsetLastCluster + 3] 				= numberNextCluster;

}

void WriteFileToImage::writeFileBitMap(int fileSize, int firstClusterOfFile);
{
	int offsetBitMap 		= (clusterHeapOffset + sectorsPerCluster * (0)) * bytePerSector;					// ������� �������� Bit map  � ����� ��������� �����
	int lastClusterOfFile 	= firstClusterOfFile + ((fileSize / (sectorsPerCluster * bytePerSector)) + 1);		// C������ ����� ���������� �������� �����

	int indexFirst 			= (firstClusterOfFile - 2) / 8;			// ������� ������ ���� � bitmap table
	int indexLast 			= (lastClusterOfFile - 2) / 8;			// ������� ��������� ���� � bitmap table

															// ���������� ������ �������
	for (int i = indexFirst; i <= indexLast; i++) {
		if (indexFirst == indexLast)	volumeExFat[offsetBitMap + i] = volumeExFat[offsetBitMap + i] | bitMapMaskTable[((firstClusterOfFile - 2) % 8)][((lastClusterOfFile - 2) % 8)];
		else {
			if (i == indexFirst)		volumeExFat[offsetBitMap + i] = volumeExFat[offsetBitMap + i] | bitMapMaskTable[((firstClusterOfFile - 2) % 8)][7];
			else if (i == indexLast)	volumeExFat[offsetBitMap + i] = volumeExFat[offsetBitMap + i] | bitMapMaskTable[0][((lastClusterOfFile - 2) % 8)];
			else 						volumeExFat[offsetBitMap + i] = 0xFF;
		}
	}
}


void WriteFileToImage::writeFileEntry(char fileName[], unsigned int fileNumber, unsigned int fileLength, unsigned int fileFirstCluster);
{

}