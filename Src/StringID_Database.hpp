#pragma once

#include "StringID_Internal.hpp"

#if STRINGID_DATABASE

class StringID_Database
{
public:
	static void Init();
	const char *addLiteralString(const char *str, StringIDType id) { return nullptr; }
	const char *addDynamicString(const char *str, StringIDType id) { return nullptr; }
	const char *getString(StringIDType id);
	static StringID_Database *Instance() { static StringID_Database staticInstance; return &staticInstance; }
private:
};
#endif

#if STRINGID_DATABASE
    #define STRINGID_DB_INIT() StringID_Database::Init();
    #define STRINGID_DB_ADD_DYNAMIC(str, id) StringID_Database::Instance()->addLiteralString(str, id)
    #define STRINGID_DB_ADD_LITERAL(str, id) StringID_Database::Instance()->addDynamicString(str, id)
    #define STRINGID_DB_GET_STR(id) StringID_Database::Instance()->getString(id)
#else //ELSE
    #define STRINGID_DB_INIT() false
    #define STRINGID_DB_GET_STR(id) STRINGID_ASSERT(false)
#endif

#if STRINGID_DATABASE

// TODO allocate in Database

#endif