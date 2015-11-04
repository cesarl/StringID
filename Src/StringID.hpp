#pragma once

/*
Configs :

#define STRINGID_DEBUG_ENABLED      ( 0 by default )
#define STRINGID_SUPPORT_STD_STRING ( 0 by default )

If true, StringID can hash string at runtime
#define STRINGID_RT_HASH_ENABLED    ( 1 by default )

Add generate StringID generated at runtime to a database
#define STRINGID_RT_DATABASE        ( 0 by default )

*/

#define STRINGID_INVALID_ID 0

#ifndef STRINGID_RT_HASH_ENABLED
#define STRINGID_RT_HASH_ENABLED 1
#endif

class StringID
{
public:
	inline StringID()
		: _id(STRINGID_INVALID_ID)
		#if STRINGID_DEBUG_ENABLED
		, _str(nullptr)
		#endif
	{}

#if STRINGID_RT_HASH_ENABLED
	inline StringID(const char *str)
	{
	// TODO Generate hash
#if STRINGID_DEBUG_ENABLED
		_str = str;
#endif
	}
#endif

	inline StringID(const uint64_t id)
		: _id(id)
#if STRINGID_DEBUG_ENABLED
		, _str(nullptr)
#endif
	{}

	inline StringID(const StringID &o)
		: _id(o._id)
#if STRINGID_DEBUG_ENABLED
		, _str(o._str)
#endif
	{}

	inline StringID(StringID &&o)
		: _id(std::move(o._id))
#if STRINGID_DEBUG_ENABLED
		, _str(std::move(o._str))
#endif
	{}

	inline StringID(const StringID &o)
		: _id(o._id)
#if STRINGID_DEBUG_ENABLED
		, _str(o._str)
#endif
	{}

	inline StringID &operator=(const StringID &o)
		: _id(o._id)
#if STRINGID_DEBUG_ENABLED
		, _str(o._str)
#endif
	{
		return *this;
	}

	inline StringID &operator=(StringID &&o)
		: _id(std::move(o._id))
#if STRINGID_DEBUG_ENABLED
		, _str(std::move(o._str))
#endif
	{
		return *this;
	}

	inline bool operator==(const StringID &o) const
	{
		return _id == o._id;
	}

	inline bool operator!=(const StringID &o) const
	{
		return _id != o._id;
	}

	inline bool operator<(const StringID &o) const
	{
		return _id < o._id;
	}

	inline bool operator<=(const StringID &o) const
	{
		return _id <= o._id;
	}

	inline bool operator>(const StringID &o) const
	{
		return _id > o._id;
	}

	inline bool operator>=(const StringID &o) const
	{
		return _id >= o._id;
	}

	bool valid() const
	{
		return _id != STRINGID_INVALID_ID;
	}
private:
	uint64_t    _id;
#if STRINGID_DEBUG_ENABLED
	const char *_str;
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

StringID::StringID(const uint64_t id)
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

StringID::StringID(const StringID &o)
	: _id(o._id)
#if STRINGID_DEBUG_ENABLED
	, _str(o._str)
#endif
{}

StringID::StringID &operator=(const StringID &o)
	: _id(o._id)
#if STRINGID_DEBUG_ENABLED
	, _str(o._str)
#endif
{
	return *this;
}

StringID::StringID &operator=(StringID &&o)
	: _id(std::move(o._id))
#if STRINGID_DEBUG_ENABLED
	, _str(std::move(o._str))
#endif
{
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