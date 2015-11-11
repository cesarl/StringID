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
	void addString(const char *str, StringIDType id);
#if STRINGID_SUPPORT_STD_STRING
	void addString(const std::string &str, StringIDType id);
#endif
	const char *getString(StringIDType id);
	static StringID_Database *Instance();
private:
};
#endif

#if STRINGID_DATABASE
    #define STRINGID_DB_INIT() StringID_Database::Init();
    #define STRINGID_DB_ADD(str, id) StringID_Database::Instance()->addString(str, id)
    #if STRINGID_SUPPORT_STD_STRING
        #define STRINGID_DB_ADD_STDSTR(str, id) StringID_Database::Instance()->addString(str, id)
    #endif
    #define STRINGID_DB_GET_STR(id) StringID_Database::Instance()->getString(id)
#else //ELSE
    #define STRINGID_DB_INIT() false
    #if STRINGID_COPY_DYNAMIC_STRING
         #define STRINGID_DB_ADD(str, id) copyChar(str)
        #if STRINGID_SUPPORT_STD_STRING
            #define STRINGID_DB_ADD_STDSTR(str, id) copyChar(str.c_str())
        #endif
    #else
        #define STRINGID_DB_ADD(str, id) nullptr
        #if STRINGID_SUPPORT_STD_STRING
            #define STRINGID_DB_ADD_STDSTR(str, id) nullptr
        #endif
    #endif
    #define STRINGID_DB_GET_STR(id) STRINGID_ASSERT(false)
#endif

#if STRINGID_DATABASE == 0

#include <malloc.h>

inline char *copyChar(const char *str)
{
	char *buf = (char*)malloc(sizeof(char) * strlen(str));
	strcpy(buf, str);
	return buf;
}

#elif STRINGID_DATABASE

// TODO allocate in Database

#endif

#if STRINGID_DATABASE == 0

#else ////////// ELSE

#endif