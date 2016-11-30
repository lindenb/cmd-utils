#ifndef THROW_HH
#define THROW_HH

#include <iostream>
#include <sstream>
#include <stdexcept>

#define THROW(a) do {std::ostringstream _os; _os << "[" << __FILE__ << ":" << __LINE__ << "] " << a ; std::cerr << _os.str() << std::endl; throw std::runtime_error(_os.str());  } while(0)



#endif

