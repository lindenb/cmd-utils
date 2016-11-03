#ifndef TOOL_M4
#define TOOL_M4
#include <iostream>
#include <cstdio>




class TOOL_lexer
	{
	private:
		std::istream* in;
		std::FILE* fin;
		yyscan_t scanner;
	public:
		TOOL_lexer(std::istream& in):in(&in),fin(0),scanner(0) {
			::TOOLlex_init(&scanner);
			::TOOLset_extra(this, scanner);
			}
		TOOL_lexer(std::FILE* in):in(0),fin(in),scanner(0) {
			::TOOLlex_init(&scanner);
			::TOOLset_extra(this, scanner);
			}
		~TOOL_lexer()
			{
			::TOOLlex_destroy( scanner );
			}
		int lex() {
			return ::TOOLlex( scanner );
			}
		void do_before() {
			std::cout <<"do before" << std::endl;
			}
		void do_new_file() {
			std::cout <<"new file" << std::endl;
			}
		std::streamsize read(char* buff,int max_size) {
			std::cerr <<"ask="<<  max_size << std::endl;
			if( in!=0)
				{
				this->in->read(buff,max_size);
				std::cerr <<"count="<< in->gcount()  <<"/" << max_size << std::endl;
				return in->gcount();
				}
			else
				{
				return std::fread((void*)buff,sizeof(char),max_size,fin);
				}
			}
	};


#endif

