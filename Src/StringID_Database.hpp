#pragma once

#include "StringID_Internal.hpp"

#if STRINGID_CHECK_COLLISION || STRINGID_DEBUG_ENABLED
#define STRINGID_COPY_STRINGS 1
#include "StringID_StringBuffer.hpp"
#endif

#if STRINGID_DATABASE

#include STRINGID_DB_MAP_INCLUDE

#pragma warning(push)
#pragma warning(disable:4265)
#include <mutex> // to remove for not CPP11
#pragma warning(pop)


typedef STRINGID_DB_MAP<StringIDType, const char*> StringIdMapType;

class StringID_DBMap : public StringIdMapType
{
public:
	const char *findStringID(StringIDType id) const;
	void insertStringID(const char *str, StringIDType id);
private:
	// todo -> to pass to cpp11, platform specific
	mutable std::mutex _mutex;
};

class StringID_Database
{
public:
	inline const char *addLiteralString(const char *str, StringIDType id);
	inline const char *addDynamicString(const char *str, StringIDType id);
	inline const char *getString(StringIDType id) { return STRINGID_NULL; }
private:
	StringID_DBMap _map;
};


#endif


#ifdef STRINGID_IMPL

#if STRINGID_DATABASE
StringID_Database     StringIDDB;
#endif

#if STRINGID_COPY_STRINGS
StringID_StringBuffer StringIDBuffer;
#endif

#if STRINGID_DATABASE
	#define STRINGID_DB_ADD_DYNAMIC(str, id) StringIDDB.addDynamicString(str, id)
	#define STRINGID_DB_ADD_LITERAL(str, id) StringIDDB.addLiteralString(str, id)
    #define STRINGID_DB_GET_STR(id) StringIDDB.getString(id)
#elif STRINGID_DEBUG_ENABLED //ELSE debug only
    #define STRINGID_DB_ADD_DYNAMIC(str, id) StringIDBuffer.copy(str, strlen(str))
    #define STRINGID_DB_ADD_LITERAL(str, id) str
    #define STRINGID_DB_GET_STR(id) STRINGID_NULL
#else // ELSE
	#define STRINGID_DB_ADD_DYNAMIC(str, id) do{}while(0)
	#define STRINGID_DB_ADD_LITERAL(str, id) do{}while(0)
	#define STRINGID_DB_GET_STR(id) STRINGID_NULL
#endif

#if STRINGID_DATABASE

const char *StringID_Database::addLiteralString(const char *str, StringIDType id)
{
#if STRINGID_CHECK_COLLISION
	const char *find = _map.findStringID(id);
	if (find != STRINGID_NULL)
	{
		if (strcmp(str, find) != 0)
		{
			STRINGID_COLLISION(str, find, id);
		}
		return find;
	}
	_map.insertStringID(str, id);
#endif
	return str;
}

const char *StringID_Database::addDynamicString(const char *str, StringIDType id)
{
#if STRINGID_CHECK_COLLISION
	const char *find = _map.findStringID(id);
	if (find != STRINGID_NULL)
	{
		if (strcmp(str, find) != 0)
		{
			STRINGID_COLLISION(str, find, id);
		}
		return find;
	}
#endif

#if STRINGID_COPY_STRINGS
	char *copy = StringIDBuffer.copy(str, strlen(str));
#if STRINGID_CHECK_COLLISION
	_map.insertStringID(copy, id);
#endif
	return copy;
#endif
	return nullptr;
}

const char *StringID_DBMap::findStringID(StringIDType id) const
{
	std::lock_guard<std::mutex> lock(_mutex);
	StringID_DBMap::const_iterator found = find(id);
	if (found == end())
	{
		return STRINGID_NULL;
	}
	return found->second;
}

void StringID_DBMap::insertStringID(const char *str, StringIDType id)
{
	std::lock_guard<std::mutex> lock(_mutex);
	STRINGID_ASSERT(find(id) == end() && "ID already exists.");
	insert(std::make_pair(id, str));
}

#endif
#endif