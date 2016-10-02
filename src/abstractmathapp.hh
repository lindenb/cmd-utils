#ifndef MATH_APP_HH
#define MATH_APP_HH
#include "throw.hh"
#include "debug.hh"

template<class T>
class AbstractMath: public T
	{
	protected:
		virtual bool error_is_fatal()=0;
		unsigned long count_errors;
		virtual void watch(double v)=0;
	public:
		AbstractMath():count_errors(0UL) {
		}
		virtual ~AbstractMath() {
		}
		
	
	protected:	
		virtual void processFILE(const char* input,FILE* in) {
			while(!std::feof(in))
				{
				double num;
				if(std::fscanf(in,"%lf",&num)!=1) {
					int c;
					if((c=std::fgetc(in))==EOF) break;
					if(this->error_is_fatal()) THROW("Bad number, got character (" << c << ")");
					++count_errors;
					continue;
					}
				watch(num);
				}
			}
		
		
	};

#endif

