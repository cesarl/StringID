#pragma once

/*

/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
StringIDManager config
/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\

Full debug config example :
STRINGID_DEBUG_ENABLED       1
STRINGID_RT_HASH_ENABLED     1
STRINGID_CHECK_COLLISIONS    1


Release config example :
STRINGID_DEBUG_ENABLED       0
STRINGID_RT_HASH_ENABLED     1
STRINGID_CHECK_COLLISIONS    1

Retail config example :
STRINGID_DEBUG_ENABLED       0
STRINGID_RT_HASH_ENABLED     0

*/

//StringID keep a const char ptr*
#define STRINGID_DEBUG_ENABLED      1//0

//StringID hash string at runtime
#define STRINGID_RT_HASH_ENABLED    1

// Detect collisions at runtime
// It'll enable database
#if STRINGID_RT_HASH_ENABLED
    #define STRINGID_CHECK_COLLISIONS   0
    #define STRINGID_SUPPORT_STD_STRING 0
#endif

//If 0, it'll be 32
#define STRINGID_64                 1

#define STRINGID_CPP11              1

//---- Define assertion handler. Defaults to calling assert().
//#define STRINGID_ASSERT(_EXPR)  MyAssert(_EXPR)

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define STRINGID_API __declspec( dllexport )
//#define STRINGID_API __declspec( dllimport )