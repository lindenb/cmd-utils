#ifndef STRING_UTILS_HH
#define STRING_UTILS_HH

#include <cctype>
#include <string>

class StringUtils
	{
	public:
		// trim from start
		std::string& ltrim(std::string &s) const  {
			std::string::size_type t=0UL;
			while(t<s.size() && std::isspace(s[t])) ++t;
			return s.erase(0,t);
		}

		// trim from end
		std::string& rtrim(std::string &s) const  {
			if(s.empty()) return s;
			std::string::size_type t=s.size();
			while(t>0 && std::isspace(s[t-1]))
				{
				--t; 
				}
			return s.erase(t,s.size());
		}

		// trim from both ends
		std::string& trim(std::string &s) const {
			return ltrim(rtrim(s));
		}

		bool isHex(int c) const {
		return (c>='A' && c<='F') || std::isdigit(c); 
		}
	};

#endif

