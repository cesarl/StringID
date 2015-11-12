#pragma once

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

#ifndef STRINGID_64
#define STRINGID_64 1
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

#if STRINGID_RT_HASH_ENABLED
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