#pragma once

#include <iostream> 
#include <fstream>

using namespace std;


class ExFatImage
{
public:
	ExFatImage(unsigned char * volume);
	// инициализирует обьект

	void fullInitExfatImage();
	void printExFatParametrs();

	void setBootRegionBootSector();
	void setBootRegionExtendedBootSectors();
	void setBootRegionOemParametrs();
	void setBootRegionReserved();
	void setBootRegionChecksum();

	void setFatRegionFatAligment();
	void setFatRegionFirstFat();

	void setDataRegionClusterHeapAligment();
	void setDataRegionUpcaseTable();
	void setDataRegionVolumeLabel();
	void setDataRegionAllocationBitMapEntry();
	void setDataRegionUpCaseTableEntry();


	void writeFileEntry(char fileName[], unsigned int fileNumber, unsigned int fileLength, unsigned int fileFirstCluster);
	void writeFileDataRegion(char file[], int fileSize, int firstClusterOfFile);
	void writeFileFatTable(int fileSize, int firstClusterOfFile);
	void writeFileBitMap(int fileSize, int firstClusterOfFile);


	void writeFileToImage(char file[], int fileSize, char fileName[], unsigned int fileNumber, int firstClusterOfFile);

	
private:
	unsigned char *image; // массив образа диска

};