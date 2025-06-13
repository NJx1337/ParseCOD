#pragma once

class CODFile
{
private:
	char* m_decCOD; // Decoded Text of COD (readable format of COD)

public:
	CODFile(const char* CodFileName);
	CODFile(const char* FileName, bool isTextFile);
	void writeDecCOD(const char* DecCodFileName);
	inline char* GetRawDecCOD() const { return m_decCOD; }

private:
	void readCOD(const char* CodFileName);
	void readTxt(const char* TxtFileName);
};

