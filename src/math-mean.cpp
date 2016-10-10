#include <cstdio>
#include <cstdlib>
#include <inttypes.h> 
#include "math-mean.tab.h"
#include "abstractmathapp.hh"
using namespace std;

class MathMean: public AbstractMath<MathMeanBase>
	{
	public:
		double total;
		unsigned int count;
		MathMean():total(0.0),count(0U) {}
		virtual ~MathMean() {}
		virtual int main(int argc,char** argv) {
			int optind = parseOptions(argc,argv);
			this->fManyOrStdin(optind,argc,&argv[optind]);
			if(count==0)
				{
				cerr << "No Data" << endl;
				return EXIT_FAILURE;
				}
			cout << total/(double)count <<endl;
			return EXIT_SUCCESS;
			}
	protected:
		virtual bool error_is_fatal() { return !ignore_errors;};
		virtual void watch(double v)
			{
			total+=v;
			++count;
			}
		
	};

int main(int argc,char** argv)
	{
	MathMean app;
	return app.main(argc,argv);
	}

