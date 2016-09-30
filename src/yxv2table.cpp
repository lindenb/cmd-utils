#include <fstream>
#include <algorithm>
#include <map>
#include "yxv2table.tab.h"
#include "stringsplitter.hh"

using namespace std;

class Aggregator
	{
	public:
		Aggregator() {}
		virtual ~Aggregator(){}
		virtual void visit(const std::string& s)=0;
		virtual void print(std::ostream& out)=0;
		double strtod(const std::string& s) {
			char* p2=0;
			return std::strtod(s.c_str(),&p2);
			}
	};



typedef std::pair<std::string,std::string> pair_of_str_t;

struct cmpByStringLength {
    bool operator()(const pair_of_str_t& a, const pair_of_str_t& b) const {
         if(  a.first < b.first ) return true;
         if(  a.first > b.first ) return false;
         return a.second < b.second ;
    	}
};




class YXV2Table: public YXV2TableBase
	{
	public:
	
	class SimpleAggregator: public Aggregator
		{
		public:
			std::string value;
			virtual ~SimpleAggregator(){}
			virtual void visit(const std::string& s) { value.assign(s);}
			virtual void print(std::ostream& out) { out << this->value; }
		};
	
	class FirstAggregator: public Aggregator
		{
		public:
			std::string value;
			bool seen;
			FirstAggregator():seen(false){}
			virtual ~FirstAggregator(){}
			virtual void visit(const std::string& s) {if(!seen) value.assign(s);seen=true;}
			virtual void print(std::ostream& out) {  out << this->value; }
		};
	
	class SumAggregator: public Aggregator
		{
		public:
			double total;
			SumAggregator():total(0){}
			virtual ~SumAggregator(){}
			virtual void visit(const std::string& s) {total+=strtod(s);}
			virtual void print(std::ostream& out) {  out << this->total; }
		};
	class MeanAggregator: public Aggregator
		{
		public:
			double total;
			int n;
			MeanAggregator():total(0),n(0){}
			virtual ~MeanAggregator(){}
			virtual void visit(const std::string& s) {total+=strtod(s);++n;}
			virtual void print(std::ostream& out) {  out << (this->total/n); }
		};
	
	class MinAggregator: public Aggregator
		{
		public:
			double value;
			bool first;
			MinAggregator():value(0),first(true){}
			virtual ~MinAggregator(){}
			virtual void visit(const std::string& s) {double d=strtod(s);value=(first || d<value?d:value) ; first=false;}
			virtual void print(std::ostream& out) {  out << value; }
		};
	
	class MaxAggregator: public Aggregator
		{
		public:
			double value;
			bool first;
			MaxAggregator():value(0),first(true){}
			virtual ~MaxAggregator(){}
			virtual void visit(const std::string& s) {double d=strtod(s);value=(first || d>value?d:value) ; first=false;}
			virtual void print(std::ostream& out) {  out << value; }
		};
	
	class SetAggregator: public Aggregator
		{
		public:
			set<string> values;
			SetAggregator() {}
			virtual ~SetAggregator(){}
			virtual void visit(const std::string& s) {values.insert(s);}
			virtual void print(std::ostream& out) {
				bool first(true);
				for(set<string>::iterator r=values.begin();r!=values.end();++r)
					{
					if(!first) out <<";";
					first=false; 
					out << (*r);
					}
				}
		};
	
	class MedianAggregator: public Aggregator
		{
		public:
			vector<double> values;
			MedianAggregator() {}
			virtual ~MedianAggregator(){}
			virtual void visit(const std::string& s) {values.push_back(strtod(s));}
			virtual void print(std::ostream& out) {
				std::sort(values.begin(),values.end());
				unsigned int i=values.size()/2;
				if(values.size()%2==0)
					{
					out << (values[i-1]+values[i])/2.0;
					}
				else
					{
					out << values[i];
					}
				}
		};
	
	Aggregator* build()
		{
		switch(this->aggregator)
			{
			case E_aggregator_first :return new FirstAggregator();
			case E_aggregator_sum :return new SumAggregator();
			case E_aggregator_mean :return new MeanAggregator();
			case E_aggregator_min :return new MinAggregator();
			case E_aggregator_max :return new MaxAggregator();
			case E_aggregator_set :return new SetAggregator();
			case E_aggregator_median :return new MedianAggregator();
			default: return new SimpleAggregator();
			}
		}

	void run(const char* fname,istream& in)
		{
		std::set<std::string> x_headers;
		std::set<std::string> y_headers;
		std::map<pair_of_str_t,Aggregator*,cmpByStringLength> yxvalues;
		std::string line;
		std::vector<std::string> tokens;
		StringSplitter splitter;
		splitter.set_delimiter(this->delimiter);
		while(getline(in,line)) {
			splitter.split(line,tokens);
			pair_of_str_t key = make_pair(tokens[0],tokens[1]);
			std::map<pair_of_str_t,Aggregator*>::iterator rm = yxvalues.find(key);
			Aggregator* ag;
			if( rm == yxvalues.end() )
				{
				ag = build();
				x_headers.insert(tokens[1]);
				y_headers.insert(tokens[0]);
				yxvalues.insert(make_pair(key,ag));
				}
			else
				{
				ag = rm->second;
				}
			ag->visit(tokens[2]);
			}
		
		cout << this->firstheader;
		for(std::set<std::string>::iterator rx= x_headers.begin(); rx!=x_headers.end();++rx)
			{
			cout << this->delimiter << (*rx);
			}
		cout << endl;
	
		for(std::set<std::string>::iterator ry= y_headers.begin(); ry!=y_headers.end();++ry)
			{
			cout << (*ry);
			for(std::set<std::string>::iterator rx= x_headers.begin(); rx!=x_headers.end();++rx)
				{
				pair_of_str_t k = make_pair(*ry,*rx);
				cout << this->delimiter ;//<< k.first+"/"+k.second<<":";
				std::map<pair_of_str_t,Aggregator*>::iterator rm = yxvalues.find(k);
				if( rm == yxvalues.end()) {
					cout << this->nullvalue;
					}
				else
					{
					rm->second->print(cout);
					delete rm->second;
					}
			
				}
			cout << endl;
			}
	
		}
	
	int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		
		if(optind==argc)
			{
			run(0,cin);
			}
		else if(optind+1==argc)
			{
			ifstream f(argv[optind]);
			if(!f.is_open()) {
				cerr << "Cannot open " << argv[optind] << ": "
					<< strerror(errno) << "." << endl;
				return EXIT_FAILURE;
				}
			run(argv[optind],f);
			f.close();
			}
		else
			{
			cerr << "Illegal Number of arguments"<< endl;
			return EXIT_FAILURE;
			}
		return EXIT_SUCCESS;
		}

	};


int main(int argc,char** argv)
	{
	YXV2Table app;
	return app.main(argc,argv);
	}
		

	
