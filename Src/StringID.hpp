#pragma once

#include "StringID_Database.hpp"

#if STRINGID_CPP11
#include <utility>
#endif

#if STRINGID_SUPPORT_STD_STRING
#include <xstring>
#endif

#if STRINGID_RT_HASH_ENABLED
#include "fnv.h"

#if STRINGID_64
static StringIDType __lastStringIDHash = FNV1A_64_INIT;
#else
static StringIDType __lastStringIDHash = FNV1A_32_INIT;
#endif

#if STRINGID_64
static StringIDType StringIDHash64(const char *str)
{
	__lastStringIDHash = fnv_64a_str(str, __lastStringIDHash);
	return __lastStringIDHash;
}
#define STRINGID_HASH(str) StringIDHash64(str)
#else
static StringIDType StringIDHash32(const char *str)
{
	__lastStringIDHash = fnv_32a_str(str, __lastStringIDHash);
	return __lastStringIDHash;
}
#define STRINGID_HASH(str) StringIDHash32(str)
#endif

#endif

typedef const char*& StrPtr;

struct StringIDCharWrapper
{
#if STRINGID_SUPPORT_STD_STRING
	explicit
#endif
	StringIDCharWrapper(const char* val) : val(val) {};
	const char* val;
};

class StringID
{
public:
	inline StringID();
	inline ~StringID();
#if STRINGID_RT_HASH_ENABLED
	explicit StringID(StringIDCharWrapper str);
	template <int N> explicit StringID(const char(&str)[N]);
#if STRINGID_SUPPORT_STD_STRING
	explicit StringID(const std::string &str);
#endif
#endif
	explicit inline StringID(const char *str, const StringIDType id);
	explicit inline StringID(const StringIDType id);
	StringID(const StringID &o);
	inline StringID &operator=(const StringID &o);
#if STRINGID_CPP11
	inline StringID &operator=(StringID &&o);
#endif
	inline bool operator==(const StringID &o) const;
	inline bool operator!=(const StringID &o) const;
	inline bool operator<(const StringID &o) const;
	inline bool operator<=(const StringID &o) const;
	inline bool operator>(const StringID &o) const;
	inline bool operator>=(const StringID &o) const;
	bool valid() const;
private:
	StringIDType _id;
#if STRINGID_DEBUG_ENABLED
	const char  *_str;
#endif
};

#ifdef STRINGID_IMPL

StringID::StringID()
	: _id(STRINGID_INVALID_ID)
#if STRINGID_DEBUG_ENABLED
	, _str(nullptr)
#endif
{}

StringID::~StringID()
{}

StringID::StringID(const char *str, const StringIDType id)
	: _id(id)
#if STRINGID_DEBUG_ENABLED
	, _str(str)
#endif
{
	STRINGID_UNUSED(str);
}
////
#if STRINGID_RT_HASH_ENABLED
StringID::StringID(StringIDCharWrapper str)
{
	_id = STRINGID_HASH(str.val);
#if STRINGID_DEBUG_ENABLED
	_str =
#endif
		STRINGID_DB_ADD_DYNAMIC(str.val, _id);
}

template <int N>
StringID::StringID(const char(&str)[N])
{
	_id = STRINGID_HASH(str);
	STRINGID_DB_ADD_LITERAL(str, _id);
#if STRINGID_DEBUG_ENABLED
	_str = str;
#endif
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
////

StringID::StringID(const StringIDType id)
	: _id(id)
#if STRINGID_DEBUG_ENABLED
	, _str(nullptr)
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