
#include <vector>
#include <string>

class StringSplitter {

public:
	std::string delim;
	bool ignore_trailing;
	StringSplitter():delim("\t"),ignore_trailing(true) {
		}
	
	std::vector<std::string>::size_type split(const std::string& s,std::vector<std::string>& tokens) {
		tokens.clear();
		std::string::size_type prev= 0UL;
		for(;;) {
			std::string::size_type n  = s.find(this->delim,prev);
			if(n==std::string::npos) {
				tokens.push_back(s.substr(prev));
				break;
				}
			else
				{
				tokens.push_back(s.substr(prev,n-prev));
				prev = n + this->delim.size();
				}
			}
		if(ignore_trailing) {
			while(!tokens.empty() && tokens.back().empty()) tokens.pop_back();
			}
		return tokens.size();
		}
	std::vector<std::string> split(const std::string& s) {
		std::vector<std::string> tokens;
		split(s,tokens);
		return tokens;
	}
};
