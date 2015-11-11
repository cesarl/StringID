#pragma once

#include "StringID_Config.hpp"

#include <stdint.h>

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
#define STRINGID_COLLISION(STR1, STR2, ID) printf("StringID : Collision id %i between \"%s\" and \"%s\"", ID, STR1, STR2)
#endif