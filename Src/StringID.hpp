#pragma once

#include "StringID_Database.hpp"

#if STRINGID_CPP11
#include <utility>
#endif

typedef const char*& StrPtr;

struct StringIDCharWrapper
{
	StringIDCharWrapper(const char* val) : val(val) {};
	const char* val;
};

class StringID
{
public:
	inline StringID();
#if STRINGID_RT_HASH_ENABLED
	explicit StringID(StringIDCharWrapper str)
	{
		_id = 0; // TODO GENERATE ID
		_str = STRINGID_DB_ADD(str.val);
	}

	template <int N>
	explicit StringID(const char(&str)[N])
		: _str(str)
	{
		_str = str;
	}

	template <int N>
	explicit StringID(char(&str)[N])
		: _str(str)
	{
		STRINGID_ASSERT(false);
		_str = str;
	}
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

StringID::StringID()
	: _id(STRINGID_INVALID_ID)
#if STRINGID_DEBUG_ENABLED
	, _str(nullptr)
#endif
{}

StringID::StringID(const char *str, const StringIDType id)
	: _id(id)
#if STRINGID_DEBUG_ENABLED
	, _str(str)
#else
	STRINGID_UNUSED(str)
#endif
{}

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