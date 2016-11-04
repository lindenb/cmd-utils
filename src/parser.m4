define(`concat',`$1$2')dnl
changecom([,])dnl
define(`HEADERNAME',`concat(TOOL,`_M4_HH')')dnl
define(`CLASSNAME',`concat(TOOL,`Parser')')dnl
#ifndef HEADERNAME
#define HEADERNAME
#include <iostream>
#include <cstdio>
#include "json_array_foreach.bison.h"

class CLASSNAME
	{
	public:
		typedef void* scanner_t;
		typedef void* yystype;
	private:
		std::istream* istr;
		std::FILE* fin;
		scanner_t scanner;
		YYSTYPE lvalp;
	public:
		CLASSNAME (std::istream& is):istr(&is),fin(0),scanner(0) {
			::concat(TOOL,`lex_init')(&scanner);
			::concat(TOOL,`set_extra')(this, scanner);
			}
		CLASSNAME (std::FILE* is):istr(0),fin(is),scanner(0) {
			::concat(TOOL,`lex_init')(&scanner);
			::concat(TOOL,`set_extra')(this, scanner);
			}
		~CLASSNAME ()
			{
			::concat(TOOL,`lex_destroy')( scanner );
			}
		int lex() {
			return ::concat(TOOL,`lex')( &lvalp,scanner );
			}
		void do_before() {
			std::cout <<"do before" << std::endl;
			}
		void do_new_file() {
			std::cout <<"new file" << std::endl;
			}
		std::streamsize read(char* buff,int max_size) {
			std::cerr <<"ask="<<  max_size << std::endl;
			if( istr!=0)
				{
				this->istr->read(buff,max_size);
				std::cerr <<"count="<< istr->gcount()  <<"/" << max_size << std::endl;
				return istr->gcount();
				}
			else
				{
				return std::fread((void*)buff,sizeof(char),max_size,fin);
				}
			}
	};


#endif

