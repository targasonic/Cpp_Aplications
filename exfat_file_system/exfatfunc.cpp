#include "stdafx.h"
#include "exfat.h"
#include "exfatfunc.h"
#include <iostream> 
#include <fstream>
#include "imageinit.h"

using namespace std;


void fullInitExfatImage(unsigned char * image) {

	setBootRegionBootSector(&(*image));						// Инициализация Boot Sector		
	setBootRegionExtendedBootSectors(&(*image));			// Инициализация Extended Boot Sector		
	setBootRegionOemParametrs(&(*image));					// Инициализация Boot Region Oem Parametrs		
	setBootRegionReserved(&(*image));						// Инициализация Boot Region Reserved		
	setBootRegionChecksum(&(*image));						// Инициализация Boot Region Checksum		

	setFatRegionFatAligment(&(*image));
	setFatRegionFirstFat(&(*image));

	setDataRegionClusterHeapAligment(&(*image));
	setDataRegionUpcaseTable(&(*image));
	setDataRegionVolumeLabel(&(*image));
	setDataRegionAllocationBitMapEntry(&(*image));
	setDataRegionUpCaseTableEntry(&(*image));
}

void printExFatParametrs() {
	cout << bytePerSector		<< "		Byte Per Sector Shift"	<< "\n";
	cout << sectorsPerCluster	<< "		Sectors Per Cluster"	<< "\n";
	cout << clusterCount		<< "		Clusters Per Volume"	<< "\n";
	cout << volumeSize			<< "		Bytes Per Volume"		<< "\n";
	cout << (volumeSize / 1024) << "		KBytes Per Volume"		<< "\n" << "\n";
}

void setBootRegionBootSector(unsigned char * image)
{
	unsigned char exFatBootSector[bytePerSector];
	// exFAT IMAGE / MAIN BOOT REGION / BOOT SECTOR		(Offset, sectors = 0. Size, sectors = 1)
	// JUMP BOOT.  0xEB7690; Смещение 0, Размер 3.
	exFatBootSector[0] = 0xEB;	exFatBootSector[1] = 0x76;	exFatBootSector[2] = 0x90;

	// File System Name. "EXFAT> Смещение 3, Размер 8.
	exFatBootSector[3] = 'E'; 	exFatBootSector[4] = 'X';	exFatBootSector[5] = 'F'; 	exFatBootSector[6] = 'A';
	exFatBootSector[7] = 'T';	exFatBootSector[8] = 0x20; 	exFatBootSector[9] = 0x20;	exFatBootSector[10] = 0x20;

	// Must Be Zero. Смещение 11, Размер 53.
	for (int i = 11; i < (11 + 53); i++) exFatBootSector[i] = 0;

	// Partition Offset. In sectors; if 0, shall be ignored; Смещение 64, Размер 8.
	exFatBootSector[64] = 0;	exFatBootSector[65] = 0;	exFatBootSector[66] = 0; exFatBootSector[67] = 0;
	exFatBootSector[68] = 0;	exFatBootSector[69] = 0;	exFatBootSector[70] = 0; exFatBootSector[71] = 0;

	// Volume Length. Size of exFAT volume in sectors. Смещение 72, Размер 8.
	int volumeLengthTemp = volumeLengthL;
	for (int i = 72; i < 72 + 4; i++) {
		exFatBootSector[i] = volumeLengthTemp % 256;
		volumeLengthTemp = volumeLengthTemp / 256;
	}
	volumeLengthTemp = volumeLengthH;
	for (int i = 76; i < 76 + 4; i++) {
		exFatBootSector[i] = volumeLengthTemp % 256;
		volumeLengthTemp = volumeLengthTemp / 256;
	}
	// FAT Offset. In sectors. Смещение 80, Размер 4.
	int fatOffsetTemp = fatOffset;
	for (int i = 80; i < 80 + 4; i++) {
		exFatBootSector[i] = fatOffsetTemp % 256;
		fatOffsetTemp = fatOffsetTemp / 256;
	}
	// FAT Lendth. In sectors. May exceed the required space in order to align the second FAT. Смещение 84, Размер 4.
	int fatLendthTemp = fatLendth;
	for (int i = 84; i < 84 + 4; i++) {
		exFatBootSector[i] = fatLendthTemp % 256;
		fatLendthTemp = fatLendthTemp / 256;
	}
	// Cluster Heap Offset. In sectors. Смещение 88, Размер 4.
	int clusterHeapOffsetTemp = clusterHeapOffset;
	for (int i = 88; i < 88 + 4; i++) {
		exFatBootSector[i] = clusterHeapOffsetTemp % 256;
		clusterHeapOffsetTemp = clusterHeapOffsetTemp / 256;
	}
	// Cluster Count. 2^32-11 is the maximum number of clusters could be described. Смещение 92, Размер 4.
	int clusterCountTemp = clusterCount;
	for (int i = 92; i < 92 + 4; i++) {
		exFatBootSector[i] = clusterCountTemp % 256;
		clusterCountTemp = clusterCountTemp / 256;
	}
	// Root Directory Cluster. Смещение 96, Размер 4.
	unsigned int rootDirectoryCluster = 0x5;
	for (int i = 96; i < 96 + 4; i++) {
		exFatBootSector[i] = rootDirectoryCluster % 256;
		rootDirectoryCluster = rootDirectoryCluster / 256;
	}
	// Volume Serial Number. Смещение 100, Размер 4.
	unsigned int volumeSerialNumber = 0x8634EA57;
	for (int i = 100; i < 100 + 4; i++) {
		exFatBootSector[i] = volumeSerialNumber % 256;
		volumeSerialNumber = volumeSerialNumber / 256;
	}
	// File System Revision. Смещение 104, Размер 2.
	unsigned int fileSystemRevision = 0x100;
	for (int i = 104; i < 104 + 2; i++) {
		exFatBootSector[i] = fileSystemRevision % 256;
		fileSystemRevision = fileSystemRevision / 256;
	}
	// Volume Flag. Смещение 106, Размер 2.
	unsigned int volumeFlag = 0;
	for (int i = 106; i < 106 + 2; i++) {
		exFatBootSector[i] = volumeFlag % 256;
		volumeFlag = volumeFlag / 256;
	}
	// Byte Per Sector Shift. Power of 2. Minimum 9 (512 bytes per sector), maximum 12 (4096 bytes per sector). Смещение 108, Размер 1.
	exFatBootSector[108] = bytePerSector2Power;
	// Sectors Per Cluster Shift. Power of 2. Minimum 0 (1 bytes per sector), maximum 25 - BytesPerSectorShift, so max cluster size is 32MB. Смещение 109, Размер 1.
	exFatBootSector[109] = sectorsPerCluster2Power;
	// Number of Fats. 2 is for TexFAT only. Смещение 110, Размер 1.
	exFatBootSector[110] = 1;
	// Drive Select. Extended INT 13h drive number; typically 0x80. Смещение 111, Размер 1.
	exFatBootSector[111] = 0x80;
	// Percent In Use. 0..100 - percentage of allocated clusters rounded down to the integer 0xFF - percentage is not availble. Смещение 112, Размер 1.
	exFatBootSector[112] = 0x00;
	// Резерв . Смещение 113, Размер 7.
	for (int i = 113; i < (113 + 7); i++) exFatBootSector[i] = 0;
	// Boot Code. Смещение 120, Размер 390.
	for (int i = 120; i < (120 + 390); i++) exFatBootSector[i] = exFatBootSectorBootCode[i - 120];
	// Boot Signature. 0xAA55 Смещение 510, Размер 2.
	exFatBootSector[510] = 0x55;	exFatBootSector[511] = 0xAA;
	// Excess Space. Not used. Смещение 512. Размер (2^bytePerSector - 512)
	for (int i = 512; i < (bytePerSector); i++) exFatBootSector[i] = 0;


	cout << "installation Boot Region \ Boot Sector \n";

	for (int i = 0; i < 1 * bytePerSector; i++)
	{
		*(image + i) = exFatBootSector[i];							// Инициализация Main Boot Region	
		*(image + 12 * bytePerSector + i) = exFatBootSector[i];		// Инициализация Backup Boot Region	

//		volumeExFat[i] = exFatBootSector[i];							// Инициализация Main Boot Region	
//		volumeExFat[12 * bytePerSector + i] = exFatBootSector[i];		// Инициализация Backup Boot Region	
	}

}

void setBootRegionExtendedBootSectors(unsigned char * image)
{
	unsigned char exFatExtendedBootSectors[8 * bytePerSector];

	// exFAT IMAGE / MAIN BOOT REGION / Extended Boot Sectors	(Offset, sectors = 1. Size, sectors = 8)
	for (int i = 0; i < (8 * bytePerSector - 2); i++) exFatExtendedBootSectors[i] = 0x00;
	exFatExtendedBootSectors[1 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[1 * bytePerSector - 1] = 0xAA;
	exFatExtendedBootSectors[2 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[2 * bytePerSector - 1] = 0xAA;
	exFatExtendedBootSectors[3 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[3 * bytePerSector - 1] = 0xAA;
	exFatExtendedBootSectors[4 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[4 * bytePerSector - 1] = 0xAA;
	exFatExtendedBootSectors[5 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[5 * bytePerSector - 1] = 0xAA;
	exFatExtendedBootSectors[6 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[6 * bytePerSector - 1] = 0xAA;
	exFatExtendedBootSectors[7 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[7 * bytePerSector - 1] = 0xAA;
	exFatExtendedBootSectors[8 * bytePerSector - 2] = 0x55;
	exFatExtendedBootSectors[8 * bytePerSector - 1] = 0xAA;

	for (int i = 0; i < 8 * bytePerSector; i++)
	{
		*(image + bytePerSector + i) = exFatExtendedBootSectors[i];							// Инициализация Main Boot Region	
		*(image + 12 * bytePerSector + bytePerSector + i) = exFatExtendedBootSectors[i];		// Инициализация Backup Boot Region	
	}


	cout << "installation Boot Region \ Extended BootSectors \n";
}

void setBootRegionOemParametrs(unsigned char * image)
{
	unsigned char exFatOemParametrs[bytePerSector];

	// exFAT IMAGE / MAIN BOOT REGION / OEM Parametrs			(Offset, sectors = 9. Size, sectors = 1)
	for (int i = 0; i < (bytePerSector); i++) exFatOemParametrs[i] = 0;

	for (int i = 0; i < 1 * bytePerSector; i++)
	{
		*(image + 9 * bytePerSector + i) = exFatOemParametrs[i];							// Инициализация Main Boot Region	
		*(image + 12 * bytePerSector + 9 * bytePerSector + i) = exFatOemParametrs[i];		// Инициализация Backup Boot Region	
	}

	cout << "installation Boot Region \ Oem Parametrs \n";

}

void setBootRegionReserved(unsigned char * image)
{
	unsigned char exFatReserved[bytePerSector];

	// exFAT IMAGE / MAIN BOOT REGION / Reserved				(Offset, sectors = 10. Size, sectors = 1)
	for (int i = 0; i < (bytePerSector); i++) exFatReserved[i] = 0;

	for (int i = 0; i < 1 * bytePerSector; i++)
	{
		*(image + 10 * bytePerSector + i) = exFatReserved[i];							// Инициализация Main Boot Region	
		*(image + 12 * bytePerSector + 10 * bytePerSector + i) = exFatReserved[i];		// Инициализация Backup Boot Region	
	}

	cout << "installation Boot Region \ Reserved \n";
}

void setBootRegionChecksum(unsigned char * image)
{
	unsigned char exFatBootChecksum[bytePerSector];
	// exFAT IMAGE / MAIN BOOT REGION / BOOT CHECKSUM			(Offset, sectors = 11. Size, sectors = 1)
	unsigned int BootChecksum;

	for (int i = 0; i < 11 * bytePerSector; i++) {
		if (i == 106 || i == 107 || i == 122) continue;
		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + * (image + i);
	}
	for (int i = 0; i < (bytePerSector / 4); i++) {
		unsigned int chsum = BootChecksum;
		exFatBootChecksum[4 * i + 3] = chsum % 256;
		chsum = chsum / 256;
		exFatBootChecksum[4 * i + 2] = chsum % 256;
		chsum = chsum / 256;
		exFatBootChecksum[4 * i + 1] = chsum % 256;
		chsum = chsum / 256;
		exFatBootChecksum[4 * i] = chsum % 256;
		chsum = chsum / 256;
	}

	for (int i = 0; i < 1 * bytePerSector; i++)
	{
		*(image + 11 * bytePerSector + i) = exFatBootChecksum[i];						// Инициализация Main Boot Region	
		*(image + 12 * bytePerSector + 11 * bytePerSector + i) = exFatBootChecksum[i];		// Инициализация Backup Boot Region	
	}

	cout << "installation Boot Region Checksum \n\n";
}

void setFatRegionFatAligment(unsigned char * image) {
	// exFAT IMAGE / FAT REGION	/ FAT ALIGMENT		(Offset, sectors = 0. Size, sectors = fatOffset - 24)
	for (int i = 0; i < (fatOffset - 24)*bytePerSector; i++) *(image + 24 * bytePerSector + i) = 0;
	cout << "installation FAT Region FAT Aligment \n";
}

void setFatRegionFirstFat(unsigned char * image) {
	// exFAT IMAGE / FAT REGION	/ First FAT			(Offset, sectors = fatOffset. Size, sectors = fatLength)
	// Формирование FAT таблицы
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/
	unsigned char FATFirstFat[fatLendth*bytePerSector];
	// FatEntry[0]:
	FATFirstFat[0] = 0xF8;  FATFirstFat[1] = 0xFF; 	FATFirstFat[2] = 0xFF;	FATFirstFat[3] = 0xFF;
	// FatEntry[1]:
	FATFirstFat[4] = 0xFF;	FATFirstFat[5] = 0xFF;	FATFirstFat[6] = 0xFF;	FATFirstFat[7] = 0xFF;
	// First Cluster - Last Cluster:
	for (unsigned int i = 0; i < 4 * clusterCount; i++)	FATFirstFat[8 + i] = 0;			// перебираем кластеры

	for (int i = 0; i < fatLendth*bytePerSector; i++) *(image + fatOffset*bytePerSector + i) = FATFirstFat[i];
	// uiExcessEntry
	for (int i = ((fatOffset + fatLendth - 1) * bytePerSector + 4 * clusterCount + 8); i < ((fatOffset + fatLendth * numberOfFats) * bytePerSector); i++)	*(image + i) = 0x00;
	cout << "installation FAT Region First FAT \n\n";


}

void setDataRegionClusterHeapAligment(unsigned char * image) {
	int sizeClusterHeapAligment = (clusterHeapOffset - (fatOffset + fatLendth * numberOfFats)) * bytePerSector;

	for (int i = 0; i < (sizeClusterHeapAligment); i++) *( image + (fatOffset + fatLendth * numberOfFats) * bytePerSector + i) = 0;

	cout << "installation Data Region \ Cluster Heap Aligment \n";
}

void setDataRegionVolumeLabel(unsigned char * image) {
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/
	// Volume Label Directory Entry:
	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector;
	unsigned char volumeLabel[32];
	volumeLabel[0] = 0x83;																		// Entry Type
	volumeLabel[1] = 10;																		// Character Count

	for (int i = 0; i < (10); i++) {
		volumeLabel[2 * i + 2] = exFatVolumeLabel[i];
		volumeLabel[2 * i + 1 + 2] = 0x00;
	}
	for (int i = (20 + 2); i < 32; i++) volumeLabel[i] = 0;						// Reserved
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/

	for (int i = 0; i < 32; i++) *(image + offsetEntry + i) = volumeLabel[i];

	cout << "installation Data Region Volume Label \n";
}

void setDataRegionUpcaseTable(unsigned char * image) {

	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 1) * bytePerSector;

	for (int i = 0; i < upcaseTableSize; i++) *(image + offsetEntry + i) = upcaseTable[i];

	cout << "installation Data Region UpCase Table \n";
}

void setDataRegionAllocationBitMapEntry(unsigned char * image) {

	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32;

	unsigned char bitmapAllocationTableEntry[32] =
	{ 0x81, 0x0, 0x0, 0x0,	0x0, 0x0, 0x0, 0x0,			0x0, 0x0, 0x0, 0x0,		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0,	0x02, 0x0, 0x0, 0x0,		0xDC, 0x05, 0x0, 0x0,	0x0, 0x0, 0x0, 0x0 };

	for (int i = 0; i < 32; i++) *(image + offsetEntry + i) = bitmapAllocationTableEntry[i];

	cout << "installation Data Region Allocation BitMap Table Entry \n";

}

void setDataRegionUpCaseTableEntry(unsigned char * image) {

	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32 + 32;

	unsigned char upCaseTableEntry[32] =
	{ 0x82, 0x0, 0x0, 0x0,	0x0D, 0xD3, 0x19, 0xE6,			0x0, 0x0, 0x0, 0x0,		0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0,	0x03, 0x0, 0x0, 0x0,			0xCC, 0x16, 0x0, 0x0,	0x0, 0x0, 0x0, 0x0 };

	for (int i = 0; i < 32; i++) *(image + offsetEntry + i) = upCaseTableEntry[i];

	cout << "installation Data Region Up Case Table Entry \n\n";

}





void writeFileEntry(unsigned char * image, char fileName[], unsigned int fileNumber, unsigned int fileLength, unsigned int fileFirstCluster) {

	// File Directory Entry
	// ------------------------------------------------------------------------------------------------------------------------------
	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32 * (3 * fileNumber);

	unsigned char fileEntry[32];
	fileEntry[0] = 0x85;			// Type
	fileEntry[1] = 0x02;			// Secondary Entries
	fileEntry[2] = 0x28;			// File Name Length

	unsigned int fileNameHash = 0x3CAD;
	fileEntry[3] = fileNameHash / 256; 	fileEntry[4] = fileNameHash % 256;

	fileEntry[5] = 0x00; 	fileEntry[6] = 0x00;

	unsigned int fileEntryCreate = 0x4462863B;
	fileEntry[8] = fileEntryCreate % 256;
	fileEntryCreate = fileEntryCreate / 256;
	fileEntry[9] = fileEntryCreate % 256;
	fileEntryCreate = fileEntryCreate / 256;
	fileEntry[10] = fileEntryCreate % 256;
	fileEntry[11] = fileEntryCreate / 256;

	unsigned int fileEntryModified = 0xF162BA3A;
	fileEntry[12] = fileEntryModified % 256;
	fileEntryModified = fileEntryModified / 256;
	fileEntry[13] = fileEntryModified % 256;
	fileEntryModified = fileEntryModified / 256;
	fileEntry[14] = fileEntryModified % 256;
	fileEntry[15] = fileEntryModified / 256;

	fileEntry[16] = fileEntry[8];
	fileEntry[17] = fileEntry[9];
	fileEntry[18] = fileEntry[10];
	fileEntry[19] = fileEntry[11];

	fileEntry[20] = 0xA8;			// Create 10ms
	fileEntry[21] = 0x00;			// Modified 10ms
	fileEntry[22] = 0xEC;			// TX Offset CMA EC = GMT - 5
	fileEntry[23] = 0xEC;			// TX Offset CMA EC = GMT - 5

	fileEntry[24] = 0xEC;			// TX Offset CMA EC = GMT - 5
	fileEntry[25] = 0x00;
	fileEntry[26] = 0x00;
	fileEntry[27] = 0x00;
	fileEntry[28] = 0x00;
	fileEntry[29] = 0x00;
	fileEntry[30] = 0x00;
	fileEntry[31] = 0x00;

	for (int i = 0; i < 32; i++) *(image + offsetEntry + i) = fileEntry[i];

	// Stream Extension Directory Entry
	// ------------------------------------------------------------------------------------------------------------------------------
	offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32 * (3 * fileNumber + 1);

	unsigned char fileStreamEntry[32];
	fileStreamEntry[0] = 0xC0;			// Type
	fileStreamEntry[1] = 0x03;			// Flags
	fileStreamEntry[2] = 0x00;			// Reserved 

	unsigned int fileEntryCheckSum = 0x92D4;
	fileStreamEntry[2] = fileEntryCheckSum / 256; 	fileStreamEntry[3] = fileEntryCheckSum % 256;

	fileStreamEntry[4] = 0x20; 	fileStreamEntry[5] = 0x00;	// Attributes (0x0020 = Archive)
	fileStreamEntry[6] = 0x00; 	fileStreamEntry[7] = 0x00;	// 

	unsigned int fileLengthL = fileLength;
	fileStreamEntry[8] = fileLengthL % 256;
	fileLengthL = fileLengthL / 256;
	fileStreamEntry[9] = fileLengthL % 256;
	fileLengthL = fileLengthL / 256;
	fileStreamEntry[10] = fileLengthL % 256;
	fileStreamEntry[11] = fileLengthL / 256;

	fileStreamEntry[12] = 0x00;
	fileStreamEntry[13] = 0x00;
	fileStreamEntry[14] = 0x00;
	fileStreamEntry[15] = 0x00;

	fileStreamEntry[16] = 0x00;	// Reserved
	fileStreamEntry[17] = 0x00;	// Reserved
	fileStreamEntry[18] = 0x00;	// Reserved
	fileStreamEntry[19] = 0x00;	// Reserved

	unsigned int fileCluster = fileFirstCluster;			// First Cluster of File
	fileStreamEntry[20] = fileCluster % 256;
	fileCluster = fileCluster / 256;
	fileStreamEntry[21] = fileCluster % 256;
	fileCluster = fileCluster / 256;
	fileStreamEntry[22] = fileCluster % 256;
	fileStreamEntry[23] = fileCluster / 256;

	fileStreamEntry[24] = fileStreamEntry[8];
	fileStreamEntry[25] = fileStreamEntry[9];
	fileStreamEntry[26] = fileStreamEntry[10];
	fileStreamEntry[27] = fileStreamEntry[11];
	fileStreamEntry[28] = fileStreamEntry[12];
	fileStreamEntry[29] = fileStreamEntry[13];
	fileStreamEntry[30] = fileStreamEntry[14];
	fileStreamEntry[31] = fileStreamEntry[15];

	for (int i = 0; i < 32; i++) *(image + offsetEntry + i) = fileStreamEntry[i];

	// File Name Extension Directory Entry
	// ------------------------------------------------------------------------------------------------------------------------------
	offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32 * (3 * fileNumber + 2);

	unsigned char fileNameEntry[32];
	fileNameEntry[0] = 0xC1;			// Type
	fileNameEntry[1] = 0x00;			// Reserved

	cout << "installation File Entries:	";

	int index = 0;
	while ((fileName[index] != '\0') && (index < 15))
	{
		fileNameEntry[2 * index + 2] = fileName[index];
		cout << fileName[index];
		fileNameEntry[2 * index + 3] = 0x00;
		index++;
	}
	for (int i = index; i < 15; i++) {
		fileNameEntry[2 * index + 2] = 0x00;
		fileNameEntry[2 * index + 3] = 0x00;
	}
	cout << "\n";

	for (int i = 0; i < 32; i++) *(image + offsetEntry + i) = fileNameEntry[i];

}

void writeFileDataRegion(unsigned char * image, char file[], int fileSize, int firstClusterOfFile) {
	int offsetFileStart = (clusterHeapOffset + sectorsPerCluster * (firstClusterOfFile - 2)) * bytePerSector;
	for (int i = 0; i < fileSize; i++) 	*(image + offsetFileStart + i) = file[i];
}

void writeFileBitMap(unsigned char * image, int fileSize, int firstClusterOfFile) {

	int offsetBitMap = (clusterHeapOffset + sectorsPerCluster * (0)) * bytePerSector;					// Считаем смещение Bit map  в общей структуре диска
	int lastClusterOfFile = firstClusterOfFile + ((fileSize / (sectorsPerCluster * bytePerSector)) + 1);		// Cчитаем номер последнего кластера файла

	int indexFirst = (firstClusterOfFile - 2) / 8;			// Считаем первый байт в bitmap table
	int indexLast = (lastClusterOfFile - 2) / 8;			// Считаем последний байт в bitmap table
	
	// Заполнение битмап таблицы
	for (int i = indexFirst; i <= indexLast; i++) {
		if (indexFirst == indexLast)	*(image + offsetBitMap + i)		= *(image + offsetBitMap + i) | bitMapMaskTable[((firstClusterOfFile - 2) % 8)][((lastClusterOfFile - 2) % 8)];
		else {
			if (i == indexFirst)		*(image + offsetBitMap + i)	= *(image + offsetBitMap + i) | bitMapMaskTable[((firstClusterOfFile - 2) % 8)][7];
			else if (i == indexLast)	*(image + offsetBitMap + i)	= *(image + offsetBitMap + i) | bitMapMaskTable[0][((lastClusterOfFile - 2) % 8)];
			else 						*(image + offsetBitMap + i)	= 0xFF;
		}
	}

}

void writeFileFatTable(unsigned char * image, int fileSize, int firstClusterOfFile) {

	int numberOfClusterForFile = (fileSize / (sectorsPerCluster * bytePerSector)) + 1;		// Считаем число кластеров, которые занимает файл
	int offsetFileStart = fatOffset*bytePerSector + firstClusterOfFile * 4 + 4;				// Считаем смещение FAT TABLE  в общей структуре диска

	for (int i = 0; i < numberOfClusterForFile - 1; i++) {

		unsigned int numberNextCluster = firstClusterOfFile + i + 1;
		*(image + offsetFileStart + 4 * i)		= numberNextCluster % 256;
		numberNextCluster = numberNextCluster / 256;
		*(image + offsetFileStart + 4 * i + 1)	= numberNextCluster % 256;
		numberNextCluster = numberNextCluster / 256;
		*(image + offsetFileStart + 4 * i + 2)	= numberNextCluster % 256;
		numberNextCluster = numberNextCluster / 256;
		*(image + offsetFileStart + 4 * i + 3)	= numberNextCluster;
	}
	unsigned int numberNextCluster = 0xFFFFFFFF;
	int offsetLastCluster = offsetFileStart + 4 * (numberOfClusterForFile - 1);
	*(image + offsetLastCluster) = numberNextCluster % 256;
	numberNextCluster = numberNextCluster / 256;
	*(image + offsetLastCluster + 1) = numberNextCluster % 256;
	numberNextCluster = numberNextCluster / 256;
	*(image + offsetLastCluster + 2) = numberNextCluster % 256;
	numberNextCluster = numberNextCluster / 256;
	*(image + offsetLastCluster + 3) = numberNextCluster;

}


void writeFileToImage(unsigned char * image, char file[], int fileSize, char fileName[], unsigned int fileNumber, int firstClusterOfFile) {	
	writeFileDataRegion	(&(*image), file, fileSize, firstClusterOfFile);
	writeFileFatTable	(&(*image), fileSize, firstClusterOfFile);
	writeFileEntry		(&(*image), fileName, fileNumber, fileSize, firstClusterOfFile);
	writeFileBitMap		(&(*image), fileSize, firstClusterOfFile);
}