#pragma once

/*
/!\ Set your configs in StringID_Config.hpp
*/

#include "StringID_Database.hpp"

class StringID
{
public:
	inline StringID();
#if STRINGID_RT_HASH_ENABLED
	inline StringID(const char *str);
#endif
	inline StringID(const StringIDType id);
	inline StringID(const StringID &o);
	inline StringID(StringID &&o);
	inline StringID &operator=(const StringID &o);
	inline StringID &operator=(StringID &&o);
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

#if STRINGID_RT_HASH_ENABLED
StringID::StringID(const char *str)
{
	// TODO Generate hash
#if STRINGID_DEBUG_ENABLED
	_str = str;
#endif
}
#endif

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
{}

StringID::StringID(StringID &&o)
	: _id(std::move(o._id))
#if STRINGID_DEBUG_ENABLED
	, _str(std::move(o._str))
#endif
{}

StringID &StringID::operator=(const StringID &o)
{
	_id = o._id;
#if STRINGID_DEBUG_ENABLED
	_str = o._str;
#endif
	return *this;
}

StringID &StringID::operator=(StringID &&o)
{
	_id = std::move(o._id);
#if STRINGID_DEBUG_ENABLED
	_str = std::move(o._str);
#endif
	return *this;
}

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