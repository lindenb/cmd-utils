#include <map>
#include "wordcount.tab.h"


using namespace std;

/** http://stackoverflow.com/a/1801913/58082 */
 struct ci_less : std::binary_function<std::string, std::string, bool>
  {
    bool ignore_case;
    ci_less(bool ignore_case):ignore_case(ignore_case)
    	{
    	
    	}
    // case-independent (ci) compare_less binary function
    struct nocase_compare : public std::binary_function<unsigned char,unsigned char,bool> 
    {
      bool operator() (const unsigned char& c1, const unsigned char& c2) const {
          return tolower (c1) < tolower (c2); 
      }
    };
    bool operator() (const std::string & s1, const std::string & s2) const {
    	if(!ignore_case) return s1<s2;
      return std::lexicographical_compare 
        (s1.begin (), s1.end (),   // source range
        s2.begin (), s2.end (),   // dest range
        nocase_compare ());  // comparison
    }
  };

class WordCount: public WordCountBase {
public:
	std::map<std::string,unsigned long,ci_less>* w2c;
	WordCount():w2c(0) {
		}
	
	virtual void processIstream(const char* f,std::istream& in) {
		string line;
		while(getline(in,line)) {
			std::map<std::string,unsigned long,ci_less>::iterator r=w2c->find(line);
			if( r == w2c->end()) {
				w2c->insert(make_pair(line,1UL));
				}
			else
				{
				r->second++;
				}
			}
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		ci_less cmp(this->ignorecase);
		w2c = new std::map<std::string,unsigned long,ci_less>(cmp);
		manyOrStdin(optind,argc,argv);
		
		for(std::map<std::string,unsigned long,ci_less>::iterator r= w2c->begin();
			r!=w2c->end();
			++r)
			{
			cout << (r->second) << this->delimiter << (r->first) << endl;
			}
		delete w2c;
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	WordCount app;
	return app.main(argc,argv);
	}
