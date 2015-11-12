#pragma once

/*

/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
StringIDManager config
/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\

Full debug config example :
STRINGID_DEBUG_ENABLED       1
STRINGID_RT_HASH_ENABLED     1


Release config example :
STRINGID_DEBUG_ENABLED       0
STRINGID_RT_HASH_ENABLED     1

Retail config example :
STRINGID_DEBUG_ENABLED       0
STRINGID_RT_HASH_ENABLED     0

*/

//StringID keep a const char ptr*
//#define STRINGID_DEBUG_ENABLED      1

//StringID hash string at runtime
//#define STRINGID_RT_HASH_ENABLED    1

//StringID handle std::string
//#define STRINGID_SUPPORT_STD_STRING 1

//If 0, it'll be 32
//#define STRINGID_64               0

// Disable CPP11 features
//#define STRINGID_CPP11            0

//---- Define assertion handler. Defaults to calling assert().
//#define STRINGID_ASSERT(_EXPR)  MyAssert(_EXPR)

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define STRINGID_API __declspec( dllexport )
//#define STRINGID_API __declspec( dllimport )

//#define STRINGID_COLLISION(STR1, STR2, ID)

//#define STRINGID_DB_MAP_INCLUDE     <unordered_map>
//#define STRINGID_DB_MAP             std::unordered_map

//#define STRINGID_BUFFER_CHUNK_SIZE  Name your size ( default : 1024 )