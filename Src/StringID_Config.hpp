#pragma once

/*

/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
StringIDManager will generate hash based on this file
/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\

*/

#define STRINGID_DEBUG_ENABLED      0
#define STRINGID_SUPPORT_STD_STRING 0
//If 1, StringID can hash string at runtime
#define STRINGID_RT_HASH_ENABLED    1
//Add StringID generated at runtime to a database
#define STRINGID_RT_DATABASE        0

//If 0, it'll be 32
#define STRINGID_64                 1