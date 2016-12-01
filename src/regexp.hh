#ifndef REGEXP_HH
#define REGEXP_HH
#include <sys/types.h>
#include <regex.h>
#include "throw.hh"
class RegExp
	{
	private:
		regex_t preg;
	public:
		RegExp(const char *regex, int cflags) {
			if(::regcomp(&preg,regex, cflags)!=0) {
				THROW("Cannot compile regular expression \"" << regex << "\"");
				}
			}
	
		RegExp(const char *regex) {
			if(::regcomp(&preg,regex, 0)!=0) {
				THROW("Cannot compile regular expression \"" << regex << "\"");
				}
			}
	
	
		bool test(const char* string,int flags)
			{
			return ::regexec(&preg,string,0,0,flags)==0;
			}
		
		bool test(const char* string)
			{
			return test(string,0);
			}
		
		virtual ~RegExp()
			{
			::regfree(&preg);
			}
	};

       

       

       size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                       size_t errbuf_size);

       void regfree(regex_t *preg);



#endif

