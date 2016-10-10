#include <vector>
#include <algorithm>
#include <cstdlib>
#include <inttypes.h> 
#include "math-median.tab.h"
#include "abstractmathapp.hh"
using namespace std;

class MathMedian: public AbstractMath<MathMedianBase>
	{
	public:
		std::vector<double> values;
		MathMedian() {}
		virtual ~MathMedian() {}
		virtual int main(int argc,char** argv) {
			int optind = parseOptions(argc,argv);
			this->fManyOrStdin(optind,argc,&argv[optind]);
			if(values.empty())
				{
				cerr << "No Data" << endl;
				return EXIT_FAILURE;
				}
			::sort(values.begin(),values.end());
			std::vector<double>::size_type mid = values.size() /2;
			if( values.size() % 2 == 0)
				{
				cout << (values[ mid-1 ] + values[ mid ] )/2.0 << endl;
				}
			else
				{
				cout << values[ mid ] << endl;
				}
			return EXIT_SUCCESS;
			}
	protected:
		virtual bool error_is_fatal() { return !ignore_errors;};
		virtual void watch(double v)
			{
			values.push_back(v);
			}
		
	};

int main(int argc,char** argv)
	{
	MathMedian app;
	return app.main(argc,argv);
	}

