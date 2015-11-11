#pragma once

#include "StringID_Internal.hpp"

#ifdef STRINGID_DATABASE

#ifdef STRINGID_SUPPORT_STD_STRING
#include <string>
#endif

class StringID_Database
{
public:
	static void Init();
	void addStringID(const char *str, StringIDType id);
#ifdef STRINGID_SUPPORT_STD_STRING
	void addStringID(const std::string &str, StringIDType id);
#endif
	const char *getString(StringIDType id);
	StringID_Database *Instance();
private:
};
#endif

#ifdef STRINGID_DATABASE
#define STRINGID_DB_INIT() false
#define STRINGID_DB_ADD(str, id) do{}while(0)
#define STRINGID_DB_ADD_STDSTR(str, id) do{}while(0)
#define STRINGID_DB_GET_STR(id) do{} while (0)
#else ////////// ELSE
#define STRINGID_DB_INIT() StringID_Database::Init();
#define STRINGID_DB_ADD(str, id) StringID_Database::Instance()->addString(str, id)
#ifdef STRINGID_SUPPORT_STD_STRING
#define STRINGID_DB_ADD_STDSTR(str, id) StringID_Database::Instance()->addString(str, id)
#endif
#define STRINGID_DB_GET_STR(id) StringID_Database::Instance()->getString(id)
#endif