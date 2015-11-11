#pragma once

#include "StringID_Internal.hpp"

#if STRINGID_DATABASE

#if STRINGID_SUPPORT_STD_STRING
#include <string>
#endif

class StringID_Database
{
public:
	static void Init();
	const char *addLiteralString(const char *str, StringIDType id) { return nullptr; }
	const char *addDynamicString(const char *str, StringIDType id) { return nullptr; }
#if STRINGID_SUPPORT_STD_STRING
	const char *addDynamicString(const std::string &str, StringIDType id) { return nullptr; }
#endif
	const char *getString(StringIDType id);
	static StringID_Database *Instance() { static StringID_Database staticInstance; return &staticInstance; }
private:
};
#endif

#if STRINGID_DATABASE
    #define STRINGID_DB_INIT() StringID_Database::Init();
    #define STRINGID_DB_ADD_DYNAMIC(str, id) StringID_Database::Instance()->addLiteralString(str, id)
    #define STRINGID_DB_ADD_LITERAL(str, id) StringID_Database::Instance()->addDynamicString(str, id)
    #if STRINGID_SUPPORT_STD_STRING
        #define STRINGID_DB_ADD_STDSTR(str, id) StringID_Database::Instance()->addDynamicString(str, id)
    #endif
    #define STRINGID_DB_GET_STR(id) StringID_Database::Instance()->getString(id)
#else //ELSE
    #define STRINGID_DB_INIT() false
    #define STRINGID_DB_GET_STR(id) STRINGID_ASSERT(false)
#endif

#if STRINGID_DATABASE

// TODO allocate in Database

#endif