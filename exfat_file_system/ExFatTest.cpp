#include "stdafx.h"
#include "exfat.h"
#include <iostream> 
#include <fstream>

using namespace std;

unsigned char volumeExFat	[volumeSize];
unsigned char volumeBitMap	[volumeSize];

void printExFatParametrs ();

void setBootRegionBootSector ();
void setBootRegionExtendedBootSectors ();
void setBootRegionOemParametrs ();
void setBootRegionReserved ();
void setBootRegionChecksum ();

void setFatRegionFatAligment ();
void setFatRegionFirstFat ();

void setDataRegionClusterHeapAligment ();
void setDataRegionUpcaseTable();
void setDataRegionVolumeLabel();
void setDataRegionAllocationBitMapEntry();
void setDataRegionUpCaseTableEntry();

void writeFileDataRegion	(char file[], int fileSize, int firstClusterOfFile);
void writeFileFatTable		(int fileSize, int firstClusterOfFile);
void writeFileBitMap		(int fileSize, int firstClusterOfFile);
void writeFileEntry			(char fileName[], unsigned int fileNumber , unsigned int fileLength, unsigned int fileFirstCluster);

void createVhdHeadToImage();


int _tmain(int argc, _TCHAR* argv[])
{	
	printExFatParametrs();
	setBootRegionBootSector();										// Èíèöèàëèçàöèÿ Boot Sector		
	setBootRegionExtendedBootSectors();								// Èíèöèàëèçàöèÿ Extended Boot Sector		
	setBootRegionOemParametrs();									// Èíèöèàëèçàöèÿ Boot Region Oem Parametrs		
	setBootRegionReserved();										// Èíèöèàëèçàöèÿ Boot Region Reserved		
	setBootRegionChecksum();										// Èíèöèàëèçàöèÿ Boot Region Checksum		

	setFatRegionFatAligment();
	setFatRegionFirstFat();

	setDataRegionClusterHeapAligment();
	setDataRegionUpcaseTable();
	setDataRegionVolumeLabel();
	setDataRegionAllocationBitMapEntry();
	setDataRegionUpCaseTableEntry();

	

	// Ãåíåðàöèÿ òåñòîãî ôàéëà
	const int	testFileSize		= 500000;
	char		testFile[testFileSize];
	char		testFileName[]		= "TestFile.txt";
	unsigned int fileFirstCluster	= 10;

	for (int i = 0; i < testFileSize; i++)	testFile[i] = i;

	writeFileDataRegion	(testFile, testFileSize, fileFirstCluster);
	writeFileFatTable	(testFileSize, fileFirstCluster);
	writeFileEntry		(testFileName, 1, testFileSize, fileFirstCluster);
	writeFileBitMap		(testFileSize, fileFirstCluster);

	// Ãåíåðàöèÿ òåñòîãî ôàéëà
	const int	testFileSize2		= 300000;
	char		testFile2[testFileSize2];
	char		testFileName2[]		= "TestFile.bin";
	fileFirstCluster				= 30;

	for (int i = 0; i < testFileSize2; i++)		testFile2[i] = 0xAA;
	
	writeFileDataRegion	(testFile2, testFileSize2, fileFirstCluster);
	writeFileFatTable	(testFileSize2, fileFirstCluster);
	writeFileEntry		(testFileName2, 2, testFileSize2, fileFirstCluster);
	writeFileBitMap		(testFileSize2, fileFirstCluster);

	// Çàïèñü êàðòèíêè íà îáðàç äèñêà
	const int	testFileSize3		= 21238; //21238;
	char		testFile3[testFileSize3];
	char		testFileName3[]		= "testPic.bmp";
	fileFirstCluster				= 41;

	ifstream inputFile("testPic.bmp");
	char next;
	for (int i = 0; i < testFileSize3; i++)
	{	
		next = inputFile.get();
		testFile3[i] = next;
	}
	writeFileDataRegion	(testFile3, testFileSize3, fileFirstCluster);
	writeFileFatTable	(testFileSize3, fileFirstCluster);
	writeFileEntry		(testFileName3, 3, testFileSize3, fileFirstCluster);
	writeFileBitMap		(testFileSize3, fileFirstCluster);
	
	cout << "\n";
	cout << testFileSize3;
	cout << "\n";
	cout << fileFirstCluster;
	cout << "\n";

	inputFile.close();


	
	// Çàïèñü îáðàçà äèñêà â  áèíàðíûé ôàéë
	ofstream fout("ExFat.bin" , std::ios::binary);
	for (int i = 0; i < volumeSize; i++)	fout << volumeExFat[i];
	fout.close();

	// Ñîçäàíèå VHD îáðàçà äèñêà
	createVhdHeadToImage();

	int a;
	cin  >> a;
	return 0;
}


void printExFatParametrs ()
{
	cout << bytePerSector		<< "		Byte Per Sector Shift\n";
	cout << sectorsPerCluster	<< "		Sectors Per Cluster\n";
	cout << clusterCount		<< "		Clusters Per Volume\n";
	cout << volumeSize			<< "		Bytes Per Volume\n";
	cout << (volumeSize/1024)	<< "		KBytes Per Volume\n";
	cout << "\n";
	cout.hex << 1024;
	cout.width(8) << "12334435345";
	cout << "asdasd asd ";
}

void setBootRegionBootSector ()
{
	
	unsigned char exFatBootSector[bytePerSector];
	// exFAT IMAGE / MAIN BOOT REGION / BOOT SECTOR		(Offset, sectors = 0. Size, sectors = 1)
	// JUMP BOOT.  0xEB7690; Ñìåùåíèå 0, Ðàçìåð 3.
 	exFatBootSector[0] = 0xEB;	exFatBootSector[1] = 0x76;	exFatBootSector[2] = 0x90;

	// File System Name. "EXFAT> Ñìåùåíèå 3, Ðàçìåð 8.
 	exFatBootSector[3]  = 'E'; 	exFatBootSector[4]  = 'X';	exFatBootSector[5]  = 'F'; 	exFatBootSector[6]  = 'A';
	exFatBootSector[7]  = 'T';	exFatBootSector[8]  = 0x20; 	exFatBootSector[9]  = 0x20;	exFatBootSector[10] = 0x20;

	// Must Be Zero. Ñìåùåíèå 11, Ðàçìåð 53.
	for (int i = 11; i < (11 + 53); i++) exFatBootSector[i] = 0;

	// Partition Offset. In sectors; if 0, shall be ignored; Ñìåùåíèå 64, Ðàçìåð 8.
 	exFatBootSector[64] = 0;	exFatBootSector[65] = 0;	exFatBootSector[66] = 0; exFatBootSector[67] = 0;
 	exFatBootSector[68] = 0;	exFatBootSector[69] = 0;	exFatBootSector[70] = 0; exFatBootSector[71] = 0;

	// Volume Length. Size of exFAT volume in sectors. Ñìåùåíèå 72, Ðàçìåð 8.
	int volumeLengthTemp = volumeLengthL;
	for (int i = 72; i < 72 + 4; i++){
		exFatBootSector[i]	= volumeLengthTemp % 256;
		volumeLengthTemp	= volumeLengthTemp / 256;
	}
	volumeLengthTemp = volumeLengthH;
	for (int i = 76 ; i < 76 + 4; i++){
		exFatBootSector[i]	= volumeLengthTemp % 256;
		volumeLengthTemp	= volumeLengthTemp / 256;
	}
	// FAT Offset. In sectors. Ñìåùåíèå 80, Ðàçìåð 4.
	int fatOffsetTemp = fatOffset;
	for (int i = 80; i < 80 + 4; i++ ) {
		exFatBootSector[i]	= fatOffsetTemp % 256;
		fatOffsetTemp		= fatOffsetTemp / 256;
	}	
	// FAT Lendth. In sectors. May exceed the required space in order to align the second FAT. Ñìåùåíèå 84, Ðàçìåð 4.
	int fatLendthTemp = fatLendth;
	for (int i = 84; i < 84 + 4; i++){
		exFatBootSector[i]	= fatLendthTemp % 256;
		fatLendthTemp		= fatLendthTemp / 256;
	}	
	// Cluster Heap Offset. In sectors. Ñìåùåíèå 88, Ðàçìåð 4.
	int clusterHeapOffsetTemp	   = clusterHeapOffset;
	for (int i = 88; i < 88 + 4; i++){
		exFatBootSector[i]		= clusterHeapOffsetTemp % 256;
		clusterHeapOffsetTemp	= clusterHeapOffsetTemp / 256;
	}
	// Cluster Count. 2^32-11 is the maximum number of clusters could be described. Ñìåùåíèå 92, Ðàçìåð 4.
	int clusterCountTemp	= clusterCount;
	for (int i = 92; i < 92 + 4; i++){
		exFatBootSector[i]	= clusterCountTemp % 256;
		clusterCountTemp	/= 256;
	}
	// Root Directory Cluster. Ñìåùåíèå 96, Ðàçìåð 4.
	unsigned int rootDirectoryCluster = 0x5;
	for (int i = 96; i < 96 + 4; i++){
		exFatBootSector[i]		= rootDirectoryCluster % 256;
		rootDirectoryCluster	/= 256;
	}	
	// Volume Serial Number. Ñìåùåíèå 100, Ðàçìåð 4.
	unsigned int volumeSerialNumber = 0x8634EA57;
	for (int i = 100; i < 100 + 4; i++){
		exFatBootSector[i] = volumeSerialNumber % 256;
		volumeSerialNumber /= 256;
	}
	// File System Revision. Ñìåùåíèå 104, Ðàçìåð 2.
	unsigned int fileSystemRevision = 0x100;
	for (int i = 104; i < 104 + 2; i++){
		exFatBootSector[i] = fileSystemRevision % 256;
		fileSystemRevision /= 256;
	}
	// Volume Flag. Ñìåùåíèå 106, Ðàçìåð 2.
	unsigned int volumeFlag = 0;
	for (int i = 106; i < 106 + 2; i++){
		exFatBootSector[i]	= volumeFlag % 256;
		volumeFlag			/= 256;
	}
	// Byte Per Sector Shift. Power of 2. Minimum 9 (512 bytes per sector), maximum 12 (4096 bytes per sector). Ñìåùåíèå 108, Ðàçìåð 1.
	exFatBootSector[108] = bytePerSector2Power;
	// Sectors Per Cluster Shift. Power of 2. Minimum 0 (1 bytes per sector), maximum 25 - BytesPerSectorShift, so max cluster size is 32MB. Ñìåùåíèå 109, Ðàçìåð 1.
	exFatBootSector[109] = sectorsPerCluster2Power; 
	// Number of Fats. 2 is for TexFAT only. Ñìåùåíèå 110, Ðàçìåð 1.
	exFatBootSector[110] = 1;
	// Drive Select. Extended INT 13h drive number; typically 0x80. Ñìåùåíèå 111, Ðàçìåð 1.
	exFatBootSector[111] = 0x80;
	// Percent In Use. 0..100 - percentage of allocated clusters rounded down to the integer 0xFF - percentage is not availble. Ñìåùåíèå 112, Ðàçìåð 1.
	exFatBootSector[112] = 0x00;
	// Ðåçåðâ . Ñìåùåíèå 113, Ðàçìåð 7.
	for (int i = 113; i < (113 + 7); i++) exFatBootSector[i] = 0;
	// Boot Code. Ñìåùåíèå 120, Ðàçìåð 390.
	for (int i = 120; i < (120 + 390); i++) exFatBootSector[i] = exFatBootSectorBootCode[i - 120];
	// Boot Signature. 0xAA55 Ñìåùåíèå 510, Ðàçìåð 2.
	exFatBootSector[510] = 0x55;	exFatBootSector[511] = 0xAA;
	// Excess Space. Not used. Ñìåùåíèå 512. Ðàçìåð (2^bytePerSector - 512)
	for (int i = 512; i < (bytePerSector); i++) exFatBootSector[i] = 0;

	
	cout << "installation Boot Region \ Boot Sector \n";

	for (int i = 0; i < 1*bytePerSector; i++)
	{
		volumeExFat[i]						= exFatBootSector[i];		// Èíèöèàëèçàöèÿ Main Boot Region	
		volumeExFat[12*bytePerSector + i]	= exFatBootSector[i];		// Èíèöèàëèçàöèÿ Backup Boot Region	
	}

}

void setBootRegionExtendedBootSectors ()
{
	unsigned char exFatExtendedBootSectors[8*bytePerSector];

	// exFAT IMAGE / MAIN BOOT REGION / Extended Boot Sectors	(Offset, sectors = 1. Size, sectors = 8)
	for (int i = 0; i < (8*bytePerSector - 2); i++) exFatExtendedBootSectors[i] = 0x00;
	exFatExtendedBootSectors[1*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[1*bytePerSector - 1]	= 0xAA;
	exFatExtendedBootSectors[2*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[2*bytePerSector - 1]	= 0xAA;
	exFatExtendedBootSectors[3*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[3*bytePerSector - 1]	= 0xAA;
	exFatExtendedBootSectors[4*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[4*bytePerSector - 1]	= 0xAA;
	exFatExtendedBootSectors[5*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[5*bytePerSector - 1]	= 0xAA;
	exFatExtendedBootSectors[6*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[6*bytePerSector - 1]	= 0xAA;
	exFatExtendedBootSectors[7*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[7*bytePerSector - 1]	= 0xAA;
	exFatExtendedBootSectors[8*bytePerSector - 2]	= 0x55;
	exFatExtendedBootSectors[8*bytePerSector - 1]	= 0xAA;

	for (int i = 0; i < 8*bytePerSector; i++)
	{
		volumeExFat[bytePerSector + i]						= exFatExtendedBootSectors[i];		// Èíèöèàëèçàöèÿ Main Boot Region	
		volumeExFat[12*bytePerSector + bytePerSector + i]	= exFatExtendedBootSectors[i];		// Èíèöèàëèçàöèÿ Backup Boot Region	
	}	


	cout << "installation Boot Region \ Extended BootSectors \n";
}

void setBootRegionOemParametrs ()
{
	unsigned char exFatOemParametrs[bytePerSector];
	
	// exFAT IMAGE / MAIN BOOT REGION / OEM Parametrs			(Offset, sectors = 9. Size, sectors = 1)
	for (int i = 0; i < (bytePerSector); i++) exFatOemParametrs[i] = 0;		
		
	for (int i = 0; i < 1*bytePerSector; i++)
	{
		volumeExFat[9*bytePerSector  + i]					= exFatOemParametrs[i];		// Èíèöèàëèçàöèÿ Main Boot Region	
		volumeExFat[12*bytePerSector + 9*bytePerSector+  i]	= exFatOemParametrs[i];		// Èíèöèàëèçàöèÿ Backup Boot Region	
	}	
	
	cout << "installation Boot Region \ Oem Parametrs \n";

}

void setBootRegionReserved ()
{
	unsigned char exFatReserved[bytePerSector];

	// exFAT IMAGE / MAIN BOOT REGION / Reserved				(Offset, sectors = 10. Size, sectors = 1)
	for (int i = 0; i < (bytePerSector); i++) exFatReserved[i] = 0;		
	
	for (int i = 0; i < 1*bytePerSector; i++)
	{
		volumeExFat[10*bytePerSector  + i]						= exFatReserved[i];			// Èíèöèàëèçàöèÿ Main Boot Region	
		volumeExFat[12*bytePerSector + 10*bytePerSector+  i]	= exFatReserved[i];		// Èíèöèàëèçàöèÿ Backup Boot Region	
	}	

	cout << "installation Boot Region \ Reserved \n";
}

void setBootRegionChecksum ()
{
	unsigned char exFatBootChecksum[bytePerSector];

	// exFAT IMAGE / MAIN BOOT REGION / BOOT CHECKSUM			(Offset, sectors = 11. Size, sectors = 1)
	unsigned int BootChecksum;

	for (int i = 0; i < 11*bytePerSector; i++)	{
		if (i == 106 ||i==107 || i==122) continue;
		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + volumeExFat[i];
	}
	
/*
	for (int i = 0; i < bytePerSector; i++)	{
		if (i == 106 ||i==107 || i==122) continue;
		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatBootSector[i];
	}
	for (int i = 0; i < 8*bytePerSector; i++)	BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatExtendedBootSectors[i];	// Checksum Extended Boot Sectors
	for (int i = 0; i < bytePerSector; i++)		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatOemParametrs[i];			// Checksum OEM Parametrs
	for (int i = 0; i < bytePerSector; i++)		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatReserved[i];				// Checksum Reserved
	
	*/


	for (int i = 0; i < (bytePerSector/4); i++){
		unsigned int chsum = BootChecksum;
		exFatBootChecksum[4*i+3]	= chsum % 256;
		chsum /= 256;
		exFatBootChecksum[4*i+2]	= chsum % 256;
		chsum /= 256;
		exFatBootChecksum[4*i+1]	= chsum % 256;
		chsum /= 256;
		exFatBootChecksum[4*i]		= chsum % 256;
		chsum /= 256;
	}	

	for (int i = 0; i < 1*bytePerSector; i++)
	{
		volumeExFat[11*bytePerSector  + i]						= exFatBootChecksum[i];			// Èíèöèàëèçàöèÿ Main Boot Region	
		volumeExFat[12*bytePerSector  + 11*bytePerSector+  i]	= exFatBootChecksum[i];		// Èíèöèàëèçàöèÿ Backup Boot Region	
	}	
	
	cout << "installation Boot Region Checksum \n\n";
}
	
void setFatRegionFatAligment (){
	// exFAT IMAGE / FAT REGION	/ FAT ALIGMENT		(Offset, sectors = 0. Size, sectors = fatOffset - 24)
	unsigned char exFatFatAligment[(fatOffset-24)*bytePerSector];
	
	for (int i = 0; i < (fatOffset-24)*bytePerSector; i++) exFatFatAligment[i] = 0;
		
	for (int i = 0; i < (fatOffset-24)*bytePerSector; i++) volumeExFat[24*bytePerSector  + i]	= exFatFatAligment[i];	
		
	cout << "installation FAT Region FAT Aligment \n";
}

void setFatRegionFirstFat (){
	// exFAT IMAGE / FAT REGION	/ First FAT			(Offset, sectors = fatOffset. Size, sectors = fatLength)
	// Ôîðìèðîâàíèå FAT òàáëèöû
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		

	unsigned char FATFirstFat[fatLendth*bytePerSector];
	// FatEntry[0]:
	FATFirstFat[0] = 0xF8;  FATFirstFat[1] = 0xFF; 	FATFirstFat[2] = 0xFF;	FATFirstFat[3] = 0xFF;
	// FatEntry[1]:
	FATFirstFat[4] = 0xFF;	FATFirstFat[5] = 0xFF;	FATFirstFat[6] = 0xFF;	FATFirstFat[7] = 0xFF;
	// First Cluster - Last Cluster:
	for (unsigned int i = 0; i < 4*clusterCount; i++)	FATFirstFat[8 + i] = 0;			// ïåðåáèðàåì êëàñòåðû
	for (int i = 0; i < fatLendth*bytePerSector; i++) volumeExFat[fatOffset*bytePerSector  + i]	= FATFirstFat[i];	
	// uiExcessEntry
	for (int i = ((fatOffset + fatLendth - 1) * bytePerSector + 4*clusterCount + 8); i < ((fatOffset + fatLendth * numberOfFats) * bytePerSector); i++)	volumeExFat[i]	= 0x00;
	cout << "installation FAT Region First FAT \n\n";


}


void setDataRegionClusterHeapAligment(){

	int sizeClusterHeapAligment = (clusterHeapOffset-(fatOffset + fatLendth * numberOfFats)) * bytePerSector;

	for (int i = 0; i < (sizeClusterHeapAligment); i++) volumeExFat[(fatOffset + fatLendth * numberOfFats) * bytePerSector + i] = 0;		
		
	cout << "installation Data Region \ Cluster Heap Aligment \n";


}

void setDataRegionVolumeLabel(){
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/	
	// Volume Label Directory Entry:
	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector ;
	unsigned char volumeLabel[32];
	volumeLabel[0]  = 0x83;																		// Entry Type
	volumeLabel[1]  = 10;																		// Character Count
	
		for (int i = 0;  i < (10); i++){
			volumeLabel [2*i+2]	= exFatVolumeLabel [i];	
			volumeLabel [2*i+1+2] = 0x00;	
		} 
	for (int i = (20 + 2); i < 32; i++) volumeLabel [i] = 0;						// Reserved
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/	

	for (int i = 0; i < 32; i++) volumeExFat[offsetEntry + i]	= volumeLabel[i];	

	cout << "installation Data Region Volume Label \n";
}

void setDataRegionUpcaseTable(){

	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 1) * bytePerSector ;
	
	for (int i = 0; i < upcaseTableSize; i++) volumeExFat[offsetEntry + i]	= upcaseTable[i];	

	cout << "installation Data Region UpCase Table \n";
}

void setDataRegionAllocationBitMapEntry(){
	
	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32;
	
	unsigned char bitmapAllocationTableEntry[32] = 
	{0x81, 0x0, 0x0, 0x0,	0x0, 0x0, 0x0, 0x0,			0x0, 0x0, 0x0, 0x0,		0x0, 0x0, 0x0, 0x0,
	  0x0, 0x0, 0x0, 0x0,	0x02, 0x0, 0x0, 0x0,		0xDC, 0x05, 0x0, 0x0,	0x0, 0x0, 0x0, 0x0};

	for (int i = 0; i < 32; i++) volumeExFat[offsetEntry + i]	= bitmapAllocationTableEntry[i];	

	cout << "installation Data Region Allocation BitMap Table Entry \n";

}
	
void setDataRegionUpCaseTableEntry(){
	
	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32 + 32;
	
	unsigned char upCaseTableEntry[32] = 
	{0x82, 0x0, 0x0, 0x0,	0x0D, 0xD3, 0x19, 0xE6,			0x0, 0x0, 0x0, 0x0,		0x0, 0x0, 0x0, 0x0,
	  0x0, 0x0, 0x0, 0x0,	0x03, 0x0, 0x0, 0x0,			0xCC, 0x16, 0x0, 0x0,	0x0, 0x0, 0x0, 0x0};

	for (int i = 0; i < 32; i++) volumeExFat[offsetEntry + i]	= upCaseTableEntry[i];	

	cout << "installation Data Region Up Case Table Entry \n\n";

}


void writeFileEntry(char fileName[], unsigned int fileNumber , unsigned int fileLength, unsigned int fileFirstCluster){

	// File Directory Entry
	// ------------------------------------------------------------------------------------------------------------------------------
	unsigned int  offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32*(3*fileNumber);
	
	unsigned char fileEntry[32];
	fileEntry[0] = 0x85;			// Type
	fileEntry[1] = 0x02;			// Secondary Entries
	fileEntry[2] = 0x28;			// File Name Length

	unsigned int fileNameHash	= 0x3CAD;
	fileEntry[3] = fileNameHash / 256; 	fileEntry[4] = fileNameHash % 256;
	
	fileEntry[5] = 0x00; 	fileEntry[6] = 0x00; 
		
	unsigned int fileEntryCreate	= 0x4462863B;
	fileEntry[8]		= fileEntryCreate % 256;
	fileEntryCreate	   /= 256;
	fileEntry[9]		= fileEntryCreate % 256;
	fileEntryCreate	   /= 256;
	fileEntry[10]		= fileEntryCreate % 256;
	fileEntry[11]		= fileEntryCreate / 256;

	unsigned int fileEntryModified	= 0xF162BA3A;
	fileEntry[12]		= fileEntryModified % 256;
	fileEntryModified	/= 256;
	fileEntry[13]		= fileEntryModified % 256;
	fileEntryModified  /= 256;
	fileEntry[14]		= fileEntryModified % 256;
	fileEntry[15]		= fileEntryModified / 256;

	fileEntry[16]		= fileEntry[8];
	fileEntry[17]		= fileEntry[9];
	fileEntry[18]		= fileEntry[10];
	fileEntry[19]		= fileEntry[11];
		
	fileEntry[20]		= 0xA8;			// Create 10ms
	fileEntry[21]		= 0x00;			// Modified 10ms
	fileEntry[22]		= 0xEC;			// TX Offset CMA EC = GMT - 5
	fileEntry[23]		= 0xEC;			// TX Offset CMA EC = GMT - 5
	
	fileEntry[24]		= 0xEC;			// TX Offset CMA EC = GMT - 5
	fileEntry[25]		= 0x00;
	fileEntry[26]		= 0x00;
	fileEntry[27]		= 0x00;
	fileEntry[28]		= 0x00;
	fileEntry[29]		= 0x00;
	fileEntry[30]		= 0x00;
	fileEntry[31]		= 0x00;
	
	for (int i = 0; i < 32; i++) volumeExFat[offsetEntry + i]	= fileEntry[i];	
	
	// Stream Extension Directory Entry
	// ------------------------------------------------------------------------------------------------------------------------------
	offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32*(3*fileNumber + 1);
	
	unsigned char fileStreamEntry[32];
	fileStreamEntry[0] = 0xC0;			// Type
	fileStreamEntry[1] = 0x03;			// Flags
	fileStreamEntry[2] = 0x00;			// Reserved 

	unsigned int fileEntryCheckSum	= 0x92D4;
	fileStreamEntry[2] = fileEntryCheckSum / 256; 	fileStreamEntry[3] = fileEntryCheckSum % 256;
	
	fileStreamEntry[4] = 0x20; 	fileStreamEntry[5] = 0x00;	// Attributes (0x0020 = Archive)
	fileStreamEntry[6] = 0x00; 	fileStreamEntry[7] = 0x00;	// 

	unsigned int fileLengthL= fileLength;
	fileStreamEntry[8]		= fileLengthL % 256;
	fileLengthL				/= 256;
	fileStreamEntry[9]		= fileLengthL % 256;
	fileLengthL				/= 256;
	fileStreamEntry[10]		= fileLengthL % 256;
	fileStreamEntry[11]		= fileLengthL / 256;

	fileStreamEntry[12]		=	0x00;
	fileStreamEntry[13]		=	0x00;
	fileStreamEntry[14]		=	0x00;
	fileStreamEntry[15]		=	0x00;

	fileStreamEntry[16]		=	0x00;	// Reserved
	fileStreamEntry[17]		=	0x00;	// Reserved
	fileStreamEntry[18]		=	0x00;	// Reserved
	fileStreamEntry[19]		=	0x00;	// Reserved
	
	unsigned int fileCluster = fileFirstCluster;			// First Cluster of File
	fileStreamEntry[20]		= fileCluster % 256;
	fileCluster				/= 256;
	fileStreamEntry[21]		= fileCluster % 256;
	fileCluster				/= 256;
	fileStreamEntry[22]		= fileCluster % 256;
	fileStreamEntry[23]		= fileCluster / 256;

	fileStreamEntry[24]		= fileStreamEntry[8];
	fileStreamEntry[25]		= fileStreamEntry[9];
	fileStreamEntry[26]		= fileStreamEntry[10];
	fileStreamEntry[27]		= fileStreamEntry[11];
	fileStreamEntry[28]		= fileStreamEntry[12];			
	fileStreamEntry[29]		= fileStreamEntry[13];			
	fileStreamEntry[30]		= fileStreamEntry[14];			
	fileStreamEntry[31]		= fileStreamEntry[15];			

	for (int i = 0; i < 32; i++) volumeExFat[offsetEntry + i]	= fileStreamEntry[i];	
		
	// File Name Extension Directory Entry
	// ------------------------------------------------------------------------------------------------------------------------------
	offsetEntry = (clusterHeapOffset + sectorsPerCluster * 3) * bytePerSector + 32*(3*fileNumber + 2);

	unsigned char fileNameEntry[32];
	fileNameEntry[0] = 0xC1;			// Type
	fileNameEntry[1] = 0x00;			// Reserved

	cout << "installation File Entries:	";

	int index = 0;
	while ((fileName[index] != '\0') && (index < 15))
	{
		fileNameEntry [2*index + 2] = fileName[index];
		cout << fileName[index];
		fileNameEntry [2*index + 3] = 0x00;
		index ++;
	}
	for (int i = index; i < 15; i++){
		fileNameEntry [2*index + 2]	= 0x00;
		fileNameEntry [2*index + 3] = 0x00;	
	}
	cout << "\n";
	
	for (int i = 0; i < 32; i++) volumeExFat[offsetEntry + i]	= fileNameEntry[i];	

}

void writeFileDataRegion(char file[], int fileSize, int firstClusterOfFile){
	int offsetFileStart = (clusterHeapOffset + sectorsPerCluster * (firstClusterOfFile - 2)) * bytePerSector;
	for (int i = 0; i < fileSize; i++){
		volumeExFat[offsetFileStart + i] = file[i];
	}
}

void writeFileBitMap(int fileSize, int firstClusterOfFile){
	
	int offsetBitMap		= (clusterHeapOffset + sectorsPerCluster * (0)) * bytePerSector;					// Ñ÷èòàåì ñìåùåíèå Bit map  â îáùåé ñòðóêòóðå äèñêà
	int lastClusterOfFile	= firstClusterOfFile + ((fileSize / (sectorsPerCluster * bytePerSector)) + 1);		// C÷èòàåì íîìåð ïîñëåäíåãî êëàñòåðà ôàéëà

	int indexFirst	= (firstClusterOfFile - 2) / 8;			// Ñ÷èòàåì ïåðâûé áàéò â bitmap table
	int indexLast	= (lastClusterOfFile - 2) / 8;			// Ñ÷èòàåì ïîñëåäíèé áàéò â bitmap table
	
	// Çàïîëíåíèå áèòìàï òàáëèöû
	for (int i = indexFirst; i <= indexLast; i++){
		if (indexFirst == indexLast)	volumeExFat[offsetBitMap + i] = volumeExFat[offsetBitMap + i] | bitMapMaskTable[((firstClusterOfFile - 2) % 8)][((lastClusterOfFile - 2) % 8)];
		else {
			if		(i == indexFirst)	volumeExFat[offsetBitMap + i] = volumeExFat[offsetBitMap + i] | bitMapMaskTable[((firstClusterOfFile - 2) % 8)][7];
			else if (i == indexLast)	volumeExFat[offsetBitMap + i] = volumeExFat[offsetBitMap + i] | bitMapMaskTable[0][((lastClusterOfFile - 2) % 8)];
			else 						volumeExFat[offsetBitMap + i] = 0xFF;
		}
	}

}

void writeFileFatTable(int fileSize, int firstClusterOfFile){
	
	int numberOfClusterForFile	= (fileSize / (sectorsPerCluster * bytePerSector)) + 1;		// Ñ÷èòàåì ÷èñëî êëàñòåðîâ, êîòîðûå çàíèìàåò ôàéë
	int offsetFileStart = fatOffset*bytePerSector + firstClusterOfFile*4 + 4;				// Ñ÷èòàåì ñìåùåíèå FAT TABLE  â îáùåé ñòðóêòóðå äèñêà

	for (int i = 0; i < numberOfClusterForFile - 1; i++){
	
		unsigned int numberNextCluster = firstClusterOfFile + i + 1;
		volumeExFat[offsetFileStart + 4*i]		= numberNextCluster % 256;
		numberNextCluster	/= 256; 
		volumeExFat[offsetFileStart + 4*i + 1]	= numberNextCluster % 256;
		numberNextCluster /= 256;
		volumeExFat[offsetFileStart + 4*i + 2]	= numberNextCluster % 256;
		numberNextCluster /= 256;
		volumeExFat[offsetFileStart + 4*i + 3]	= numberNextCluster;	
	}
		unsigned int numberNextCluster = 0xFFFFFFFF;
		int offsetLastCluster				= offsetFileStart + 4*(numberOfClusterForFile - 1);
		volumeExFat[offsetLastCluster]		= numberNextCluster % 256;
		numberNextCluster					/= 256; 
		volumeExFat[offsetLastCluster + 1]	= numberNextCluster % 256;
		numberNextCluster					/= 256;
		volumeExFat[offsetLastCluster + 2]	= numberNextCluster % 256;
		numberNextCluster					/= 256;
		volumeExFat[offsetLastCluster + 3]	= numberNextCluster;

}


void createVhdHeadToImage (){
	
	ofstream imageOut	("ExFatVhd.vhd" , std::ios::binary);
	ifstream imageEntry	("ExFatEntry"	, std::ios::binary);

	const int		entrySize = 65536;
	char			charImage;
	unsigned char	entryImage[entrySize];

	for (int i = 0; i < entrySize; i++)		entryImage[i] = imageEntry.get(); 
	for (int i = 0; i < entrySize; i++)		imageOut << entryImage[i];
	for (int i = 0; i < volumeSize; i++)	imageOut << volumeExFat[i];
		
	imageOut.close();
	imageEntry.close();
}
