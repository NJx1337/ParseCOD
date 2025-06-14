#include "CODFile.h"
#include "CODTileInfo.h"

#include <iostream>     // std::cout, std::ostream, std::ios
#include <fstream>

CODFile::CODFile(const char* CodFileName)
{
	readByType(CodFileName, EInputFileType::IFT_COD);
}

CODFile::CODFile(const char* FileName, bool isTextFile)
{
	if (isTextFile)
	{
		readByType(FileName, EInputFileType::IFT_TXT);
	}
	else
	{
		readByType(FileName, EInputFileType::IFT_COD);
	}
}

CODFile::CODFile(const char* FileName, EInputFileType InputFileType)
{
	readByType(FileName, InputFileType);
}

CODFile::~CODFile()
{
	printf("CODFile::~CODFile()\n");
	//free(m_decCOD);
}

void CODFile::writeDecCOD(const char* DecCodFileName)
{
	std::ofstream f(DecCodFileName);

	f << m_decCOD;

	f.close();
}


void CODFile::writeHaeuserCacheFromCOD(const char* CacheFileName, CODTileInfo* tileInfoIn)
{
	FILE* f;

	errno_t err;

	printf("Writing CacheFile '%s'.\n", CacheFileName);

	if ((err = fopen_s(&f, CacheFileName, "w")) != 0)
	{
		// File could not be opened. filepoint was set to NULL
		// error code is returned in err.
		// error message can be retrieved with strerror(err);
		//fprintf(stderr, "Error CODFile::writeCacheFromCOD(): Cannot open file '%s' for writing: %s\n", CacheFileName, strerror(err));
		// If your environment insists on using so called secure
		// functions, use this instead:
		//char buf[strerrorlen_s(err) + 1];
		char buf[2048];
		strerror_s(buf, sizeof buf, err);
		fprintf_s(stderr, "Error CODFile::writeCacheFromCOD(): Cannot open file '%s'for writing: %s\n", CacheFileName, buf);
	}
	else
	{
		// File was opened, filepoint can be used to read the stream.

		// Write header
		fprintf(f, "; CODHaeuserCacheFile v0.1\n");
		fprintf(f, "; Automatic generated cache file.\n\n");
		fprintf(f, "; Id Gfx SizeX SizeY Rotate Anim PosOffs HighFlg ;Description\n");

		STileTypeInfo* typeInfo = tileInfoIn->GetTileTypes();
		int typeInfo_size = tileInfoIn->GetTileTypesSize();

		for (int i = 0; i < typeInfo_size; ++i)
		{
			const STileTypeInfo& o = typeInfo[i];
			//printf(" %d. {Nummer: %d, Id: %d, Gfx: %d}\n", i, o.Nummer, o.Id, o.Gfx);
			fprintf(f, "%d %d %d %d %d %d %d %d ;%s\n", o.Id, o.Gfx, o.SizeX, o.SizeY, o.Rotate, o.AnimAnz, o.Posoffs, o.HighFlg, o.Description);
			//printf(" %d. {Id: %d, Gfx: %d, [%d,%d],R:%d,An:%d,POffs:%d,hFlg:%d} // %s\n", i, o.Id, o.Gfx, o.SizeX, o.SizeY, o.Rotate, o.AnimAnz, o.Posoffs, o.HighFlg, o.Description);
		}

		printf("Successfully written CacheFile '%s'.\n", CacheFileName);

		fclose(f);
	}
}

void CODFile::readByType(const char* FileName, EInputFileType InputFileType)
{
	switch (InputFileType)
	{
	case EInputFileType::IFT_TXT:
		m_inputType = EInputFileType::IFT_TXT;
		readTxt(FileName);

		break;

	case EInputFileType::IFT_HCACHE:
		m_inputType = EInputFileType::IFT_HCACHE;
		//readCacheFile(FileName);
		readTxt(FileName);

		break;

		//case EInputFileType::IFT_COD:
	default:
		m_inputType = EInputFileType::IFT_COD;
		readCOD(FileName);

		break;
	}
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
