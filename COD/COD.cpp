#include "COD.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <cassert>

COD::COD()
{
	//m_pubConstArr_names = (char**)malloc(1);
	//m_pubConstArr_names = (char**) malloc(1 * sizeof(*m_pubConstArr_names));
	//m_pubConstArr_values = (char**) malloc(1 * sizeof(*m_pubConstArr_names));

	//char** tmp = (char**)malloc(1 * sizeof(char*));
	//if (tmp)
	//{
	//	m_pubConstArr_names = tmp;
	//	m_pubConstArr_names[0] = (char*)"Name";
	//}
	//printf("v:%s\n", m_pubConstArr_names[0]);
}

COD::~COD()
{
	for (int i = 0; i < m_pubConstArr_size; i++) free(m_pubConstArr_names[i]);
	free(m_pubConstArr_names);
	free(m_pubConstArr_types);
	free(m_pubConstArr_values);

	for (int i = 0; i < m_Enum_size; i++) free(m_Enum_names[i]);
	free(m_Enum_names);

	free(m_pubConstArr_values);
}

bool COD::ExistsPubConst(const char* Name) const
{
	if (GetPubConstIdx(Name) != -1) return true;

	return false;
}

int COD::GetPubConstIdx(const char* Name) const
{
	for (int i = 0; i < m_pubConstArr_size; ++i)
	{
		if (strcmp(m_pubConstArr_names[i], Name) == 0)
		{
			return i;
		}
	}

	return -1;
}

int COD::GetPubConst(const char* Name) const
{
	int idx = GetPubConstIdx(Name);

	if (idx != -1)
	{
		return GetPubConstByIdx(idx);
	}

	return -1;
}

void COD::AddPubConst(const char* Name, int Value, int Type /*= 0*/)
{
	if (ExistsPubConst(Name)) return; // Do not allow double add //ToDo:Change Value!
	//printf("COD::AddPubConst(\"%s\", \"%s\");\n", Name, Value);

	char** tmp0 = (char**)(m_pubConstArr_size > 0 ? realloc(m_pubConstArr_names, (m_pubConstArr_size + 1) * sizeof(*m_pubConstArr_names)) : malloc(1 * sizeof(*m_pubConstArr_names)));
	int* tmp1 = (int*)(m_pubConstArr_size > 0 ? realloc(m_pubConstArr_types, (m_pubConstArr_size + 1) * sizeof(int)) : malloc(1 * sizeof(int)));
	int* tmp2 = (int*)(m_pubConstArr_size > 0 ? realloc(m_pubConstArr_values, (m_pubConstArr_size + 1) * sizeof(int)) : malloc(1 * sizeof(int))); 

	if (tmp0 && tmp1 && tmp2)
	{
		m_pubConstArr_names = tmp0;
		m_pubConstArr_types = tmp1;
		m_pubConstArr_values = tmp2;
		m_pubConstArr_names[m_pubConstArr_size] = (char*)Name;
		m_pubConstArr_types[m_pubConstArr_size] = Type;
		m_pubConstArr_values[m_pubConstArr_size] = Value;
		++m_pubConstArr_size;
	}
}

void COD::SetPubConst(const char* Name, int Value, int Type /*= 0*/)
{
	int idx = GetPubConstIdx(Name);

	if (idx != -1)
	{
		if (m_pubConstArr_types[idx] == Type)
		{
			m_pubConstArr_values[idx] = Value;
		}
		else
		{
			printf("Error: Setting COD::Constant with different type. This is not allowed!\n");
			return;
		}
	}
	else
	{
		AddPubConst(Name, Value, Type);
	}
}

bool COD::ExistsPubVar(const char* Name) const
{
	//if (!m_pubVarArr_names) return false;

	return false;
}

bool COD::GetPubVar(const char* Name, char& Value) const
{
	//if (!m_pubVarArr_names) return false;

	return false;
}

void COD::AddPubVar(const char* Name, const char* Value)
{

}

bool COD::ExistsEnumName(const char* Name) const
{
	if (GetEnumIdx(Name) != -1) return true;

	return false;
}

int COD::GetEnumIdx(const char* Name) const
{
	for (int i = 0; i < m_Enum_size; ++i)
	{
		if (strcmp(m_Enum_names[i], Name) == 0)
		{
			return i;
		}
	}

	return -1;
}

void COD::SetEnum(const char* Name)
{
	if (ExistsEnumName(Name)) return; // Do not allow double add //ToDo:Change Value!
	//printf("COD::AddPubConst(\"%s\", \"%s\");\n", Name, Value);

	char** tmp = (char**)(m_Enum_size > 0 ? realloc(m_Enum_names, (m_Enum_size + 1) * sizeof(*m_Enum_names)) : malloc(1 * sizeof(*m_Enum_names)));

	if (tmp)
	{
		m_Enum_names = tmp;
		m_Enum_names[m_Enum_size] = (char*)Name;
		++m_Enum_size;
	}
}
