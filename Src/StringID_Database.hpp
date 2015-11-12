#pragma once

#include "StringID_Internal.hpp"

#if STRINGID_DATABASE

#include <map>

class StringID_Database
{
public:
	const char *addLiteralString(const char *str, StringIDType id);
	const char *addDynamicString(const char *str, StringIDType id);
	const char *getString(StringIDType id) { return STRINGID_NULL; }
private:
	std::map<StringIDType, const char *> _map;
};
#endif


#ifdef STRINGID_IMPL
#if STRINGID_DATABASE

StringID_Database StringIDDB;

#if STRINGID_DATABASE
    #define STRINGID_DB_ADD_DYNAMIC(str, id) StringIDDB.addLiteralString(str, id)
    #define STRINGID_DB_ADD_LITERAL(str, id) StringIDDB.addDynamicString(str, id)
    #define STRINGID_DB_GET_STR(id) StringIDDB.getString(id)
#else //ELSE
    #define STRINGID_DB_GET_STR(id) STRINGID_ASSERT(false)
#endif


const char *StringID_Database::addLiteralString(const char *str, StringIDType id)
{
	// for the moment we copy all
	return addDynamicString(str, id);
}

const char *StringID_Database::addDynamicString(const char *str, StringIDType id)
{
	auto find = _map.find(id);
	if (find != std::end(_map))
	{
		if (strcmp(str, find->second) != 0)
		{
			STRINGID_COLLISION(str, find->second, id);
		}
		return find->second;
	}
	char *copy = (char*)malloc(sizeof(char) * strlen(str));
	strcpy(copy, str);
	_map.insert(std::make_pair(id, copy));
	return copy;
}


#endif
#endif