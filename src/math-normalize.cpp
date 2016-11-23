#include <vector>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <inttypes.h> 
#include "math-normalize.tab.h"
#include "abstractmathapp.hh"
using namespace std;


struct Point {
	unsigned int index;
	double value;
	bool operator < (const Point& o) const
		{
		return value < o.value;
		}
	bool operator == (const Point& o) const
		{
		return value == o.value;
		}
	};

static bool sort_on_index (const Point& i,const Point& j) {
	return (i.index < j.index);
	}

class MathNormalize: public AbstractMath<MathNormalizeBase>
	{
	public:
		std::vector<Point> points;
		MathNormalize() {}
		virtual ~MathNormalize() {}
		virtual int main(int argc,char** argv) {
			int optind = parseOptions(argc,argv);
			
			if(on_average && on_median) {
				cerr << "both option median/average defined." << endl;
				return EXIT_FAILURE;
				}
			
			this->fManyOrStdin(optind,argc,&argv[optind]);
			
			if(points.empty())
				{
				cerr << "No Data" << endl;
				return EXIT_FAILURE;
				}
			
			
			if(on_average || on_median)
			 	{
				double pivot=0.0;
				if(on_median)
					{
					::sort(points.begin(),points.end());
					std::vector<Point>::size_type mid = points.size() /2;
					if( points.size() % 2 == 0)
						{
						pivot = (points[ mid-1 ].value + points[ mid ].value )/2.0 ;
						}
					else
						{
						pivot =  points[ mid ].value;
						}
					::sort(points.begin(),points.end(), sort_on_index);
					}
				else
					{
					double total=0.0;
					for(std::vector<Point>::iterator i= points.begin();i!=points.end();++i)
						{
						total += (*i).value;
						}
					pivot = total / points.size();
					}
			 
				for(std::vector<Point>::iterator i= points.begin();i!=points.end();++i)
						{
						(*i).value -= pivot;
						}
				}
			
			double min_value = std::numeric_limits<double>::max();
			double max_value = -min_value;

			for(std::vector<Point>::iterator i= points.begin();i!=points.end();++i)
					{
					min_value = std::min(i->value,min_value);
					max_value = std::max(i->value,max_value);
					}
			
		
						
			for(std::vector<Point>::iterator i= points.begin();i!=points.end();++i)
					{
					double v = (i->value - min_value) / (max_value - min_value);
					cout << (v) << endl;
					}
			
			
			return EXIT_SUCCESS;
			}
	protected:
		virtual bool error_is_fatal() { return !ignore_errors;};
		virtual void watch(double v)
			{
			Point pt;
			pt.index = points.size();
			pt.value = v;
			points.push_back(pt);
			}
		
	};

int main(int argc,char** argv)
	{
	MathNormalize app;
	return app.main(argc,argv);
	}

