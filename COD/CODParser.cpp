#include "CODParser.h"
#include "CODFile.h"
#include "CODTileInfo.h"
#include "COD.h"
#include "CODIntConsts.h"
#include <stdio.h>
#include <cstring>
#include <ctype.h>
#include <stdlib.h>
#include <chrono>

CODParser::CODParser(const char* CodFileName)
{
	CODFile* CodFile = new CODFile(CodFileName);
	ParseCODFile(CodFile);
}

CODParser::CODParser(CODFile* CodFile)
{
	ParseCODFile(CodFile);
}

CODParser::~CODParser()
{
	for (int i = 0; i < m_ObjectStack_size; i++) free(m_ObjectStack[i]);
	free(m_ObjectStack);
}

void CODParser::ParseCODFile(CODFile* CodFile)
{
	int limitLine = 10; // temporary variable to stop parsing at this line
	//int V_DEBUGLINE = 66;
	//int V_DEBUGLINE = 22;
	//int V_DEBUGLINE = 740;
	int V_DEBUGLINE = -1; // Deactivate
	int linepos = 0, charpos = 0; // Current processing line and character position

	m_CODTileInfo = new CODTileInfo();

	m_COD = new COD();
	char* rawData = CodFile->GetRawDecCOD();
	//int rawSize = strlen(rawData);
	int rawSize = 0;
	const int rawSizeSingleChar = sizeof(rawData[0]);

	for (; *(rawData + rawSize) != '\0'; ++rawSize);
	rawSize *= rawSizeSingleChar; // in case char is not 1 byte

	printf("CODFile size: %i\n", rawSize);
	printf("--------------------------------------------------\n");

	AddIntConsts();

	double duration0 = 0;
	auto start = std::chrono::high_resolution_clock::now();
	int i = 0;
	int offset = 0;
	int firstcharpos = -1, lastcharpos = -1; // Store offsets of first and last char position in a line (to skip whitespaces)
	do
	{
		//if (i < rawSize - 128796) // Check if is not last char
		if (i < rawSize) // Check if is not last char
		{
			char c = *(rawData + i);
			char c_1 = *(rawData + i + 1);
			if (c == '\n' || c == '\r') // New Line detected (\r or \n)
			{
				if (linepos == V_DEBUGLINE)
				{
					printf("\\n\n");
					printf(" f:%d, l:%d\n", firstcharpos, lastcharpos);
				}

				// process line from offset to charpos - 1
				char line_buffer[1024];//ToDo:Replace static allocated string
				//char* line_buffer = (char*)malloc((linelen + 1) * sizeof(char));

				bool isEmpty = false;
				if (firstcharpos == -1 && lastcharpos == -1) // Line only countains empty spaces
				{
					isEmpty = true;
				}

				firstcharpos = firstcharpos == -1 ? 0 : firstcharpos;
				lastcharpos = lastcharpos == -1 ? charpos : lastcharpos;
				//lastcharpos -= lastcharpos > 0 ? 1 : 0;
				//lastcharpos -= (rawData + offset + firstcharpos)[lastcharpos-1] == '\n' ? 1 : 0;
				//lastcharpos -= (rawData + offset + firstcharpos)[lastcharpos-1] == '\r' ? 1 : 0;

				if (linepos == V_DEBUGLINE)
				{
					printf("-f:%d, l:%d\n", firstcharpos, lastcharpos);
				}

				if (lastcharpos > -1)
				{
					strncpy_s(line_buffer, rawData + offset + firstcharpos, lastcharpos - firstcharpos + 1);
				}
				else
				{
					line_buffer[0] = '\0';
				}

				if (linepos == V_DEBUGLINE)
				{
					//printf("c:%c\nc", (rawData + offset + firstcharpos)[lastcharpos]);
					printf("c/");
					int cpos = 0;
					do
					{
						char c = line_buffer[cpos];
						if (c == '\0') break;
						printf("%c.", c == '\t' ? '~' : c);
					} while (++cpos);
					printf("/c_end\n");
				}

				if (strlen(line_buffer) > 0 && !isEmpty) // Only process line, if its not empty
				{
					int res = ParseCODFileLine(line_buffer, linepos, firstcharpos);

					if (res == -1)
					{
						//printf("Abort;\n");
						break;
					}
					//printf("%d\n", linepos + 1);
				}

				if (linepos < limitLine)
				{
					//printf("ln:%d %d\n", firstcharpos, lastcharpos);
					//printf("%d:%s [%d:%d] (%d)\n", linepos + 1, line_buffer, firstcharpos, lastcharpos, (int)strlen(line_buffer));
				}

				offset = i + 1; // Char after new line char is start of next line
				++linepos;
				charpos = 0;
				firstcharpos = -1, lastcharpos = -1;

				if (c == '\r' && c_1 == '\n') // New Line detected (\r\n combination)
				{
					printf("\\r\\n\n");
					++offset; // \r\n combination shifts next char by one.
					++i;
				}
			}
			else
			{
				if (firstcharpos == -1)
				{
					if (c != ' ' && c != '\t')
					{
						firstcharpos = charpos;
					}

					if (linepos == V_DEBUGLINE) {
						printf("%c.", c == '\t' ? '~': c);
					}
				}
				else
				{
					//if (!isspace(c)) // isspace has a problem with german special chars: ae, ue, ...
					if (c != ' ' && c != '\t')
					{
						lastcharpos = charpos;
					}

					if (linepos == V_DEBUGLINE) {
						printf("%c,", c == '\t' ? '~': c);
					}
				}
				++charpos;
			}
		}
		else
		{
			printf("CODRaw EOL reached\n");
			printf("Lines: %i\n", linepos);
			break;
		}
	} while (++i);

	auto s0_1 = std::chrono::high_resolution_clock::now();
	duration0 += (double)std::chrono::duration_cast<std::chrono::milliseconds>(s0_1 - start).count() / 1000;
	//printf("Reading COD finished in %.2lfs! {d0:%.2lfs, d1:%.2lfs, d2:%.2lfs, d3:%.2lfs, dx1:%.2lfs}\n", (double)duration.count() / 1000, duration0_0 + duration0_1, duration1, duration2, duration3, durationx1);
	printf("Reading COD finished in %.2lfs!\n", duration0);
}

int CODParser::ParseCODFileLine(const char* line, int linepos, int startcharoffset)
{
	//int limitLine = 10; // GFXBURG Const
	//int limitLine = 66; // GFXBURG Const
	//int limitLine = 92; // Nahrung var
	//int limitLine = 152; // Last Const before 1.Object
	int limitLine = 999152; // Last Const before 1.Object
	int result = 0; // 0 = Normal operation, -1 = Error
	if (linepos > limitLine)
	{
		result = -1;
		return result;
		//printf("ln:%d %d\n", firstcharpos, lastcharpos);
		//printf("%d:%s [%d:%d] (%d)\n", linepos + 1, line_buffer, firstcharpos, lastcharpos, (int)strlen(line_buffer));
	}
	//printf("linepos:%d ", linepos);

	//size_t linelen = strlen(line);
	int linelen = (int) strlen(line);

	enum EState {
		INITIAL, // Beginning state for line. Look for word, comment, etc
		WORD, // Found character, continue finding word that is unknown type
		FINWORD, // Found first word, try to get type by syntax
		CONSTANT1, // Found =, Try to find space
		CONSTANT2, // Found space. Try to find more spaces or value
		CONSTANT3, // Found alphanum. Try to finish value
		PARAM1, // Found :, Try to find space
		PARAM2, // Found space, Try to find value
		PARAM3, // Found alphanum, Try to finish value
		CMD, // Found word and ; Assuming its a single cmd like EndObj;
		FIN, // FIN state, no more char processing of current line
		SYNTAXERR, // Found :, Try to find value
		PARSEERROR // Left parse error
	};

	int state = EState::INITIAL;
	//int b1,e1,b2,e2; // begins and ends of left and right of an expression
	int b1,e1,b2; // begins and ends of left and right of an expression
	// Get word by word:
	for (int i = 0; i < linelen; ++i)
	{
		char c = line[i];
		switch (state)
		{
		case EState::INITIAL:
			if (c == ';') // Comment found
			{
				b1 = i + 1;
				if (b1 > linelen) return -1;

				char* rawCmnt = (char*)malloc((linelen - b1 + 1) * sizeof(char));
				sprintf_s(rawCmnt, linelen - b1 + 1, "%.*s", (int)linelen - b1, line + b1);

				ProcessLastComment(rawCmnt);

				//printf(";%s\n",rawCmnt);
				//printf(";%s\n", line + i + 1);
				return result;
			}
			else if (isalpha(c) || c == '@') // Word can only start with characters
			{
				b1 = i;
				state = EState::WORD;
			}
			else if (isspace(c)) // Found space
			{
				printf("space.%s",line);
				// Continue searching start of word
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				printf("SyntaxError: %s\n", line + i);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::WORD:
			if (isalnum(c) || c == '_') // Word can include alphanumerics
			{
				// Continue searching end of word
			}
			else if (c == ':') // Found :, must be parameter
			{
				e1 = i;
				state = EState::PARAM1;
			}
			else if (isspace(c)) // Found space, word is finished, continue seraching syntax
			{
				e1 = i;
				state = EState::FINWORD;
			}
			else if (c == ';' && line[b1] != '@') // Found ;, must be cmd
			{
				e1 = i;

				char* name = (char*)malloc((e1 - b1 + 1) * sizeof(char));
				sprintf_s(name, e1 - b1 + 1, "%.*s", (int)e1 - b1, line + b1);

				if (strcmp(name, "EndObj") == 0)
				{
					//DEBUG:
					const char* lastobjname = GetLastObjFromStack();
					if (strcmp(lastobjname, "HAUS_PRODTYP") != 0 && strcmp(lastobjname, "HAUS_BAUKOST") != 0)
						printf("%d. C1: EndObj {%s} [%s] (%s)\n", linepos, lastobjname, DebugReturnMainObjName(), DebugReturnObjNames());

					if (!RemoveLastObjFromStack())
					{
						printf("Error: EndObj; failed. There was no more object on the stack. COD file is invalid.\n");
					}
					else
					{
						break;
					}
				}
				else
				{
					//ToDo: Check left
					printf("%d. Cmd: %s\n", linepos, name);
					//printf("%d. P: %.*s = %.*s\n", linepos, (int)e1 - b1, line + b1, (int)linelen - b2, line + b2);
					//	//printf("%.*s", 4, buff + 10);
				}
				state = EState::FIN;

			}
			else
			{
				// SYNTAX ERROR: Expression not finished. (Only one word found) 
				printf("SyntaxError: %s\n", line + i);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::FINWORD:
			if (c == '=') // Found =, must be constant
			{
				state = EState::CONSTANT1;
			}
			else if (isspace(c)) // Found space, word is finished, continue seraching syntax
			{
				// Continue searching for syntax
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. (Only one word found) 
				printf("SyntaxError: %s\n", line + i);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::CONSTANT1:
			if (isspace(c)) // Found space, constant-expression is finished, continue searching value
			{
				state = EState::CONSTANT2;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				printf("SyntaxError: %s\n", line + i);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::CONSTANT2:
			if (isalnum(c) || c == '+' || c == '-') // Found Alphanumeric, value starts here
			{
				b2 = i;
				state = EState::CONSTANT3;

				char* name = (char*)malloc((e1 - b1 + 1) * sizeof(char));
				sprintf_s(name, e1 - b1 + 1, "%.*s", (int)e1 - b1, line + b1);

				// Get last relevant character of line for parsing:
				int e2 = GetLastRelevantChar(line, b2, linelen);

				//Check left:
				char* left = (char*)malloc((linelen - b2 + 1) * sizeof(char));
				//sprintf_s(left, linelen - b2 + 1, "%.*s", (int)linelen - b2, line + b2);
				sprintf_s(left, e2 - b2 + 1, "%.*s", (int)e2 - b2, line + b2);

				//if (linepos == 29)
				//{
				//	printf(">>31. [%d,%d,(%d)] \"%s\"\n", (int)b2, e2, e2 - b2 + 1, left);
				//}

				//printf("%d. C: %s = ", linepos, name);
				//printf("%d. C: %.*s = ", linepos, (int)e1 - b1, line + b1);
				//printf("%d. C: %.*s = %s\n", linepos, (int)e1 - b1, line + b1, left);
				char* valStr;
				int valInt;
				float valFloat;
				char* valComment;
				int valType = ParseLeft(left, name, linepos, startcharoffset + b2, &valStr, &valInt, &valFloat, &valComment);
				
				if (valType == 1 || (name[0] == '@' && valType == 0 && valInt == 0)) // inc/dec Integer value
				{
					if (name[0] != '@')
					{
						printf("Error on line %d: Constant value can not be a inc/dec number \"%s\".\n", linepos, valStr);
						result = -1;
						break;
					}
					else
					{
						// PROCESS CONSTANT => Get value
						//char* constName = (char*)malloc((e1 - b1 + 2) * sizeof(char));
						//sprintf_s(constName, e1 - b1 + 2, "%.*s", (int)e1 - b1 + 1, left + b1);

						int constIdx = m_COD->GetPubConstIdx(name + 1);
						if (constIdx == -1)
						{
							printf("Error: Constant %s not found! Make sure to set it in program or .cod file before trying to inc/dec it on line %d.\n", name + 1, linepos);
							result = -1;
							break;
						}
						int constValueInt = m_COD->GetPubConstByIdx(constIdx);
						int newValueInt = constValueInt + valInt;
						char constValue[11]; // int max len 10 + \0
						sprintf_s(constValue, 11, "%d", newValueInt);

						//if (strcmp(name, "@Nummer") == 0 || strcmp(name, "Nummer"))
						//if (strcmp(name, "Nummer"))
						if (strcmp(name, "Nummer") == 0) //ToDo:Check if this works, is needed and is valid
						{
							//if (strcmp(name, "@Nummer") == 0)
							//{
							//	//Get
							//	int val = m_COD->GetPubConst("Nummer");
							//
							//	if (val != -1)
							//	{
							//		valInt += val;
							//	}
							//}

							//m_COD->SetPubConst("Nummer", valInt); // Set Const, can override
							//printf("  >>>Lastid: %d -> %d\n", m_InClass_LastId, newValueInt);
							m_InClass_LastId = newValueInt;
						}
						//if (strcmp(name, "@GFXNR") == 0)
						//{
						//	printf("C: %s = %d\n", name, newValueInt);
						//}

						m_COD->SetPubConst(name + 1, newValueInt); // Set Const, can override existing
						///printf("%d.@C: %s = %s (%d)\n", linepos, name+1, valStr, m_COD->GetPubConst(name + 1));
					}
				}
				else if (valType == 0) // Integer value
				{
					//if (strcmp(name, "GFXNR") == 0)
					//{
					//	printf("C1: %s = %d\n", name, valInt);
					//}
					m_COD->SetPubConst(name, valInt); // Set Const, can override existing
					///printf("%d.C: %s = %d\n", linepos, name, m_COD->GetPubConst(name));
				}
				else if (valType > 1) // Float or Multivalue
				{
					if (valType == 2)
					{
						//printf("Error on line %d: Constant value can not be a float \"%f\".\n", linepos, valFloat);
						printf("Error on line %d: Constant value can not be a float \"%s\".\n", linepos, valStr);
					}
					else if (valType == 3)
					{
						printf("Error on line %d: Constant value can not be a multi-value \"%s\".\n", linepos, valStr);
					}
					result = -1;
					printf("Error parsing line %d. Aborting.\n", linepos);
					break;
				}
				else if (valType == -1)
				{
					printf("Error parsing line %d. Aborting.\n", linepos);
					state = EState::PARSEERROR;
					break;
				}
				//ParseLeft(line, linepos, startcharoffset + b2);
				//ParseLeft(line, linepos, startcharoffset + b2, &*vals , &valsSize);
				//printf("%d. C: %.*s = %.*s\n", linepos, (int)e1 - b1, line + b1, (int)linelen - b2, line + b2);
				//printf("%d. C: %.*s = %s\n", linepos, (int)e1 - b1, line + b1, left);
				//printf("%d. C: %.*s = ", linepos, (int)e1 - b1, line + b1);
				//	//printf("%.*s", 4, buff + 10);

				return result;
			}
			else if (isspace(c)) // Found space
			{
				// Continue searching for begin of value
			}
			else
			{
				// SYNTAX ERROR: Expression not finished.
				printf("SyntaxError: ;%s\n", line + i);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::PARAM1:
			if (isspace(c))// Found space, param-expression is finished, continue searching value
			{
				state = EState::PARAM2;
				// Continue searching for begin of value
			}
			else
			{
				// SYNTAX ERROR: Expression not finished.
				printf("SyntaxError1: ;%s\n", line + i);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::PARAM2:
			if (isalnum(c) || (line[b1] == '@' && (c == '+' || c == '-'))) // Found Alphanumeric, value starts here
			{
				b2 = i;
				state = EState::PARAM3;

				char* name = (char*)malloc((e1 - b1 + 1) * sizeof(char));
				sprintf_s(name, e1 - b1 + 1, "%.*s", (int)e1 - b1, line + b1);

				// Get last relevant character of line for parsing:
				int e2 = GetLastRelevantChar(line, b2, linelen);

				char* left = (char*)malloc((linelen - b2 + 1) * sizeof(char));
				//sprintf_s(left, linelen - b2 + 1, "%.*s", (int)linelen - b2, line + b2);
				sprintf_s(left, e2 - b2 + 1, "%.*s", (int)e2 - b2, line + b2);

				//if (linepos == 150)
				//{
				//	printf(">>150. [%d,%d,(%d)] \"%s\"\n", (int)b2, e2, e2 - b2 + 1, left);
				//}

				if (strcmp(name, "Objekt") == 0)
				{
					const char* name2 = ParseLeftName(left, linepos, startcharoffset + b2);
					AddObjToStack(name2);

					if (strcmp(name2, "HAUS_PRODTYP") != 0 && strcmp(name2, "HAUS_BAUKOST") != 0)
						printf("%d. C0: BeginObj \"%s\" [%s] (%s)\n", linepos, name2, DebugReturnMainObjName(), DebugReturnObjNames());
				}
				else
				{
					char* valStr;
					int valInt;
					float valFloat;
					char* valComment;

					int valType = ParseLeft(left, name, linepos, startcharoffset + b2, &valStr, &valInt, &valFloat, &valComment);

					//if (linepos == 740)
					//{
					//	printf(">>>741.%s = %s (%d)\n", left, valStr, valInt);
					//}

					if (valType == 1 || (name[0] == '@' && valType == 0 && valInt == 0)) // inc/dec Integer value
					{
						if (name[0] != '@')
						{
							printf("Error on line %d: Param value can not be a inc/dec number \"%s\".\n", linepos, valStr);
							result = -1;
							break;
						}
						//if (strcmp(name, "@Nummer") == 0 || strcmp(name, "@Id") == 0 || strcmp//(name, "@Gfx") == 0)
						//	printf(" . x: %s - %d\n", valStr, valInt);
					}
					else if (valType == -1)
					{
						printf("Error parsing line %d. Aborting (p).\n", linepos);
						state = EState::PARSEERROR;
						break;
					}

					if (strcmp(name, "@Nummer") == 0 || strcmp(name, "Nummer") == 0)
					{
						if (strcmp(name, "@Nummer") == 0)
						{
							//Get
							int val = m_COD->GetPubConst("Nummer");

							if (val != -1)
							{
								valInt += val;
							}
						}

						m_COD->SetPubConst("Nummer", valInt); // Set Const, can override
						//printf("  >>>2Lastid: %d -> %d\n", m_InClass_LastId, valInt);
						m_InClass_LastId = valInt;
					}
					//valComment[0] = '\0';

					//if (strcmp(name, "Nummer") == 0 || strcmp(name, "Id") == 0 || strcmp(name, "Gfx") == 0)
					//	printf("   x: %s - %d\n", valStr, valInt);

					//if (linepos == 740)
					//{
					//	printf(">o>>741.%s = %s (%d) [t:%d] {}\n", name, valStr, valInt, valType);
					//}

					ProcessObjectData(linepos, valType, name, valStr, valInt, valFloat, valComment);

					///printf("%d. P: %s = %s\n", linepos, name, left);
					//printf("%d. P: %.*s = %.*s\n", linepos, (int)e1 - b1, line + b1, (int)linelen - b2, line + b2);
					//	//printf("%.*s", 4, buff + 10);
					//ProcessObjectData()
				}

				return result;
			}
			else if (isspace(c)) // Found space
			{
				// Continue searching for begin of value
			}
			else
			{
				// SYNTAX ERROR: Expression not finished.
				printf("SyntaxError2: ;%s\n", line + i);
				state = EState::SYNTAXERR;
			}

			break;
		}

		//if (line[i] == ';') // Check if comment
		//{
		//	//printf("%.*s", 4, buff + 10);
		//	printf(";%s\n", line + i + 1);
		//	break;
		//}
		//else
		//{
		//	printf(".");
		//}
	}

	if (state == EState::SYNTAXERR || state == EState::PARSEERROR)
	{
		result = -1;
		return result;
	}

	//ToDo: Return type: SetConstant, SetParam, Classbegin, ClassEnd,...

	return result;
}

int CODParser::GetLastRelevantChar(const char* line, int start, size_t linelen) const
{
	//int e = (int) linelen;
	int e = start - 1;
	//printf("'%c' <%d - %d>\n", line[start], start, (int) linelen);
	//return (int) linelen;

	for (int i = start; i < (int) linelen; ++i)
	{
		char c = line[i];

		if (c == ';') // Found comment start, end searching
		{
			break;
		}
		else if (c == ' ' || c == '\t')
		{
			// Ignore this char, do nothing
		}
		else
		{
			e = i;
		}
	}

	//printf("'%c' <%d - %d>\n", line[e], start, e);
	return e + 1;
}

int CODParser::ParseLeft(const char* left, const char* name, int linepos, int startcharoffset, char** valueStr, int* valueInt, float* valueFloat, char** valueComment)
{
	if (!m_COD) return -1; // m_COD must be created!

	enum EState {
		INITIAL, // Beginning state for line. Look for word, comment, etc
		CONSTANT1, // Found =, Try to find space
		CONSTANT2, // Found space. Try to find more spaces or value
		CONSTANT3, // Found alphanum. Try to finish value
		NUM1, // Found :, Try to find space
		NUM2, // Found :, Try to find space
		INCDEC, // Found +/-, Try to find number
		FIN, // No more searching
		SYNTAXERR // Found :, Try to find value
	};

	//size_t linelen = strlen(left);
	int linelen = (int) strlen(left);

	//ToDo:add reloc to part results
	char* result = (char*)malloc((linelen + 1) * sizeof(char)); // Set return value on max len of line

	if (!result)
	{
		printf("Mem alloc err: CODParser::ParseLeft().result\n");
		return -1;
	}

	int state = EState::INITIAL;
	//int b1,e1,b2,e2; // begins and ends of left and right of an expression
	int b1, e1, b2; // begins and ends of left of an expression
	int valType = -1; // -1 = syntax error, 0 = int, 1 = inc/dec int, 2 = float, 3 = string(multi-value)
	int valOperator = -1; // 0 = +, 1 = -
	// Get word by word:
	int valuecnt = 0;
	for (int i = 0; i <= linelen; ++i)
	{
		char c = left[i];

		if (state == EState::FIN) break;

		switch (state)
		{
		case EState::INITIAL:
			if (c == '+' || c == '-') // Incr/Decr number value
			{
				b1 = i;
				state = EState::NUM1;
			}
			else if (isalpha(c)) // Constant can only start with characters
			{
				b1 = i;
				state = EState::CONSTANT1;
			}
			else if (isdigit(c)) // Number value
			{
				b1 = i;
				state = EState::NUM1;
			}
			else if (isspace(c) && valuecnt > 0) // Ignore Space (only if already processed 1 value)e
			{
				// Continue searching for syntax
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError1a line %d: %s\n", linepos, left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::CONSTANT1:
			if (isalnum(c) || c == '_') // Word can include alphanumerics
			{
				// Continue searching end of word
				e1 = i;
			}
			else if (c == ',' || c == '\0') // Found ',', Const finished
			{
				// PROCESS CONSTANT => Get value
				char* constName = (char*)malloc((e1 - b1 + 2) * sizeof(char));
				sprintf_s(constName, e1 - b1 + 2, "%.*s", (int)e1 - b1 + 1, left + b1);

				int constIdx = m_COD->GetPubConstIdx(constName);
				if (constIdx == -1)
				{
					printf("Error: Constant %s not found! Make sure to set it in program or .cod file.\n", constName);
					state = EState::SYNTAXERR;
					return -1;
				}
				int constValueInt = m_COD->GetPubConstByIdx(constIdx);
				char constValue[11]; // int max len 10 + \0
				sprintf_s(constValue, 11, "%d", constValueInt);

				if (valuecnt > 0 || c == ',')
				{
					if (valuecnt > 0) // Already found min 1 value before
						sprintf_s(result, linelen + 1, "%s, %s", result, constValue);
					else
						sprintf_s(result, linelen + 1, "%s", constValue);

					//printf("|||%d. C: %s = S1m:%s\n", linepos, name, result);
					valType = 3; // Multivalue in a string

					++valuecnt;
				}
				else
				{
					valType = 0; // Expected int-value
					*valueInt = constValueInt;

					//char* strInt = (char*)malloc((strlen(constValue) + 1) * sizeof(char));
					//sprintf_s(left + b1, e1 - b1 + 1, "%.*s", (int)i - b1, left + b1);
					//sprintf_s(strInt, strlen(constValue) + 1, "%d", constValueInt);
					//printf("    y: %s - %d\n", strInt, constValueInt);
					*valueStr = constValue;
					//*valueStr = strInt;
				}

				state = EState::INITIAL;
			}
			else if (c == '+' || c == '-') // Constant with add/sub num
			{
				valType = 1; // expected inc/dec int value
				*valueInt = 0;
				valOperator = c == '+' ? 0 : 1; // + or -

				// PROCESS CONSTANT => Get value
				//valueInt = ;
				state = EState::CONSTANT2;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::CONSTANT2:
			if (isdigit(c))
			{
				// Continue searching end of number
				b2 = i;
				state = EState::CONSTANT3; // Number has atleast 1 digit => is valid
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::CONSTANT3:
			if (isdigit(c))
			{
				// Continue searching end of number
			}
			else if (c == '\0') // Found ',', Const finished
			{
				// PROCESS CONSTANT => Get value

				char* constName = (char*)malloc((e1 - b1 + 2) * sizeof(char));
				sprintf_s(constName, e1 - b1 + 2, "%.*s", (int)e1 - b1 + 1, left + b1);

				int constIdx = m_COD->GetPubConstIdx(constName);
				if (constIdx == -1)
				{
					printf("Error: Constant %s not found! Make sure to set it in program or .cod file.\n", constName);
					state = EState::SYNTAXERR;
					return -1;
				}
				*valueInt += m_COD->GetPubConstByIdx(constIdx);

				char* strInt = (char*)malloc((i - b2 + 1) * sizeof(char));
				//sprintf_s(left + b1, e1 - b1 + 1, "%.*s", (int)i - b1, left + b1);
				sprintf_s(strInt, i - b2 + 1, "%.*s", (int)i - b2, left + b2);
				int valInt2 = atoi(strInt);
				*valueInt = valOperator == 0 ? *valueInt + valInt2 : *valueInt - valInt2;
				//*valueStr[0] = '/0';

				char constValue[11]; // int max len 10 + \0
				sprintf_s(constValue, 11, "%d", *valueInt);
				*valueStr = constValue;

				//if (linepos == 740)
				//{
				//	printf(">>1>741.%s = %s (%d)\n", left, *valueStr, *valueInt);
				//}

				//int len = (int) floor(log10(abs(*valueInt))) + 1;
				//len = abs(*valueInt) < 1 ? 1 : len;

				//char constValue[11]; // int max len 10 + \0
				//sprintf_s(constValue, 11, "%d", *valueInt);
				//printf("      >>> %s - %d\n", constValue, *valueInt);
				//printf("      >>> %d - %d\n", *valueInt, len);
				//*valueStr = constValue;

				//char* strInt1 = (char*)malloc((len + 1) * sizeof(char));
				//sprintf_s(left + b1, e1 - b1 + 1, "%.*s", (int)i - b1, left + b1);
				//sprintf_s(strInt1, len + 1, "%d", *valueInt);
				//printf("    y: %s - %d\n", strInt1, *valueInt);
				//*valueStr = constValue;
				//*valueStr = strInt1;

				//m_COD->SetPubConst(name, *valueInt); // Set Const, can override existing
				valType = 0; // Expected int-value

				//printf("N1:%.*s\n", (int)i - b1, left + b1);
				//printf("%d. C: %s = N1:%d\n", linepos, name, valInt);
				//printf("%d. C: %s = N1:%d\n", linepos, name, m_COD->GetPubConst(name));
				//printf("%d. C: %s = N1b:%.*s + %.*s\n", linepos, name, (int)e1 - b1 + 1, left + b1, (int)i - b2, left + b2);
				//printf("%d. C: %s = N1b:%.*s + %d\n", linepos, name, (int)e1 - b1 + 1, left + b1, valInt);
				//printf("%d. C: %s = N1b:%s + %d\n", linepos, name, constName, valInt2);
				//printf("%d. C: %s = N1c:%d\n", linepos, name, m_COD->GetPubConst(name));

				state = EState::INITIAL; // fin state set not necesary;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("1SyntaxError: '%c' %s\n", c, left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::NUM1:
			if (isdigit(c))
			{
				// Continue searching end of number
				e1 = i;
			}
			else if (c == '.')
			{
				state = EState::NUM2;
			}
			else if (c == '\0' || c == ',' || c == '\n' || c == '\r') // Found \0, Num finished
			{
				char* strInt = (char*)malloc((i - b1 + 1) * sizeof(char));
				//sprintf_s(left + b1, e1 - b1 + 1, "%.*s", (int)i - b1, left + b1);
				sprintf_s(strInt, i - b1 + 1, "%.*s", (int)i - b1, left + b1);
				int tmpInt = atoi(strInt);

				if (valuecnt > 0 || c == ',')
				{
					if (valuecnt > 0) // Already found min 1 value before
						sprintf_s(result, linelen + 1, "%s, %d", result, tmpInt);
					else
						sprintf_s(result, linelen + 1, "%d", tmpInt);

					//printf("|||%d. C: %s = N2m:%s\n", linepos, name, result);
					valType = 3; // Multivalue in a string

					++valuecnt;
				}
				else
				{
					if (left[b1] == '+' || left[b1] == '-')
					{
						valType = 1; // Expected inc/dec int-value
					}
					else
					{
						valType = 0; // Expected int-value
					}

					*valueInt = tmpInt;
					*valueStr = strInt;
				}

				//m_COD->SetPubConst(name, *valueInt); // Set Const, can override existing

				//printf("N1:%.*s\n", (int)i - b1, left + b1);
				//printf("%d. C: %s = N1:%d\n", linepos, name, valInt);
				//printf("%d. C: %s = N1:%d\n", linepos, name, m_COD->GetPubConst(name));

				state = EState::INITIAL;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError5: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::NUM2:
			if (isdigit(c))
			{
				// Continue searching end of number
			}
			else if (isspace(c)) // Ignore Space
			{
				// Continue searching for end
			}
			else if (c == '\0' || c == ',' || c == ';') // End of string, float finished
			{
				char* strFloat = (char*)malloc((i - b1 + 1) * sizeof(char));
				//sprintf_s(left + b1, e1 - b1 + 1, "%.*s", (int)i - b1, left + b1);
				sprintf_s(strFloat, i - b1 + 1, "%.*s", (int)i - b1, left + b1);
				float tmpFloat = (float)atof(strFloat);

				if (valuecnt > 0 || c == ',')
				{
					if (valuecnt > 0) // Already found min 1 value before
						sprintf_s(result, linelen + 1, "%s, %s", result, strFloat);
					else
						sprintf_s(result, linelen + 1, "%s", strFloat);

					//printf("|||%d. C: %s = N3m:%s\n", linepos, name, result);
					valType = 3; // Multivalue in a string

					++valuecnt;
				}
				else
				{
					valType = 2; // Expected float-value
					*valueFloat = tmpFloat;
					*valueStr = strFloat;
				}

				//m_COD->SetPubConst(name, *valueInt); // Set Const, can override existing

				//printf("N1:%.*s\n", (int)i - b1, left + b1);
				//printf("%d. C: %s = N1:%d\n", linepos, name, valInt);
				//printf("%d. C: %s = N1:%d\n", linepos, name, m_COD->GetPubConst(name));

				state = EState::FIN;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::INCDEC:
			if (isdigit(c))
			{
				// Continue searching end of number
			}
			else if (c == '\0' && isdigit(left[i - 1])) // End of number, incr/decr number finished, atleast the character before needs to be a number
			{
				char* strInt = (char*)malloc((i - b1 + 1) * sizeof(char));
				//sprintf_s(left + b1, e1 - b1 + 1, "%.*s", (int)i - b1, left + b1);
				sprintf_s(strInt, i - b1 + 1, "%.*s", (int)i - b1, left + b1);
				int tmpInt = atoi(strInt);

				if (valuecnt > 0 || c == ',')
				{
					if (valuecnt > 0) // Already found min 1 value before
						sprintf_s(result, linelen + 1, "%s, %d", result, tmpInt);
					else
						sprintf_s(result, linelen + 1, "%d", tmpInt);

					//printf("|||%d. C: %s = N2m:%s\n", linepos, name, result);
					valType = 3; // Multivalue in a string

					++valuecnt;
				}
				else
				{
					valType = 0; // Expected int-value
					*valueInt = tmpInt;
					*valueStr = strInt;
				}

				//m_COD->SetPubConst(name, *valueInt); // Set Const, can override existing

				//printf("N1:%.*s\n", (int)i - b1, left + b1);
				//printf("%d. C: %s = N1:%d\n", linepos, name, valInt);
				//printf("%d. C: %s = N1:%d\n", linepos, name, m_COD->GetPubConst(name));

				state = EState::INITIAL;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;
		}
	}

	if (valType == 3)
	{
		*valueStr = result;
	}

	if (state == EState::SYNTAXERR)
	{
		return -1;
	}

	//printf("%d. C: %.*s = %s\n", linepos, (int)e1 - b1, line + b1, left);
	//printf("%s\n", result);

	return valType;
}

const char* CODParser::ParseLeftName(const char* left, int linepos, int startcharoffset)
{
	enum EState {
		INITIAL, // Beginning state for line. Look for word, comment, etc
		CONSTANT1, // Found =, Try to find space
		CONSTANT2, // Found space. Try to find more spaces or value
		SYNTAXERR // Found :, Try to find value
	};

	//size_t linelen = strlen(left);
	int linelen = (int) strlen(left);

	int state = EState::INITIAL;
	//int b1,e1,b2,e2; // begins and ends of left and right of an expression
	int b1, e1; // begins and ends of left of an expression
	// Check word Syntax and remove spaces
	int valuecnt = 0;
	for (int i = 0; i <= linelen; ++i)
	{
		char c = left[i];
		switch (state)
		{
		case EState::INITIAL:
			if (isalpha(c)) // Constant can only start with characters
			{
				b1 = i;
				state = EState::CONSTANT1;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError1: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::CONSTANT1:
			if (isalnum(c) || c == '_') // Word can include alphanumerics
			{
				// Continue searching end of word
				e1 = i;
				//printf("%c", c);
			}
			else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // Check for spaces at the end
			{
				// ignore spaces, tabs and new lines
				state = EState::CONSTANT2;
			}
			else if (c == '\0') // Found ',', Const finished
			{
				// PROCESS CONSTANT => Get value
				char* constName = (char*)malloc((e1 - b1 + 2) * sizeof(char));
				sprintf_s(constName, e1 - b1 + 2, "%.*s", (int)e1 - b1 + 1, left + b1);

				return constName;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError2: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;

		case EState::CONSTANT2:
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // Check for spaces at the end, but no more characters allowed
			{
				// ignore spaces, tabs and new lines
			}
			else if (c == '\0' || c == ';') // Check for string end or start of comment (comment will be stripped
			{
				// PROCESS CONSTANT => Get value
				char* constName = (char*)malloc((e1 - b1 + 2) * sizeof(char));
				sprintf_s(constName, e1 - b1 + 2, "%.*s", (int)e1 - b1 + 1, left + b1);

				return constName;
			}
			else
			{
				// SYNTAX ERROR: Expression not finished. 
				//printf("SyntaxError: %s\n", left + i);
				printf("SyntaxError3: %s\n", left);
				state = EState::SYNTAXERR;
			}

			break;
		}
	}

	return "";
}

void CODParser::ProcessObjectData(int linepos, int valueType, char* name, char* valueStr, int valueInt, float valueFloat, char* valueComment)
{
	if (m_InMainObject == EInObject::HAUS) // All allowed main objects with "Nummer" identifier
	{
		int name_offset = name[0] == '@' ? 1 : 0;

		//if (linepos == 4933 || linepos == 4938)
		//{
		//	printf("4934 %s %d\n", name, valueInt);
		//}

		if (m_InClass_State == EInObjectState::INIT)
		{
			if (strcmp(name, "@Nummer") == 0 && strcmp(valueStr, "0") == 0)
			{
				m_InClass_State = EInObjectState::SETDEFAULTS;
			}
			else if (strcmp(name + name_offset, "Nummer") == 0)
			{
				if (name_offset && strcmp(valueStr, "+1") == 0)
				{
					//printf("NewObj(%s); // %s\n", valueStr, m_InClass_LastComment);
				}
				else if (name_offset && strcmp(valueStr, "0") == 0) // first "0" = Defaults, ends with ObjFill; second "0" = first entry
				{
					//printf("NewObj(%s); // %s\n", valueStr, m_InClass_LastComment);
				}
				else
				{
					printf("Error: Unknown Nummer param value \"%s\" in CODParser::ProcessObjectData(). Skipping.\n", valueStr);
					return;
				}
				//printf("NewObj(); // %s\n", m_InClass_LastComment);
				m_InClass_State = EInObjectState::CREATEOBJ;
			}
		}
		else if (m_InClass_State == EInObjectState::SETDEFAULTS)
		{
			if (strcmp(name + name_offset, "Nummer") == 0)
			{
				if (name_offset && strcmp(valueStr, "+1") == 0)
				{
					//printf("NewObj(%s); // %s\n", valueStr, m_InClass_LastComment);
				}
				else if (name_offset && strcmp(valueStr, "0") == 0) // first "0" = Defaults, ends with ObjFill; second "0" = first entry
				{
					//printf("NewObj(%s); // %s\n", valueStr, m_InClass_LastComment);
				}
				else
				{
					printf("Error: Unknown Nummer param value \"%s\" in CODParser::ProcessObjectData(). Skipping.\n", valueStr);
					return;
				}
				//printf("NewObj(); // %s\n", m_InClass_LastComment);
				m_InClass_State = EInObjectState::CREATEOBJ;
			}
			else // Set parameters
			{
				m_CODTileInfo->SetTileTypeParameter(-1, name, valueType, valueStr, valueInt, valueFloat, valueComment);
			}
		}
		else if (m_InClass_State == EInObjectState::CREATEOBJ)
		{
			int ParentObjId = -1;

			if (strcmp(name, "ObjFill") == 0)
			{
				//if (linepos == 4933 || linepos == 4938)
				//{
				//	printf("4934 %s %d\n", name, valueInt);
				//}
				ParentObjId = valueInt;
			}

			m_CODTileInfo->CreateTileTypeObj(ParentObjId);
			if (strlen(m_InClass_LastComment) > 0) m_CODTileInfo->SetTileTypeComment(m_InClass_LastId, m_InClass_LastComment);
			m_InClass_State = EInObjectState::PROCESS;

			if (ParentObjId == -1) // No ObjFill? Process first line after Nummer
			{
				ProcessObjectData(linepos, valueType, name, valueStr, valueInt, valueFloat, valueComment);
			}
		}
		else if (m_InClass_State == EInObjectState::PROCESS)
		{
			if (strcmp(name + name_offset, "Nummer") == 0)
			{
				if (name_offset && strcmp(valueStr, "+1") == 0)
				{
					//printf("NewObj(%s); // %s\n", valueStr, m_InClass_LastComment);
				}
				else if (name_offset && strcmp(valueStr, "0") == 0) // first "0" = Defaults, ends with ObjFill; second "0" = first entry
				{
					//printf("NewObj(%s); // %s\n", valueStr, m_InClass_LastComment);
				}
				else
				{
					printf("Error: Unknown Nummer param value \"%s\" in CODParser::ProcessObjectData(). Skipping.\n", valueStr);
					return;
				}
				//printf("NewObj(); // %s\n", m_InClass_LastComment);

				// Previous object is finished! New object begins
				m_InClass_State = EInObjectState::CREATEOBJ;
			}
			else // Set parameters
			{
				//if (strcmp(name, "Id") == 0)
				//if (strcmp(name, "Id") == 0 && valueInt == 21301)
				//{
				//	printf(">|>|741. %s = %d\n", name, valueInt);
				//}
				m_CODTileInfo->SetTileTypeParameter(m_InClass_LastId, name, valueType, valueStr, valueInt, valueFloat, valueComment);
				//if (strcmp(name, "Nummer") == 0 || strcmp(name, "Id") == 0 || strcmp(name, "Gfx") == 0)
				//	printf("   x: %s - %d\n", valueStr, valueInt);
			}
		}

	}
}

void CODParser::AddObjToStack(const char* ObjectName)
{
	if (strcmp(ObjectName, "HAUS") == 0)
	{
		m_InMainObject = EInObject::HAUS;
	}

	char** tmp0 = (char**)(m_ObjectStack_size > 0 ? realloc(m_ObjectStack, (m_ObjectStack_size + 1) * sizeof(*m_ObjectStack)) : malloc(1 * sizeof(*m_ObjectStack)));

	if (tmp0)
	{
		m_ObjectStack = tmp0;
		m_ObjectStack[m_ObjectStack_size] = (char*)ObjectName;
		++m_ObjectStack_size;
	}
}

bool CODParser::RemoveLastObjFromStack()
{
	if (m_ObjectStack_size > 0)
	{
		int lastObjId = --m_ObjectStack_size;
		if (strcmp(m_ObjectStack[lastObjId], "HAUS") == 0)
		{
			m_InMainObject = EInObject::NOOBJECT;
		}

		free(m_ObjectStack[lastObjId]);
		return true;
	}

	return false;
}

const char* CODParser::DebugReturnObjNames() const
{
	//char returnStr[2048];
	char* returnStr = (char*)malloc(2048);
	returnStr[0] = '\0';
	for (int i = 0; i < m_ObjectStack_size; i++)
	{
		sprintf_s(returnStr, 2048, i > 0 ? "%s, %s" : "%s%s", returnStr, m_ObjectStack[i]);
	}
	//printf(">.>%s\n", returnStr);

	return returnStr;
}

const char* CODParser::DebugReturnMainObjName() const
{
	//char returnStr[2048];
	char* returnStr = (char*)malloc(128);

	if (m_InMainObject == EInObject::HAUS)
		sprintf_s(returnStr, 128, "Haus");
	else
		sprintf_s(returnStr, 128, "None");

	return returnStr;
}

void CODParser::AddIntConsts()
{
	//size_t len = sizeof(InternalConsts) / sizeof(SCODIntConst);
	int len = (int) sizeof(InternalConsts) / sizeof(SCODIntConst);
	for (int i = 0; i < len; ++i)
	{
		const SCODIntConst& cnst = InternalConsts[i];
		//printf("InCnst: %s = %d\n", Internals[i].name, Internals[i].value);
		m_COD->AddPubConst(cnst.name, cnst.value, 1);
	}
}

void CODParser::ProcessLastComment(const char* rawCmnt)
{
	//size_t len = strlen(rawCmnt);
	int len = (int) strlen(rawCmnt);
	int b = -1, e = -1;
	
	for (int i = 0; i < len; ++i)
	{
		char c = rawCmnt[i];

		if (b == -1)
		{
			if (c == '-' || c == ' ' || c == '\t')
			{
				// Ignore
			}
			else
			{
				b = i;
				e = i;
			}
		}
		else
		{
			if (c == '-' || c == ' ' || c == '\t' || c == ';' || c == '*' || c == '/')
			{
				// Ignore
			}
			else if (c == '\0')
			{
				break;
			}
			else
			{
				e = i + 1;
			}
		}
	}

	if (b > e) return;

	char* tmp = (char*)malloc((e - b + 1) * sizeof(char));
	sprintf_s(tmp, e - b + 1, "%.*s", (int)e - b, rawCmnt + b);

	m_InClass_LastComment = tmp;

	//printf(";%s\n",tmp);
}
