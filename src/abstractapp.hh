#ifndef ABSTRACT_APPLICATION_HH
#define ABSTRACT_APPLICATION_HH
#include <cerrno>
class AbstractApplication
	{
	public:
		AbstractApplication(){}
		virtual ~AbstractApplication(){}
		virtual int main(int argc,char** argv)=0;
		
	};


#endif

