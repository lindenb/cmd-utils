#ifndef THROW_HH
#define THROW_HH

#include <iostream>
#include <sstream>

#define THROW(a) do {std::ostringstream _os; os << "[" << __FILE__ << ":" << __LINE__ << "] " << a ; std::cerr << os.str(); throw os.str();  } while(0)


#endif

