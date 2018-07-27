#include "stdafx.h"
#include "exfat.h"
#include <iostream> 
#include <fstream>
using namespace std;




void setBootSector (unsigned char [])
{
	// exFAT IMAGE / MAIN BOOT REGION / BOOT SECTOR		(Offset, sectors = 0. Size, sectors = 1)
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		

	// JUMP BOOT.  0xEB7690; Смещение 0, Размер 3.
 	exFatBootSector[0] = 0xEB;	exFatBootSector[1] = 0x76;	exFatBootSector[2] = 0x90;

	// File System Name. "EXFAT> Смещение 3, Размер 8.
 	exFatBootSector[3]  = 'E'; 	exFatBootSector[4]  = 'X';	exFatBootSector[5]  = 'F'; 	exFatBootSector[6]  = 'A';
	exFatBootSector[7]  = 'T';	exFatBootSector[8]  = 0x0; 	exFatBootSector[9]  = 0x0;	exFatBootSector[10] = 0x0;

	// Must Be Zero. Смещение 11, Размер 53.
	for (int i = 11; i < (11 + 53); i++) exFatBootSector[i] = 0;

	// Partition Offset. In sectors; if 0, shall be ignored; Смещение 64, Размер 8.
 	exFatBootSector[64] = 0;	exFatBootSector[65] = 0;	exFatBootSector[66] = 0; exFatBootSector[67] = 0;
 	exFatBootSector[68] = 0;	exFatBootSector[69] = 0;	exFatBootSector[70] = 0; exFatBootSector[71] = 0;

	// Volume Length. Size of exFAT volume in sectors. Смещение 72, Размер 8.
	int volumeLengthTemp = volumeLengthL;
	for (int i = 72; i < 72 + 4; i++){
		exFatBootSector[i]	= volumeLengthTemp % 256;
		volumeLengthTemp	= volumeLengthTemp / 256;
	}
	cout << "volumeLengthL\n";
	volumeLengthTemp = volumeLengthH;
	for (int i = 76 ; i < 76 + 4; i++){
		exFatBootSector[i]	= volumeLengthTemp % 256;
		volumeLengthTemp	= volumeLengthTemp / 256;
	}

	cout << "volumeLengthH\n";
	// FAT Offset. In sectors. Смещение 80, Размер 4.
	int fatOffsetTemp = fatOffset;
	for (int i = 80; i < 80 + 4; i++ ) {
		exFatBootSector[i]	= fatOffsetTemp % 256;
		fatOffsetTemp		= fatOffsetTemp / 256;
	}
	
	cout << "fatOffset\n";
	// FAT Lendth. In sectors. May exceed the required space in order to align the second FAT. Смещение 84, Размер 4.
	int fatLendthTemp = fatLendth;
	for (int i = 84; i < 84 + 4; i++){
		exFatBootSector[i]	= fatLendthTemp % 256;
		fatLendthTemp		= fatLendthTemp / 256;
	}	
	cout << "fatLendth\n";
	// Cluster Heap Offset. In sectors. Смещение 88, Размер 4.
	int clusterHeapOffsetTemp	   = clusterHeapOffset;
	for (int i = 88; i < 88 + 4; i++){
		exFatBootSector[i]		= clusterHeapOffsetTemp % 256;
		clusterHeapOffsetTemp	= clusterHeapOffsetTemp / 256;
	}
	cout << "clusterHeapOffset\n";
	// Cluster Count. 2^32-11 is the maximum number of clusters could be described. Смещение 92, Размер 4.
	int clusterCountTemp	= clusterCount;
	for (int i = 92; i < 92 + 4; i++){
		exFatBootSector[i]	= clusterCountTemp % 256;
		clusterCountTemp	= clusterCountTemp / 256;
	}
	cout << "clusterCount\n";
	// Root Directory Cluster. Смещение 96, Размер 4.
	unsigned int rootDirectoryCluster = 0x5;
	for (int i = 96; i < 96 + 4; i++){
		exFatBootSector[i]		= rootDirectoryCluster % 256;
		rootDirectoryCluster	= rootDirectoryCluster / 256;
	}	
	cout << "rootDirectoryCluster\n";
	// Volume Serial Number. Смещение 100, Размер 4.
	unsigned int volumeSerialNumber = 0x10000001;
	for (int i = 100; i < 100 + 4; i++){
		exFatBootSector[i] = volumeSerialNumber % 256;
		volumeSerialNumber = volumeSerialNumber / 256;
	}
	cout << "volumeSerialNumber\n";
	// File System Revision. Смещение 104, Размер 2.
	unsigned int fileSystemRevision = 0x100;
	for (int i = 104; i < 104 + 2; i++){
		exFatBootSector[i] = fileSystemRevision % 256;
		fileSystemRevision = fileSystemRevision / 256;
	}
	cout << "fileSystemRevision\n";
	// Volume Flag. Смещение 106, Размер 2.
	unsigned int volumeFlag = 0;
	for (int i = 106; i < 106 + 2; i++){
		exFatBootSector[i] = volumeFlag % 256;
		volumeFlag = volumeFlag / 256;
	}
	cout << "volumeFlag\n";
	// Byte Per Sector Shift. Power of 2. Minimum 9 (512 bytes per sector), maximum 12 (4096 bytes per sector). Смещение 108, Размер 1.
	exFatBootSector[108] = bytePerSector2Power;

	// Sectors Per Cluster Shift. Power of 2. Minimum 0 (1 bytes per sector), maximum 25 - BytesPerSectorShift, so max cluster size is 32MB. Смещение 109, Размер 1.
	exFatBootSector[109] = sectorsPerCluster2Power; 

	// Number of Fats. 2 is for TexFAT only. Смещение 110, Размер 1.
	exFatBootSector[110] = 1;

	// Drive Select. Extended INT 13h drive number; typically 0x80. Смещение 111, Размер 1.
	exFatBootSector[111] = 0x80;

	// Percent In Use. 0..100 - percentage of allocated clusters rounded down to the integer 0xFF - percentage is not availble. Смещение 112, Размер 1.
	exFatBootSector[112] = 0xFF;

	// Резерв . Смещение 113, Размер 7.
	for (int i = 113; i < (113 + 7); i++) exFatBootSector[i] = 0;

	// Boot Code. Смещение 120, Размер 390.
	for (int i = 120; i < (120 + 390); i++) exFatBootSector[i] = 0;

	// Boot Signature. 0xAA55 Смещение 510, Размер 2.
	exFatBootSector[510] = 0x55;	exFatBootSector[511] = 0xAA;

	// Excess Space. Not used. Смещение 512. Размер (2^bytePerSector - 512)
	for (int i = 512; i < (bytePerSector); i++) exFatBootSector[i] = 0;



	
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/
	// exFAT IMAGE / MAIN BOOT REGION / Extended Boot Sectors	(Offset, sectors = 1. Size, sectors = 8)
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		
	char exFatExtendedBootSectors[8*bytePerSector];
	for (int i = 0; i < (8*bytePerSector); i++) exFatExtendedBootSectors[i] = 0;




	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		
	// exFAT IMAGE / MAIN BOOT REGION / OEM Parametrs			(Offset, sectors = 9. Size, sectors = 1)
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		
	char exFatOemParametrs[bytePerSector];
	for (int i = 0; i < (bytePerSector); i++) exFatOemParametrs[i] = 1;
	



	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		
	// exFAT IMAGE / MAIN BOOT REGION / Reserved				(Offset, sectors = 10. Size, sectors = 1)
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		
	char exFatReserved[bytePerSector];
	for (int i = 0; i < (bytePerSector); i++) exFatReserved[i] = 0;
	



	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		
	// exFAT IMAGE / MAIN BOOT REGION / BOOT CHECKSUM			(Offset, sectors = 11. Size, sectors = 1)
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/		
	char exFatBootChecksum[bytePerSector];
	unsigned int BootChecksum;
	for (int i = 0; i < bytePerSector; i++)	{
		if (i == 106 ||i==107 || i==122) continue;
		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatBootSector[i];
	}
	for (int i = 0; i < 8*bytePerSector; i++)	BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatExtendedBootSectors[i];	// Checksum Extended Boot Sectors
	for (int i = 0; i < bytePerSector; i++)		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatOemParametrs[i];			// Checksum OEM Parametrs
	for (int i = 0; i < bytePerSector; i++)		BootChecksum = (BootChecksum << 31) | (BootChecksum >> 1) + exFatReserved[i];				// Checksum Reserved
	

	for (int i = 0; i < (bytePerSector/4); i++){
		unsigned int chsum = BootChecksum;
		exFatBootChecksum[4*i+3]	= chsum % 256;
		chsum = chsum / 256;
		exFatBootChecksum[4*i+2]	= chsum % 256;
		chsum = chsum / 256;
		exFatBootChecksum[4*i+1]	= chsum % 256;
		chsum = chsum / 256;
		exFatBootChecksum[4*i]		= chsum % 256;
		chsum = chsum / 256;
	}	
	/* -------------------------------------------------------------------------------------------------------------------------------------------------	*/			
	



}