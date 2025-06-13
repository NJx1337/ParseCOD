#include "CODFile.h"

#include <iostream>     // std::cout, std::ostream, std::ios
#include <fstream>

CODFile::CODFile(const char* CodFileName)
{
	readCOD(CodFileName);
}

CODFile::CODFile(const char* FileName, bool isTextFile)
{
	if (isTextFile)
	{
		readTxt(FileName);
	}
	else
	{
		readCOD(FileName);
	}
}

void CODFile::writeDecCOD(const char* DecCodFileName)
{
	std::ofstream f(DecCodFileName);

	f << m_decCOD;

	f.close();
}

void CODFile::readCOD(const char* CodFileName)
{
	std::ifstream f;

	f.open(CodFileName, std::ios::binary); // Open Cod file in binary
	if (f.is_open()) // Check if open was successfully
	{
		// Get filesize:
		std::streampos fsize = 0;
		fsize = f.tellg();
		f.seekg(0, std::ios::end);
		fsize = f.tellg() - fsize;

		f.seekg(0, f.beg);

		delete[] m_decCOD;
		m_decCOD = (char*)malloc(sizeof(char) * ((int)fsize + 1));
		int pos = -1;

		printf("New string size: %u\n", (int)(sizeof(char) * ((int)fsize + 1)));

		printf("Reading %s\n", CodFileName);
		while (true)
		{
			char convChar; // Get first offset
			char rawChar; // Get first offset
			//f.read(reinterpret_cast<char*>(&convChar), sizeof(convChar));
			//f.read(reinterpret_cast<char*>(&convChar), sizeof(char));
			if (f.read(&rawChar, sizeof(char)))
			{

				convChar = -rawChar & 0xff;

				//printf("%c", convChar);
				//sprintf_s()
				if (convChar != '\r')
					m_decCOD[++pos] = convChar;
			}
			else
			{
				m_decCOD[++pos] = '\0';
				printf("EOF at pos %d.\n", pos);
				break;
			}

		}

		f.close();

		printf("COD (size: %d):\n", (int)strlen(m_decCOD));
		//printf("%s\n", m_rawCOD);
	}
}

void CODFile::readTxt(const char* TxtFileName)
{
	std::ifstream f;

	f.open(TxtFileName, std::ios::binary); // Open Txt file in binary
	if (f.is_open()) // Check if open was successfully
	{
		// Get filesize:
		std::streampos fsize = 0;
		fsize = f.tellg();
		f.seekg(0, std::ios::end);
		fsize = f.tellg() - fsize;

		f.seekg(0, f.beg);

		delete[] m_decCOD;
		m_decCOD = (char*)malloc(sizeof(char) * ((int)fsize + 1));
		int pos = -1;

		printf("New string size: %u\n", (int)(sizeof(char) * ((int)fsize + 1)));

		printf("Reading %s\n", TxtFileName);
		while (true)
		{
			//char convChar; // Get first offset
			char rawChar; // Get first offset
			//f.read(reinterpret_cast<char*>(&convChar), sizeof(convChar));
			//f.read(reinterpret_cast<char*>(&convChar), sizeof(char));
			if (f.read(&rawChar, sizeof(char)))
			{

				//convChar = -rawChar & 0xff;

				//printf("%c", convChar);
				//sprintf_s()
				if (rawChar != '\r')
					m_decCOD[++pos] = rawChar;
			}
			else
			{
				m_decCOD[++pos] = '\0';
				printf("EOF at pos %d.\n", pos);
				break;
			}

		}

		f.close();

		printf("Txt (size: %d):\n", (int)strlen(m_decCOD));
		//printf("%s\n", m_rawCOD);
	}
}
