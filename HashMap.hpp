
#include <hash_map>

#ifdef WINDOWS
#undef HashMap
#define HashMap stdext::hash_map
#else
#define HashMap __gnu_cxx::hash_map
#endif