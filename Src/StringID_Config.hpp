#pragma once

/*

/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
StringIDManager will generate hash based on this file
/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\

*/

#define STRINGID_DEBUG_ENABLED      1//0
#define STRINGID_SUPPORT_STD_STRING 0
//If 1, StringID can hash string at runtime
#define STRINGID_RT_HASH_ENABLED    1
// StringID Database is enabled
#define STRINGID_DATABASE           0
//Add StringID generated at runtime to a database
#define STRINGID_RT_UPDATE_DATABASE 1//0

//If 0, it'll be 32
#define STRINGID_64                 1

#define STRINGID_CPP11              1

//---- Define assertion handler. Defaults to calling assert().
//#define STRINGID_ASSERT(_EXPR)  MyAssert(_EXPR)

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
//#define STRINGID_API __declspec( dllexport )
//#define STRINGID_API __declspec( dllimport )