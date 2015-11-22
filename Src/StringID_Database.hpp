#pragma once

#include "StringID_Internal.hpp"

#if STRINGID_CHECK_COLLISION || STRINGID_DEBUG_ENABLED || STRINGID_DATABASE
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
	inline size_t getSize() const;
	inline void lock() const;
	inline void unlock() const;
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
	inline size_t     getSize() const;
	inline uint32_t   saveToBuffer(void *buffer, size_t bufferSize) const;
private:
	StringID_DBMap _map;
};

size_t StringIDDB_GetBinarySaveSize();
uint32_t StringIDDB_SaveBinary(void *buff, size_t allocatedSize);

class StringIDDB_iterator
{
public:
	StringIDDB_iterator(const void *buffer);
	bool isValid() const;
	void get(StringIDType &id, const char *&str) const;
	void next();
private:
	const void *_buffer;
	void *      _ptr;
	uint32_t    _size;
	uint32_t    _index;
	
	StringIDType _id;
	const char * _str;

	StringIDDB_iterator() = delete;
	StringIDDB_iterator(const StringIDDB_iterator &o) = delete;
	StringIDDB_iterator(StringIDDB_iterator &&o) = delete;
	StringIDDB_iterator &operator=(const StringIDDB_iterator &o) = delete;
	StringIDDB_iterator &operator=(StringIDDB_iterator &&o) = delete;
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

size_t StringID_Database::getSize() const
{
	return _map.getSize();
}

template <typename T> 
void copyToBuffer(void *buffer, uint32_t &cursor, const T &value)
{
	void *b = (void*)(size_t(buffer) + size_t(cursor));
	const size_t size = sizeof(T);
	memcpy(b, &value, size);
	cursor += size;
}

void copyToBuffer(void *buffer, uint32_t &cursor, const char *value)
{
	void *b = (void*)(size_t(buffer) + size_t(cursor));
	const size_t size = sizeof(const char) * (strlen(value) + 1);
	memcpy(b, (void*)value, size);
	cursor += size;
}

uint32_t StringID_Database::saveToBuffer(void *buffer, size_t bufferSize) const
{
	uint32_t cursorId = 0;
	const uint32_t entryNumber = getSize();
	uint32_t cursorStr =
		sizeof(uint32_t)
		+ sizeof(StringIDType) * entryNumber
		+ sizeof(uint32_t) * entryNumber;
	if (cursorStr > bufferSize)
		return 0;
	_map.lock();

	copyToBuffer(buffer, cursorId, entryNumber);

	for (auto &it : _map)
	{
		copyToBuffer(buffer, cursorId, it.first);
		uint32_t strPos = cursorStr;
		copyToBuffer(buffer, cursorStr, it.second);
		copyToBuffer(buffer, cursorId, strPos);
	}

	_map.unlock();
	return cursorStr;
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

size_t StringID_DBMap::getSize() const
{
	std::lock_guard<std::mutex> lock(_mutex);
	return size();
}

void StringID_DBMap::lock() const
{
	_mutex.lock();
}

void StringID_DBMap::unlock() const
{
	_mutex.unlock();
}

size_t StringIDDB_GetBinarySaveSize()
{
	size_t entryNumber = StringIDDB.getSize();
	if (entryNumber == 0)
		return 0;

	size_t result = 0;
	// Str buffer
	result += StringIDBuffer.getSize();
	// Ids
	result += entryNumber * sizeof(StringIDType);
	// Ids str addr
	result += entryNumber * sizeof(uint32_t);
	// Number of entry
	result += sizeof(uint32_t);
	return result;
}

uint32_t StringIDDB_SaveBinary(void *buff, size_t allocatedSize)
{
	return StringIDDB.saveToBuffer(buff, allocatedSize);
}

StringIDDB_iterator::StringIDDB_iterator(const void *buffer)
{
	_index = 0;
	if (buffer == STRINGID_NULL)
	{
		_size = 0;
		return;
	}
	_buffer = buffer;
	_size = *(uint32_t*)(buffer);
	_ptr = (void*)(size_t(buffer) + sizeof(uint32_t));
	next();
}

bool StringIDDB_iterator::isValid() const
{
	return _index < _size;
}

void StringIDDB_iterator::get(StringIDType &id, const char *&str) const
{
	id = _id;
	str = _str;
}

void StringIDDB_iterator::next()
{
	_id = *(StringIDType *)(_ptr);
	_ptr = (void*)(size_t(_ptr) + sizeof(StringIDType));
	uint32_t index = *(uint32_t *)(_ptr);
	_str = (char *)(size_t(_buffer) + index);
	_ptr = (void*)(size_t(_ptr) + sizeof(uint32_t));
	++_index;
}

#endif
#endif