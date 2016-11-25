#include <map>
#include <cmath>
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
			
			
			unsigned long count=1UL;
			if( uniqc )
				{
				count = 0UL;
				string::size_type i1=0;
				while(i1 < line.size() && isspace(line[i1])) i1++;
				if(i1==0) THROW("expected line starting with one or more blanks");
				string::size_type i2=i1;
				while(i2 < line.size() && isdigit(line[i2])) {
					count = count*10 + (line[i2]-'0');
					i2++;
					}
				if(i2==i1 || count==0UL ) THROW("expected a number");

				if(!(i2 < line.size() && isspace(line[i2]))) {
					THROW("expected a space after the count");
					}
				line = line.substr(i2+1);
				}
			std::map<std::string,unsigned long,ci_less>::iterator r=w2c->find(line);
			if( r == w2c->end()) {
				w2c->insert(make_pair(line,count));
				}
			else
				{
				r->second+=count;
				}
				
			}
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		if( histwidth<1) THROW("Bad histogram width");
		ci_less cmp(this->ignorecase);
		w2c = new std::map<std::string,unsigned long,ci_less>(cmp);
		manyOrStdin(optind,argc,argv);
		
		double sum=0.0;
		if( this->histogram )
			{
			for(std::map<std::string,unsigned long,ci_less>::iterator r= w2c->begin();
				r!=w2c->end();
				++r)
				{
				sum+=r->second;
				}
			}
		
		for(std::map<std::string,unsigned long,ci_less>::iterator r= w2c->begin();
			r!=w2c->end();
			++r)
			{
			if(!this->right)
				{
				cout << (r->second) << this->delimiter ;
				}
			if( this->histogram )
				{
				int n = ceil((r->second/sum)*(histwidth));
				for(int i=0;i< histwidth;++i) cout << (i<n?"#":" ");
				cout << this->delimiter ;
				}
			cout << (r->first) ;
			if(this->right)
				{
				cout  << this->delimiter << (r->second);
				}
			cout << endl;
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
