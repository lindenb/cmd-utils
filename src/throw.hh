#ifndef THROW_HH
#define THROW_HH

#include <iostream>
#include <sstream>

#define THROW(a) do {std::ostringstream _os; _os << "[" << __FILE__ << ":" << __LINE__ << "] " << a ; std::cerr << _os.str(); throw _os.str();  } while(0)



#endif

