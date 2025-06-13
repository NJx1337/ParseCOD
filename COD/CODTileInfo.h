#pragma once
#include <widemath.h>

//const char C_TileTypeInfoParams[3][10] = {"Nummer", "Id", "Gfx"};

struct STileTypeInfo
{
	uint16_t Nummer; // Unique ID? ToDo:Check if needed
	uint16_t Id; // TileID
	uint16_t Gfx; // GfxID
	uint8_t Posoffs; // GroundHeight
	uint8_t SizeX; // Width
	uint8_t SizeY; // Height
	uint8_t Rotate; // (Rotate 1+ = w*h)
	uint8_t AnimAnz; // Amount of sprites for animations
	uint8_t HighFlg; // BuildingHeight?
	char* Description; // Comment

	STileTypeInfo(uint16_t _Id, uint16_t _Gfx, uint8_t _Posoffs, uint8_t _SizeX, uint8_t _SizeY, uint8_t _Rotate, uint8_t _AnimAnz, uint8_t _HighFlg)
		: Id(_Id), Gfx(_Gfx), Posoffs(_Posoffs), SizeX(_SizeX), SizeY(_SizeY), Rotate(_Rotate), AnimAnz(_AnimAnz), HighFlg(_HighFlg)
	{
	}

	STileTypeInfo(uint16_t _Id, uint16_t _Gfx, uint8_t _Posoffs, uint8_t _SizeX, uint8_t _SizeY, uint8_t _Rotate, uint8_t _AnimAnz, uint8_t _HighFlg, char* _Description)
		: Id(_Id), Gfx(_Gfx), Posoffs(_Posoffs), SizeX(_SizeX), SizeY(_SizeY), Rotate(_Rotate), AnimAnz(_AnimAnz), HighFlg(_HighFlg), Description(_Description)
	{
	}
};

class CODTileInfo // Usable structured information Haus object in haus.cod
{
private:
	STileTypeInfo m_Defaults_TileTypeInfo = STileTypeInfo(0, 0, 0, 0, 0, 0, 0, 0, 0);

	STileTypeInfo* m_TileTypes; // Array with TileTypeInfos
	int m_TileTypes_size = 0; // Size of Array with TileTypeInfos
	int m_TileTypes_sizeAllocated = 0; // TileTypeInfos-ArraySize that is preallocated in memory

	bool ParseMultiValSize(const char* valueStr, int* X, int* Y);

public:
	CODTileInfo();
	~CODTileInfo();

	void SetTileTypeDefaults();
	void SetTileTypeParameter(int Idx, char* name, int valueType, char* valueStr, int valueInt, float valueFloat, char* valueComment);
	void SetTileTypeComment(int Idx, char* valueComment);
	void CreateTileTypeObj(int ParentId = -1); // Create TileTypeObj, copy existing when provided ParentId

	void SortById();
	void PrintTileTypeObjects();

	inline STileTypeInfo* GetTileTypes() const { return m_TileTypes; }
	inline int GetTileTypesSize() const { return m_TileTypes_size; }
};

