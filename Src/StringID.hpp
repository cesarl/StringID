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
#define STRINGID_HASH(str) fnv_64a_str(str, FNV1A_64_INIT)
#else
#define STRINGID_HASH(str) fnv_32a_str(str, FNV1_32A_INIT)
#endif

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