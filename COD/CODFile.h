#pragma once

class CODTileInfo;

class CODFile
{
public:
	enum EInputFileType {
		IFT_COD, // Encoded file, originally used format
		IFT_TXT, // Decoded COD file in readable format
		IFT_HCACHE // Cache File with stored data (can be manipulated for testing purposes)
	};

private:
	char* m_decCOD; // Decoded Text of COD (readable format of COD)
	EInputFileType m_inputType = EInputFileType::IFT_COD;

public:
	CODFile(const char* CodFileName);
	CODFile(const char* FileName, bool isTextFile);
	CODFile(const char* FileName, EInputFileType InputFileType);
	~CODFile();
	void writeDecCOD(const char* DecCodFileName);
	static void writeHaeuserCacheFromCOD(const char* CacheFileName, CODTileInfo* tileInfoIn); // Write cache file
	inline char* GetRawDecCOD() const { return m_decCOD; }
	inline EInputFileType GetInputType() const { return m_inputType; }

private:
	void readByType(const char* FileName, EInputFileType InputFileType);
	void readCOD(const char* CodFileName);
	void readTxt(const char* TxtFileName);
};

