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
		
		std::string& fromUniq(std::string &line,unsigned long* uniqcount) const {
			unsigned long count=0UL;
			string::size_type i1=0;
			while(i1 < line.size() && std::isspace(line[i1])) i1++;
			if(i1==0) THROW("expected line starting with one or more blanks");
			string::size_type i2=i1;
			while(i2 < line.size() && std::isdigit(line[i2])) {
				count = count*10 + (line[i2]-'0');
				i2++;
				}
			if(i2==i1 || count==0UL ) THROW("expected a number");

			if(!(i2 < line.size() && std::isspace(line[i2]))) {
				THROW("expected a space after the count");
				}
			line = line.substr(i2+1);
			if(uniqcount!=0) *uniqcount=count;
			return line;
			}
	};

#endif

