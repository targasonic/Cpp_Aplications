#pragma once

#include "exfat.h"
#include <iostream> 
#include <fstream>

using namespace std;

class WriteFileToImage
{
public:
	void DataRegion(char file[], int fileSize, int firstClusterOfFile);
	// Добавление файла в дата регион диска

	void writeFileFatTable(int fileSize, int firstClusterOfFile);
	// Добавление информации о файле в ФАТ таблицу

	void writeFileBitMap(int fileSize, int firstClusterOfFile);
	// Добавление информации о файле в БИТ МЭП таблицу

	void writeFileEntry(char fileName[], unsigned int fileNumber, unsigned int fileLength, unsigned int fileFirstCluster);
	// Добавление информации о файле в поле заголовков
};
