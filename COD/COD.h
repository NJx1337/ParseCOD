#pragma once
class COD
{
private:
	// Attention: Constants in COD files can be overwritten, also names are case sensitive!
	//std::vector<std::string> m_pubConstArr_names;
	char** m_pubConstArr_names; // Array full of names from public constants; This array is paired with x_types and x_values below
	int* m_pubConstArr_types; // Array full of types from public constants (0 = int, 1 = internal constant); This array is paired with x_names, x_values above and below
	int* m_pubConstArr_values; // Array full of values from public constants; This array is paired with x_types and x_names above
	int m_pubConstArr_size = 0;
	//char** m_pubConstArr_names; // Array full of names from public constants; This array is paired with x_values below
	//char** m_pubConstArr_values; // Array full of values from public constants; This array is paired with x_names above

	//char** m_pubVarArr_names = nullptr; // Array full of names from public variables; This array is paired with x_values below
	//char** m_pubVarArr_values = nullptr; // Array full of values from public variables; This array is paired with x_names above

	// Enums in COD files are used to bind objects together. The Enum acts as named element. Enums look like Constraints
	char** m_Enum_names; // Array full of names from Enums
	int m_Enum_size = 0;

public:
	COD();
	~COD();

	bool ExistsPubConst(const char* Name) const;
	int GetPubConstIdx(const char* Name) const;
	int GetPubConst(const char* Name) const;
	void AddPubConst(const char* Name, int Value, int Type = 0); // Adds Const if not exists, if exists do nothing
	void SetPubConst(const char* Name, int Value, int Type = 0); // Adds Const if not exists, if exists override value

	bool ExistsPubVar(const char* Name) const;
	bool GetPubVar(const char* Name, char& Value) const;
	void AddPubVar(const char* Name, const char* Value);

	bool ExistsEnumName(const char* Name) const;
	int GetEnumIdx(const char* Name) const;
	void SetEnum(const char* Name); // Adds Enum if not exists and return new Idx, if exists return Idx
	inline const char* GetEnumName(int idx) const { return m_Enum_names[idx]; }
	inline const char* GetPubConstName(int idx) const { return m_pubConstArr_names[idx]; }
	inline int GetPubConstByIdx(int idx) const { return m_pubConstArr_values[idx]; }
};

