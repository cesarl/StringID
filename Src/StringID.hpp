#pragma once

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// INTERNAL
//////////////////////////////////////////////////////////////////////////

#include "StringID_User.hpp"

#include <stdint.h>

/////////////////////////////////////
/////// Default configuration ///////

#ifndef STRINGID_DEBUG_ENABLED
#ifdef  DEBUG
#define STRINGID_DEBUG_ENABLED      1
#else						        
#define STRINGID_DEBUG_ENABLED      0
#endif
#endif  //!STRINGID_DEBUG_ENABLED

#ifndef STRINGID_RT_HASH_ENABLED
#ifdef  RETAIL
#define STRINGID_RT_HASH_ENABLED    0
#else
#define STRINGID_RT_HASH_ENABLED    1
#endif
#endif  //!STRINGID_RT_HASH_ENABLED

#ifndef STRINGID_SUPPORT_STD_STRING
#define STRINGID_SUPPORT_STD_STRING 0
#endif

#ifndef STRINGID_CHECK_COLLISION
#define STRINGID_CHECK_COLLISION    0
#endif

#ifndef STRINGID_64
#define STRINGID_64                 1
#endif  //!STRINGID_64

#ifndef STRINGID_CPP11
#define STRINGID_CPP11              1
#endif

#ifndef STRINGID_DB_MAP_INCLUDE
#define STRINGID_DB_MAP_INCLUDE     <map>
#endif

#ifndef STRINGID_DB_MAP
#define STRINGID_DB_MAP             std::map
#endif

#ifndef STRINGID_BUFFER_CHUNK_SIZE
#define STRINGID_BUFFER_CHUNK_SIZE  1024 * 1024
#endif

///////////////////////////////////

#define STRINGID_INVALID_ID 0

#if STRINGID_64
typedef uint64_t                    StringIDType;
#else
typedef uint32_t                    StringIDType;
#endif

#ifndef STRINGID_API
#define STRINGID_API
#endif

#ifndef STRINGID_ASSERT
#include <assert.h>
#define  STRINGID_ASSERT(EXPR) assert(EXPR)
#endif

#ifndef STRINGID_UNUSED
#define STRINGID_UNUSED(v)((void*)(&v))
#endif  //!STRINGID_UNUSED   

#if STRINGID_RT_HASH_ENABLED && STRINGID_CHECK_COLLISION
#define STRINGID_DATABASE 1
#endif

#ifndef STRINGID_COLLISION
#include <cstdio>
#define STRINGID_COLLISION(STR1, STR2, ID) printf("StringID : Collision id %u between \"%s\" and \"%s\"\n", ID, STR1, STR2)
#endif

#if STRINGID_CPP11
#define STRINGID_NULL nullptr
#else
#define STRINGID_NULL NULL
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// FNV
//////////////////////////////////////////////////////////////////////////

#if STRINGID_RT_HASH_ENABLED

#if STRINGID_64
typedef uint64_t Fnv64_t;
#define FNV1_64_INIT ((Fnv64_t)0xcbf29ce484222325ULL)
#define FNV_64_PRIME ((Fnv64_t)0x100000001b3ULL)
static inline Fnv64_t STRINGID_HASH(const char *str)
{
	const unsigned char *s = (const unsigned char *)str;	/* unsigned string */
	Fnv64_t hval = FNV1_64_INIT;
	/*
	* FNV-1a hash each octet of the string
	*/
	while (*s) {

		/* xor the bottom with the current octet */
		hval ^= (Fnv64_t)*s++;

		/* multiply by the 64 bit FNV magic prime mod 2^64 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV_64_PRIME;
#else /* NO_FNV_GCC_OPTIMIZATION */
		hval += (hval << 1) + (hval << 4) + (hval << 5) +
			(hval << 7) + (hval << 8) + (hval << 40);
#endif /* NO_FNV_GCC_OPTIMIZATION */
	}

	/* return our new hash value */
	return hval;
}
#else
typedef uint32_t Fnv32_t;
#define FNV_32_PRIME ((Fnv32_t)0x01000193)
#define FNV1_32_INIT ((Fnv32_t)0x811c9dc5)
static inline Fnv32_t STRINGID_HASH(const char *str)
{
	const unsigned char *s = (const unsigned char *)str;	/* unsigned string */
	Fnv32_t hval = FNV1_64_INIT;
	/*
	* FNV-1a hash each octet in the buffer
	*/
	while (*s) {

		/* xor the bottom with the current octet */
		hval ^= (Fnv32_t)*s++;

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV_32_PRIME;
#else
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
#endif
	}

	/* return our new hash value */
	return hval;
}
#endif

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// STRING BUFFER
//////////////////////////////////////////////////////////////////////////

#if STRINGID_CHECK_COLLISION || STRINGID_DEBUG_ENABLED || STRINGID_DATABASE
#define STRINGID_COPY_STRINGS 1

struct StringID_Chunk
{
	char                     buffer[STRINGID_BUFFER_CHUNK_SIZE];
	volatile __int64         index;
	StringID_Chunk           *next;
};

void resetStringIDChunck(StringID_Chunk *chunk);

// Lock Free List
class StringID_LFChunkList
{
private:
	__declspec(align(16))
	struct Int128
	{
		__int64 up;
		__int64 down;
	};

	__declspec(align(16))
	struct Data
	{
		volatile __int64               size;
		volatile StringID_Chunk        *head;
	};

public:
	inline StringID_LFChunkList();
	// Atomic
	inline StringID_Chunk *try_grow();
	// Not atomic
	inline void reduce();
	inline size_t  getSize() const;
private:
	size_t      _nextPtrPadding;
	Data        _datas;
};

class StringID_StringBuffer
{
public:
	StringID_StringBuffer();
	~StringID_StringBuffer();
	char *copy(const char *str, size_t size);
private:
	StringID_LFChunkList _list;
	StringID_Chunk      *_chunk;
};

#ifdef STRINGID_IMPL

// For offsetof
#include <cstddef>

// For interlock exchanges
#include <intrin.h>

// For memset
#include <memory.h>

// For strcpy
#include <string.h>

StringID_LFChunkList::StringID_LFChunkList()
	: _nextPtrPadding(offsetof(StringID_Chunk, next))
{
	STRINGID_ASSERT(_nextPtrPadding != 0);
	STRINGID_ASSERT(_nextPtrPadding % 8 == 0);
	_datas.head = STRINGID_NULL;
	_datas.size = 0;
}

StringID_Chunk *StringID_LFChunkList::try_grow()
{
	Data datas;
	while (true)
	{
		datas = _datas;
		__int64 cr[2];
		cr[0] = (__int64)datas.size; /* low */
		cr[1] = (__int64)datas.head; /* high */
		if (_InterlockedCompareExchange128(&_datas.size, (__int64)datas.head, datas.size, &cr[0]) == 1)
		{
			break;
		}
	}
	Data oldDatas = datas;
	StringID_Chunk *newElement = new StringID_Chunk();
	resetStringIDChunck(newElement);
	StringID_Chunk *elementNext = (StringID_Chunk*)((size_t)newElement + _nextPtrPadding);
	*(size_t*)(elementNext) = (size_t)datas.head;
	STRINGID_ASSERT((size_t)(elementNext) % 8 == 0);
	STRINGID_ASSERT(datas.head != elementNext);

	datas.head = newElement;
	datas.size += 1;

	__int64 comparandResult[2];
	comparandResult[0] = (__int64)oldDatas.size; /* low */
	comparandResult[1] = (__int64)oldDatas.head; /* high */
	if (_InterlockedCompareExchange128(
		&_datas.size,
		(__int64)datas.head,
		datas.size,
		&comparandResult[0]
		) == 1)
	{
		return newElement;
	}
	delete newElement;
	return STRINGID_NULL;
}

void StringID_LFChunkList::reduce()
{
	if (_datas.head)
	{
		StringID_Chunk *next = _datas.head->next;
		delete _datas.head;
		_datas.head = next;
		--_datas.size;
	}
}

size_t StringID_LFChunkList::getSize() const { return _datas.size; }

void resetStringIDChunck(StringID_Chunk *chunk)
{
	memset(chunk->buffer, 0, STRINGID_BUFFER_CHUNK_SIZE);
	chunk->index = 0;
	chunk->next = STRINGID_NULL;
}

StringID_StringBuffer::StringID_StringBuffer()
{
	_chunk = _list.try_grow();
}

StringID_StringBuffer::~StringID_StringBuffer()
{
	while (_list.getSize() > 0)
		_list.reduce();
}

char *StringID_StringBuffer::copy(const char *str, size_t size)
{
	STRINGID_ASSERT(size < STRINGID_BUFFER_CHUNK_SIZE);
	while (true)
	{
		StringID_Chunk *chunk = _chunk;
		size_t originalIndex = _chunk->index;
		size_t newSize = originalIndex + size + 1;
		if (newSize >= STRINGID_BUFFER_CHUNK_SIZE)
		{
			_chunk = _list.try_grow();
			continue;
		}

		if (_InterlockedCompareExchange64(&(chunk->index), newSize, originalIndex) == originalIndex)
		{
			char *res = chunk->buffer + originalIndex;
			strcpy(res, str);
			res[size] = '\0';

			return res;
		}
	}
	return STRINGID_NULL;
}

#endif
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// DATABASE
//////////////////////////////////////////////////////////////////////////

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
	StringID_DBMap();
	const char *findStringID(StringIDType id) const;
	void insertStringID(const char *str, size_t size, StringIDType id);
	inline size_t getSize() const;
	inline size_t getEntryNumber() const;
	inline void lock() const;
	inline void unlock() const;
private:
	// todo -> to pass to cpp11, platform specific
	mutable std::mutex _mutex;
	size_t _totalSize;
};

class StringID_Database
{
public:
	inline const char *addLiteralString(const char *str, StringIDType id);
	inline const char *addDynamicString(const char *str, StringIDType id);
	inline const char *getString(StringIDType id) const;
	inline size_t     getSize() const;
	inline uint32_t   saveToBuffer(void *buffer, size_t bufferSize) const;
	inline bool       loadFromBuffer(const void *buffer, size_t bufferSize);
private:
	StringID_DBMap _map;
};

size_t   StringIDDB_GetBinarySaveSize();
uint32_t StringIDDB_SaveBinary(void *buff, size_t allocatedSize);
bool     StringIDDB_LoadBinary(const void *buff, size_t allocatedSize);

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
#pragma warning(push)
#pragma warning(disable: 4640)
static inline StringID_Database &StringID_DatabaseSingleton() { static StringID_Database db; return db; }
#define StringIDDB StringID_DatabaseSingleton()
#pragma warning(pop)
#endif

#if STRINGID_COPY_STRINGS
#pragma warning(push)
#pragma warning(disable: 4640)
static inline StringID_StringBuffer &StringID_StringBufferSingleton() { static StringID_StringBuffer sb; return sb; }
#define StringIDBuffer StringID_StringBufferSingleton()
#pragma warning(pop)
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
	_map.insertStringID(str, strlen(str), id);
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

	size_t len = strlen(str);
#if STRINGID_COPY_STRINGS
	char *copy = StringIDBuffer.copy(str, len);
#if STRINGID_CHECK_COLLISION
	_map.insertStringID(copy, len, id);
#endif
	return copy;
#endif
	return nullptr;
}

const char *StringID_Database::getString(StringIDType id) const
{
#if STRINGID_CHECK_COLLISION
	const char *find = _map.findStringID(id);
	if (find != STRINGID_NULL)
	{
		return find;
	}
#endif
	return STRINGID_NULL;
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
	const uint32_t entryNumber = _map.getEntryNumber();
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

bool StringID_Database::loadFromBuffer(const void *buffer, size_t bufferSize)
{
	StringIDDB_iterator it(buffer);
	while (it.isValid())
	{
		StringIDType id;
		const char *str;
		it.get(id, str);
		STRINGID_DB_ADD_DYNAMIC(str, id);
		it.next();
	}
	return true;
}

StringID_DBMap::StringID_DBMap()
	: _totalSize(0)
{}

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

void StringID_DBMap::insertStringID(const char *str, size_t size, StringIDType id)
{
	std::lock_guard<std::mutex> lock(_mutex);
	STRINGID_ASSERT(find(id) == end() && "ID already exists.");
	insert(std::make_pair(id, str));
	_totalSize += size + sizeof(StringIDType) + sizeof(uint32_t);
}

size_t StringID_DBMap::getSize() const
{
	return _totalSize;
}

size_t StringID_DBMap::getEntryNumber() const
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
	size_t mapSize = StringIDDB.getSize();
	if (mapSize == 0)
		return 0;

	size_t result = mapSize;
	result += sizeof(uint32_t);
	return result;
}

uint32_t StringIDDB_SaveBinary(void *buff, size_t allocatedSize)
{
	return StringIDDB.saveToBuffer(buff, allocatedSize);
}

bool     StringIDDB_LoadBinary(const void *buff, size_t allocatedSize)
{
	return StringIDDB.loadFromBuffer(buff, allocatedSize);
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// STRING ID
//////////////////////////////////////////////////////////////////////////

#if STRINGID_CPP11
#include <utility>
#endif

#if STRINGID_SUPPORT_STD_STRING
#include <xstring>
#endif

#define STRINGID_MANAGER_DISABLED()()
#define STRINGID_MANAGER_ENABLED()()

typedef const char*& StrPtr;

struct StringIDCharWrapper
{
#if STRINGID_SUPPORT_STD_STRING
	explicit
#endif
	StringIDCharWrapper(const char* val);
	const char* val;
};

class StringID
{
public:
	StringID();
	~StringID();
	StringIDType getId() const;
	const char *str() const;
#if STRINGID_RT_HASH_ENABLED
	explicit StringID(StringIDCharWrapper str);
	template <int N> StringID(const char(&str)[N])
	{
		internalConstructLiteral(str);
	}
#if STRINGID_SUPPORT_STD_STRING
	StringID(const std::string &str);
#endif
#endif
	explicit StringID(StringIDCharWrapper str, StringIDType id);
	explicit StringID(const StringIDType id);
	template <int N> StringID(const char(&str)[N], StringIDType id)
	{
		internalConstructLiteral(str, id);
	}
	StringID(const StringID &o);
	StringID &operator=(const StringID &o);
#if STRINGID_CPP11
	StringID &operator=(StringID &&o);
#endif
	bool operator==(const StringID &o) const;
	bool operator!=(const StringID &o) const;
	bool operator<(const StringID &o) const;
	bool operator<=(const StringID &o) const;
	bool operator>(const StringID &o) const;
	bool operator>=(const StringID &o) const;
	bool valid() const;
private:
#if STRINGID_RT_HASH_ENABLED
	void internalConstructLiteral(const char *str);
#endif
	void internalConstructLiteral(const char *str, StringIDType id);

	StringIDType _id;
#if STRINGID_DEBUG_ENABLED
	const char  *_str;
#endif
};

#ifdef STRINGID_IMPL

StringIDCharWrapper::StringIDCharWrapper(const char* val)
	: val(val)
{};

StringID::StringID()
	: _id(STRINGID_INVALID_ID)
#if STRINGID_DEBUG_ENABLED
	, _str(STRINGID_NULL)
#endif
{}

StringID::~StringID()
{}

StringIDType StringID::getId() const
{
	return _id;
}

const char *StringID::str() const
{
#if STRINGID_DEBUG_ENABLED
	return _str;
#elif STRINGID_DATABASE
	return STRINGID_DB_GET_STR(_id);
#else
	return STRINGID_NULL;
#endif
}

void StringID::internalConstructLiteral(const char *str)
{
	_id = STRINGID_HASH(str);
#if STRINGID_DEBUG_ENABLED
	_str =
#endif
		STRINGID_DB_ADD_LITERAL(str, _id);
}

void StringID::internalConstructLiteral(const char *str, StringIDType id)
{
	_id = id;
#if STRINGID_DEBUG_ENABLED
	_str =
#endif
		STRINGID_DB_ADD_LITERAL(str, _id);
}

StringID::StringID(StringIDCharWrapper str, const StringIDType id)
	: _id(id)
{
#if STRINGID_DEBUG_ENABLED
	_str =
#endif
	    STRINGID_DB_ADD_DYNAMIC(str.val, id);
}

#if STRINGID_RT_HASH_ENABLED
StringID::StringID(StringIDCharWrapper str)
{
	_id = STRINGID_HASH(str.val);
#if STRINGID_DEBUG_ENABLED
	_str =
#endif
		STRINGID_DB_ADD_DYNAMIC(str.val, _id);
}

#if STRINGID_SUPPORT_STD_STRING
StringID::StringID(const std::string &str)
{
	_id = STRINGID_HASH(str.c_str());
#if STRINGID_DEBUG_ENABLED
	_str =
#endif
		STRINGID_DB_ADD_DYNAMIC(str.c_str(), _id);
}
#endif
#endif

StringID::StringID(const StringIDType id)
	: _id(id)
#if STRINGID_DEBUG_ENABLED
	, _str(STRINGID_NULL)
#endif
{}

StringID::StringID(const StringID &o)
	: _id(o._id)
#if STRINGID_DEBUG_ENABLED
	, _str(o._str)
#endif
{

}

StringID &StringID::operator=(const StringID &o)
{
	_id = o._id;
#if STRINGID_DEBUG_ENABLED
	_str = o._str;
#endif
	return *this;
}

#if STRINGID_CPP11
StringID &StringID::operator=(StringID &&o)
{
	_id = std::move(o._id);
#if STRINGID_DEBUG_ENABLED
	_str = std::move(o._str);
#endif
	return *this;
}
#endif

bool StringID::operator==(const StringID &o) const
{
	return _id == o._id;
}

bool StringID::operator!=(const StringID &o) const
{
	return _id != o._id;
}

bool StringID::operator<(const StringID &o) const
{
	return _id < o._id;
}

bool StringID::operator<=(const StringID &o) const
{
	return _id <= o._id;
}

bool StringID::operator>(const StringID &o) const
{
	return _id > o._id;
}

bool StringID::operator>=(const StringID &o) const
{
	return _id >= o._id;
}

bool StringID::valid() const
{
	return _id != STRINGID_INVALID_ID;
}

#endif