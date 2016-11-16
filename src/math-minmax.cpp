#include <vector>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <inttypes.h> 
#include "math-minmax.tab.h"
#include "abstractmathapp.hh"
using namespace std;


class MinMax: public AbstractMath<MinMaxBase>
	{
	public:
		double min_value;
		double max_value;
		MinMax():min_value(std::numeric_limits<double>::max()),
					max_value( - std::numeric_limits<double>::max())
			{}
		virtual ~MinMax() {}
		virtual int main(int argc,char** argv) {
			int optind = parseOptions(argc,argv);
			
			if(hide_min && hide_max) {
				cerr << "both option m/M defined." << endl;
				return EXIT_FAILURE;
				}
			
			this->fManyOrStdin(optind,argc,&argv[optind]);
			
			if(min_value>max_value)
				{
				cerr << "No Data" << endl;
				return EXIT_FAILURE;
				}
			
			if(!hide_min) 
				{
				cout << min_value;
				}
			if(!hide_max) 
				{
				if(!hide_min) cout << delimiter;
				cout << max_value;
				}
			cout << endl;
			
			return EXIT_SUCCESS;
			}
	protected:
		virtual bool error_is_fatal() { return !ignore_errors;};
		virtual void watch(double v)
			{
			min_value = std::min(v,min_value);
			max_value = std::max(v,max_value);
			}
		
	};

int main(int argc,char** argv)
	{
	MinMax app;
	return app.main(argc,argv);
	}

