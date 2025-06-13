#pragma once

class COD;
class CODFile;
class CODTileInfo;

enum EInObject {
	NOOBJECT, // Beginning state, global constants, global parameters, ...
	BGRUPPE, // Building group = describes citizen wealth groups
	BAUINFRA, // Building infrastructure = describes requirements for buildings
	HAUS // House = describes every tile-type
};

enum EInObjectState {
	INIT, // Beginning state, waiting for NUMMER
	SETDEFAULTS, // Set defaults (this is always the first @Nummer: 0 object)
	CREATEOBJ, // Found NUMMER, wait for next step
	PROCESS // Processing OBJ parameters
};

class CODParser
{
private:
	COD* m_COD;
	CODTileInfo* m_CODTileInfo;
	EInObject m_InMainObject = EInObject::NOOBJECT;
	char** m_ObjectStack; // Stack of objects. Get added by "Objekt" and removed by "EndObj"
	int m_ObjectStack_size = 0;

	// Class parameter input temporary storage TEMP DEACTIVATED/no need sice clean obj structure for Haus in cod
	///char** m_InClassParam_names; // Array full of names from temporary class input parameters; This array is paired with x_types and x_values below
	///int* m_InClassParam_types; // Array full of types from temporary class input parameters (0 = int, 1 = Enum-Idx); This array is paired with x_names, x_values above and below
	///char** m_InClassParam_values; // Array full of values from temporary class input parameters; This array is paired with x_types and x_names above
	///int m_InClassParam_size = 0;
	
	// Class input temporary variables
	char* m_InClass_LastComment; // Top comment
	int m_InClass_LastId; // Last used Nummer-identifier
	int m_InClass_State = EInObjectState::INIT; // State of object processing

	


	void ParseCODFile(CODFile* CodFile);
	int ParseCODFileLine(const char* line, int linepos, int startcharoffset);

	int GetLastRelevantChar(const char* line, int start, size_t linelen) const;

	int ParseLeft(const char* left, const char* name, int linepos, int startcharoffset, char** valueStr, int* valueInt, float* valueFloat, char** valueComment); // Parses values

	const char* ParseLeftName(const char* left, int linepos, int startcharoffset);

	void ProcessObjectData(int linepos, int valueType, char* name, char* valueStr, int valueInt, float valueFloat, char* valueComment); // Processes by filling known data for objects

	void AddObjToStack(const char* ObjectName);
	bool RemoveLastObjFromStack();
	const char* DebugReturnObjNames() const;
	const char* DebugReturnMainObjName() const;
	inline const char* GetLastObjFromStack() const { return m_ObjectStack[m_ObjectStack_size - 1]; }

	void AddIntConsts();

	void ProcessLastComment(const char* rawCmnt);

public:
	CODParser(const char* CodFileName);
	CODParser(CODFile* CodFile);
	~CODParser();

	inline COD* GetCOD() const { return m_COD; }
	inline CODTileInfo* GetTileInfo() const { return m_CODTileInfo; }
};

