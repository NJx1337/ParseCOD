#include <stdio.h>
#include <iostream>     // std::cout, std::ostream, std::ios
#include <fstream>
#include "COD/CODFile.h"
#include "COD/COD.h"
#include "COD/CODParser.h"
#include "COD/CODTileInfo.h"
//#include "addon/TileMappingOld.h"

int main(int argc, char** argv)
{
	//ToDo:Move ReadCOD project to DecCOD. Move ParseCOD(this) to DecCOD.

	//if (argc != 2)
	//	exit(EXIT_FAILURE);

	//COD* cod1 = new COD("Text.cod");
	//cod1->writeDecCOD("Text.txt");
	//COD* cod2 = new COD("figuren.cod");
	//cod2->writeDecCOD("figuren.txt");
	//CODFile* cod3 = new CODFile("haeuser.cod");
	//cod3->writeDecCOD("haeuser.txt");
	//COD* cod4 = new COD("Editor.cod");
	//cod4->writeDecCOD("Editor.txt");

	//CODFile* cf = new CODFile("haeuser_test_class3_hausonly.txt", CODFile::EInputFileType::IFT_TXT);
	//CODFile* cf = new CODFile("haeuser_cache0.cache.txt", CODFile::EInputFileType::IFT_HCACHE);
	//CODParser* codParse = new CODParser(cf);
	//CODParser* codParse = new CODParser("haeuser.cod");
	//CODParser* codParse = new CODParser(new CODFile("haeuser_test.txt", true));
	//CODParser* codParse = new CODParser(new CODFile("haeuser_test_class2.txt", true));
	//CODParser* codParse = new CODParser(new CODFile("haeuser_test_class3_hausonly.txt", true));
	CODParser* codParse = new CODParser(new CODFile("haeuser_test_cache0.cache.txt", CODFile::EInputFileType::IFT_HCACHE));
	COD* haeuserConf = codParse->GetCOD();
	CODTileInfo* tileInfo = codParse->GetTileInfo();
	//tileInfo->SortById();
	//CODFile::writeHaeuserCacheFromCOD("haeuser_test_cache1.cache.txt", tileInfo);
	//tileInfo->PrintTileTypeObjects();
	//TileMappingOld tilemap("tilemapping.txt");
	//tilemap.PrintDifference(tileInfo);

	return 0;
	haeuserConf->SetPubConst("ENUM1", 2);
	haeuserConf->SetPubConst("ENUM2a", 123);
	printf("exists: %d\n", haeuserConf->ExistsPubConst("ENUM1"));
	printf("exists: %d\n", haeuserConf->ExistsPubConst("test2"));
	printf("exists: %d\n", haeuserConf->ExistsPubConst("ENUM2a"));
	int idx01 = haeuserConf->GetPubConstIdx("ENUM1");
	if (idx01 > -1) { printf("idx1 Name: %s, Value: %d\n", haeuserConf->GetPubConstName(idx01), haeuserConf->GetPubConstByIdx(idx01)); }
	int idx02 = haeuserConf->GetPubConstIdx("test2");
	if (idx02 > -1) { printf("idx2 Name: %s, Value: %d\n", haeuserConf->GetPubConstName(idx02), haeuserConf->GetPubConst(haeuserConf->GetPubConstName(idx02))); }
	int idx03 = haeuserConf->GetPubConstIdx("ENUM2a");
	if (idx03 > -1) { printf("idx3 Name: %s, Value: %d\n", haeuserConf->GetPubConstName(idx03), haeuserConf->GetPubConst(haeuserConf->GetPubConstName(idx03))); }

	return 0;

	haeuserConf->SetEnum("ENUM1");
	haeuserConf->SetEnum("ENUM2a");
	printf("exists: %d\n", haeuserConf->ExistsEnumName("ENUM1"));
	printf("exists: %d\n", haeuserConf->ExistsEnumName("test2"));
	printf("exists: %d\n", haeuserConf->ExistsEnumName("ENUM2a"));
	int idx1 = haeuserConf->GetEnumIdx("ENUM1");
	if (idx1 > -1) { printf("idx1 Name: %s\n", haeuserConf->GetEnumName(idx1)); }
	int idx2 = haeuserConf->GetEnumIdx("ENUM2");
	if (idx2 > -1) { printf("idx2 Name: %s\n", haeuserConf->GetEnumName(idx2)); }
	int idx3 = haeuserConf->GetEnumIdx("ENUM2a");
	if (idx3 > -1) { printf("idx3 Name: %s\n", haeuserConf->GetEnumName(idx3)); }

	return 0;

	printf("AddPubConst()\n");
	printf("exists: %d\n", haeuserConf->ExistsPubConst("test"));
	haeuserConf->AddPubConst("test", 5);
	haeuserConf->AddPubConst("test2", 3);
	printf("exists: %d\n", haeuserConf->ExistsPubConst("test"));
	printf("exists: %d\n", haeuserConf->ExistsPubConst("test2"));
	printf("exists: %d\n", haeuserConf->ExistsPubConst("test1"));

	//int val0;
	//printf((haeuserConf->GetPubConst("test", &val0) ? "value test: %d\n" : "value test: <none>//\n"), val0);
	//printf((haeuserConf->GetPubConst("test1", &val0) ? "value test1: %d\n" : "value test1: /<none>/\n"), val0);
	//printf((haeuserConf->GetPubConst("test2", &val0) ? "value test2: %d\n" : "value test2: <none>\n"), val0);
	printf("end\n");

	return 0;

	char** array1;
	char** array;
	array = (char**)malloc(1 * sizeof(*array));
	//m_pubConstArr_names = array;

	if (array)
	{
		//array[0] = "This";
		//memcpy(array[0], "This", 4);
		//strncpy(array[0], "This", 5);
		array[0] = (char*)"This";

		printf("%s\n------\n", array[0]);
		printf("size:%i\n", (int)sizeof(array));
		printf("size[0]:%i\n", (int)sizeof(array[0]));

		char** tmp = (char**)realloc(array, 2 * sizeof(*array));
		if (tmp)
		{
			array = tmp;
			array[1] = (char*)"That";

			printf("%s\n", array[0]);
			printf("%s\n", array[1]);
		}

		array1 = array;
		printf("%s\n", array1[0]);
		printf("%s\n", array1[1]);



		free(array);
	}
	return 0;



	char** names;
	names = (char**)malloc(1 * sizeof(*names));

	const char* inp = "hi";
	char* str = (char*)malloc((sizeof(inp) + 1) * sizeof(char));
	//str = (char*)inp;
	//names[1] = str;
	//memcpy(names[1], str, sizeof(inp) + 1);
	strncpy_s(names[1],  sizeof(inp) + 1, str, sizeof(inp) + 1);
	//memcpy(array[0], "This", 4);
	//strncpy(array[0], "This", 5);
	printf("1\n");

	char** tmp = (char**)realloc(names, 2 * sizeof(*names));
	if (tmp)
	{
		printf("2\n");
		names = tmp;
		//names[1] = (char*)"That";

		printf("3\n");
		printf("%s\n", names[0]);
		printf("%s\n", names[1]);
		printf("4\n");
	}
	printf("5\n");

	free(names);

	return 0;
}
