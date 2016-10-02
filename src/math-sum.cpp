#include <cstdio>
#include <cstdlib>
#include <inttypes.h> 
#include "math-sum.tab.h"
#include "abstractmathapp.hh"
using namespace std;

class MathSum: public AbstractMath<MathSumBase>
	{
	public:
		double total;
		MathSum():total(0.0) {}
		virtual ~MathSum() {}
		virtual int main(int argc,char** argv) {
			int optind = parseOptions(argc,argv);
			this->fManyOrStdin(optind,argc,&argv[optind]);
			cout << total <<endl;
			return EXIT_SUCCESS;
			}
	protected:
		virtual bool error_is_fatal() { return !ignore_errors;};
		virtual void watch(double v)
			{
			total+=v;
			}
		
	};

int main(int argc,char** argv)
	{
	MathSum app;
	return app.main(argc,argv);
	}

