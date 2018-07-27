#pragma once

#include <iostream> 
#include <fstream>

using namespace std;


void fullInitExfatImage(unsigned char * image);
void printExFatParametrs();

void setBootRegionBootSector(unsigned char * image);
void setBootRegionExtendedBootSectors(unsigned char * image);
void setBootRegionOemParametrs(unsigned char * image);
void setBootRegionReserved(unsigned char * image);
void setBootRegionChecksum(unsigned char * image);									// Инициализация Boot Region Checksum		

void setFatRegionFatAligment(unsigned char * image);
void setFatRegionFirstFat(unsigned char * image);

void setDataRegionClusterHeapAligment(unsigned char * image);
void setDataRegionUpcaseTable(unsigned char * image);
void setDataRegionVolumeLabel(unsigned char * image);
void setDataRegionAllocationBitMapEntry(unsigned char * image);
void setDataRegionUpCaseTableEntry(unsigned char * image);

void writeFileEntry(unsigned char * image, char fileName[], unsigned int fileNumber, unsigned int fileLength, unsigned int fileFirstCluster);
void writeFileDataRegion(unsigned char * image, char file[], int fileSize, int firstClusterOfFile);
void writeFileFatTable(unsigned char * image, int fileSize, int firstClusterOfFile);
void writeFileBitMap(unsigned char * image, int fileSize, int firstClusterOfFile);


void writeFileToImage(unsigned char * image, char file[], int fileSize, char fileName[], unsigned int fileNumber, int firstClusterOfFile);
