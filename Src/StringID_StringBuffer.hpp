#pragma once

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