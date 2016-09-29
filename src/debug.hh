#ifndef DEBUG_HH
#define DEBUG_HH


#define WHERE  "[" << __FILE__ << ":" << __LINE__ << "]"

#define DEBUG(a) do{ std::cerr << "[DEBUG]" << WHERE << a << std::endl;} while(0)

#endif

