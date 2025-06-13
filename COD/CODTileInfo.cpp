#include "CODTileInfo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

CODTileInfo::CODTileInfo()
{

}

CODTileInfo::~CODTileInfo()
{
	free(m_TileTypes);
}

void CODTileInfo::SetTileTypeParameter(int Idx, char* name, int valueType, char* valueStr, int valueInt, float valueFloat, char* valueComment)
{
	//if (strcmp(name, "Nummer") == 0 || strcmp(name, "Id") == 0 || strcmp(name, "Gfx") == 0)
	//	printf("SetTileTypeParameter(%d, %s, %d, %s, %d)\n", Idx, name, valueType, valueStr, valueInt);

	if (Idx != -1) // Is NOT Default values
	{
		int name_offset = name[0] == '@' ? 1 : 0;

		if (strcmp(name + name_offset, "Nummer") == 0)
		{
			if (name_offset && Idx > 0) valueInt += m_TileTypes[Idx - 1].Nummer;
			m_TileTypes[Idx].Nummer = valueInt;
		}
		else if (strcmp(name + name_offset, "Id") == 0)
		{
			if (name_offset && Idx > 0) valueInt += m_TileTypes[Idx - 1].Id;
			else valueInt -= valueInt > 0 ? 20000 : 0; // Id's are stored +20000 in haueser.cod

			m_TileTypes[Idx].Id = valueInt;
			//printf("   %d. Id: %d\n", Idx, valueInt);
		}
		else if (strcmp(name + name_offset, "Gfx") == 0)
		{
			if (name_offset && Idx > 0) valueInt += m_TileTypes[Idx - 1].Gfx;
			m_TileTypes[Idx].Gfx = valueInt;
		}
		else if (strcmp(name, "Rotate") == 0)
		{
			m_TileTypes[Idx].Rotate = valueInt;
		}
		else if (strcmp(name, "Size") == 0)
		{
			//printf("size:%s\n", valueStr);
			//size_t len = strlen(rawCmnt);

			//printf("size:[%.*s,%.*s]\n", valueStr);
			//printf("s[%.*s,%.*s]\n", e1 + 1, valueStr, len - b2, valueStr + b2);
			//printf("s[%d,%d]\n", tmpIntX, tmpIntY);

			int X, Y;
			if (ParseMultiValSize(valueStr, &X, &Y))
			{
				//printf("s1[%d,%d]\n", X, Y);
				m_TileTypes[Idx].SizeX = X;
				m_TileTypes[Idx].SizeY = Y;
			}

			//m_TileTypes[Idx].Rotate = valueInt;
		}
		else if (strcmp(name, "AnimAnz") == 0)
		{
			m_TileTypes[Idx].AnimAnz = valueInt;
		}
		else if (strcmp(name, "Highflg") == 0)
		{
			m_TileTypes[Idx].HighFlg = valueInt;
		}
		else if (strcmp(name, "Posoffs") == 0)
		{
			m_TileTypes[Idx].Posoffs = valueInt;
		}
	}
	else // Defaults:
	{
		if (strcmp(name, "Nummer") == 0)
		{
			m_Defaults_TileTypeInfo.Nummer = valueInt;
		}
		else if (strcmp(name, "Id") == 0)
		{
			m_Defaults_TileTypeInfo.Id = valueInt;
		}
		else if (strcmp(name, "Gfx") == 0)
		{
			m_Defaults_TileTypeInfo.Gfx = valueInt;
		}
		else if (strcmp(name, "Rotate") == 0)
		{
			m_Defaults_TileTypeInfo.Rotate = valueInt;
		}
		else if (strcmp(name, "Size") == 0)
		{
			int X, Y;
			if (ParseMultiValSize(valueStr, &X, &Y))
			{
				//printf("s1[%d,%d]\n", X, Y);
				m_Defaults_TileTypeInfo.SizeX = X;
				m_Defaults_TileTypeInfo.SizeY = Y;
			}
		}
		else if (strcmp(name, "AnimAnz") == 0)
		{
			m_Defaults_TileTypeInfo.AnimAnz = valueInt;
		}
		else if (strcmp(name, "Highflg") == 0)
		{
			m_Defaults_TileTypeInfo.HighFlg = valueInt;
		}
		else if (strcmp(name, "Posoffs") == 0)
		{
			m_Defaults_TileTypeInfo.Posoffs = valueInt;
		}
	}
}

void CODTileInfo::SetTileTypeComment(int Idx, char* valueComment)
{
	size_t len = strlen(valueComment);
	char* tmp = (char*)malloc((len + 1) * sizeof(char));
	sprintf_s(tmp, len + 1, "%s", valueComment);

	if (!tmp)
	{
		printf("Error: Memory allocation CODTileInfo::SetTileTypeComment().\n");
		return;
	}

	m_TileTypes[m_TileTypes_size - 1].Description = tmp;
}

void CODTileInfo::CreateTileTypeObj(int ParentId /*= -1*/)
{
	//if (ParentId == 413)
	//{
	//	printf(".4934 |Objfill %d\n", ParentId);
	//}
	//if (ParentId != -1)
	//	printf("Objfill:%d\n", ParentId);

	if (m_TileTypes_size == m_TileTypes_sizeAllocated) // Check if memory needs to be allocated
	{
		// Resize memory
		STileTypeInfo* tmp = (STileTypeInfo*) (m_TileTypes_size > 0 ? realloc(m_TileTypes, (m_TileTypes_size + 1) * sizeof(STileTypeInfo)) : malloc(1 * sizeof(STileTypeInfo)));

		if (tmp)
		{
			++m_TileTypes_size;
			++m_TileTypes_sizeAllocated;
			m_TileTypes = tmp;
		}
	}

	//STileTypeInfo* ref = ParentId == -1 ? &m_Defaults_TileTypeInfo : &m_TileTypes[ParentId];
	//memcpy(&m_TileTypes[m_TileTypes_size - 1], &m_Defaults_TileTypeInfo, sizeof(STileTypeInfo));
	//memcpy(&m_TileTypes[m_TileTypes_size - 1], ParentId == -1 ? &m_Defaults_TileTypeInfo : &m_TileTypes[ParentId], sizeof(STileTypeInfo));
	memcpy(&m_TileTypes[m_TileTypes_size - 1], ParentId != -1 && ParentId < (m_TileTypes_size - 1) ? &m_TileTypes[ParentId] : &m_Defaults_TileTypeInfo, sizeof(STileTypeInfo));

	//printf("CreateTileTypeObj(%d)\n", m_TileTypes_size);
}

void CODTileInfo::PrintTileTypeObjects()
{
	for (int i = 0; i < m_TileTypes_size; ++i)
	{
		const STileTypeInfo& o = m_TileTypes[i];
		//printf(" %d. {Nummer: %d, Id: %d, Gfx: %d}\n", i, o.Nummer, o.Id, o.Gfx);
		printf(" %d. {Id: %d, Gfx: %d, [%d,%d],R:%d,An:%d,POffs:%d,hFlg:%d} // %s\n", i, o.Id, o.Gfx, o.SizeX, o.SizeY, o.Rotate, o.AnimAnz, o.Posoffs, o.HighFlg, o.Description);
	}
}

void CODTileInfo::SortById()
{
	STileTypeInfo* Old_TileTypes; // Array with TileTypeInfos
	int Old_TileTypes_size = m_TileTypes_size; // Size of Array with TileTypeInfos
	int Old_TileTypes_sizeAllocated = m_TileTypes_size; // TileTypeInfos-ArraySize that is preallocated in memory

	STileTypeInfo* New_TileTypes; // Array with TileTypeInfos
	int New_TileTypes_size = 0; // Size of Array with TileTypeInfos
	int New_TileTypes_sizeAllocated = m_TileTypes_size; // TileTypeInfos-ArraySize that is preallocated in memory

	STileTypeInfo* tmp0 = (STileTypeInfo*)(malloc(Old_TileTypes_sizeAllocated * sizeof(STileTypeInfo)));
	STileTypeInfo* tmp1 = (STileTypeInfo*)(malloc(New_TileTypes_sizeAllocated * sizeof(STileTypeInfo)));

	if (tmp0 && tmp1)
	{
		// Copy array to work array (which can be manipulated)
		memcpy(tmp0, m_TileTypes, m_TileTypes_size * sizeof(*tmp0));
		Old_TileTypes = tmp0;
		New_TileTypes = tmp1;

		// Sorting by "Id" parameter:
		for (int i = 0; i < m_TileTypes_size; ++i)
		{
			uint16_t LowestId = 0xffff; // uint16_t max value: 65535
			int LowestId_Idx = -1;

			for (int j = 0; j < Old_TileTypes_size; ++j)
			{
				if (LowestId > Old_TileTypes[j].Id || LowestId_Idx == -1)
				{
					LowestId = Old_TileTypes[j].Id;
					LowestId_Idx = j;
				}
			}

			if (LowestId_Idx != -1)
			{
				//New_TileTypes[i] = STileTypeInfo(Old_TileTypes[LowestId_Idx]);
				New_TileTypes[i] = Old_TileTypes[LowestId_Idx];

				// Shift array by 1 element:
				for (int j = LowestId_Idx; j < Old_TileTypes_size - 1; ++j)
				{
					Old_TileTypes[j] = Old_TileTypes[j + 1];
				}
				--Old_TileTypes_size;
				++New_TileTypes_size;
			}
			else
			{
				printf("Error CODTileInfo::SortById(). Aborting.\n");
				return;
			}
		}
		
		//char tmpc[] = "asda";
		//tmp1[0] = STileTypeInfo(1,1,1,1,1,1,1,tmpc);
		//tmp1[0] = STileTypeInfo(1,1,1,1,1,1,1);
		//tmp1[0] = Old_TileTypes[1];
		// Empty array:
		m_TileTypes = New_TileTypes;
		free(Old_TileTypes);
		//m_TileTypes_size = New_TileTypes_size + 1;
	}

}

bool CODTileInfo::ParseMultiValSize(const char* valueStr, int* X, int* Y)
{
	int len = (int)strlen(valueStr);
	int e1 = -1, b2 = -1;

	for (int i = 0; i < len; ++i)
	{
		char c = valueStr[i];

		if (b2 == -1)
		{
			if (isdigit(c))
			{
				e1 = i;
			}
			else if (c == ',')
			{
				b2 = -2; // Next state: Search for begin of 2nd value digit
			}
			else
			{
				printf("Multivalue 'Size' Syntax Error.\n");
				return false;
			}
		}
		else if (b2 == -2)
		{
			if (c == ' ')
			{
				// Ignore
			}
			else if (isdigit(c))
			{
				b2 = i;
			}
			else
			{
				printf("Multivalue 'Size' Syntax Error.\n");
				return false;
			}
		}
		else
		{
			if (isdigit(c))
			{
				// Ignore
			}
			else
			{
				printf("Multivalue 'Size' Syntax Error.\n");
				return false;
			}
		}
	}

	//if (e1 > b2) return;
	if (e1 > b2)
	{
		printf("x{%d,%d}", e1, b2);
		return false;
	}

	char constValue[11]; // int max len 10 + \0
	sprintf_s(constValue, 11, "%.*s", e1 + 1, valueStr);
	*X = atoi(constValue);
	sprintf_s(constValue, 11, "%.*s", len - b2, valueStr + b2);
	*Y = atoi(constValue);
	//printf("s[%d,%d]\n", *X, *Y);

	return true;
}
