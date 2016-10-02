#ifndef ABSTRACT_APPLICATION_HH
#define ABSTRACT_APPLICATION_HH
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include "throw.hh"

class AbstractApplication
	{
	public:
		AbstractApplication(){}
		virtual ~AbstractApplication(){}
		virtual const char* get_application_name() const = 0;
		
		
		virtual void processIstream(const char* f,std::istream& in) {
		THROW("no implemented");
		}
		
		
		virtual void oneOrStdin(int optind,int argc,char** argv)
			{
			if(optind==argc)
				{
				processIstream("<STDIN>",std::cin);
				}
			else if(optind+1==argc)
				{
				std::ifstream f(argv[optind]);
				if(!f.is_open()) {
					THROW("Cannot open \"" << argv[optind] << "\": " << std::strerror(errno) << ".");
					}
				processIstream(argv[optind],f);
				f.close();
				}
			else
				{
				THROW("Illegal Number of arguments.");
				}
			}
		
		virtual void manyOrStdin(int optind,int argc,char** argv)
			{
			if(optind==argc)
				{
				processIstream("<STDIN>",std::cin);
				}
			else {
				while(optind < argc)
					{
					std::ifstream f(argv[optind]);
					if(!f.is_open()) {
						THROW("Cannot open \"" << argv[optind] << "\": " << std::strerror(errno) << ".");
						}
					processIstream(argv[optind],f);
					f.close();
					++optind;
					}
				}
			}
			
		virtual int main(int argc,char** argv)=0;
		
	};


#endif

