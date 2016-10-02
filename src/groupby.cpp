#include <map>
#include "stringsplitter.hh"
#include "aggregators.hh"
#include "groupby.tab.h"

using namespace std;

class GroupBy: public GroupByBase
	{
	public:
		GroupBy() {}
		virtual ~GroupBy() {}
		
		virtual void processIstream(const char* fname,istream& in) {
			StringSplitter splitter;
			splitter.set_delimiter(this->delimiter);
			map<string,vector<Aggregator*>*> key2row;
			std::string line;
			std::vector<std::string> tokens;
			while(getline(in,line)) {
				splitter.split(line,tokens);
				if( tokens.size()!=2) {
					THROW("expected only two columns in " << line);
					}
				std::map<string,vector<Aggregator*>*>::iterator rm = key2row.find(tokens[0]);
				vector<Aggregator*>* row;
				if(rm == key2row.end()) {
					row = new vector<Aggregator*>();
					for(set<int>::iterator ra = this->aggregators.begin();ra!=this->aggregators.end();++ra)
						{
						Aggregator* ag;
						switch(*ra)
							{
							case E_aggregators_first :ag = new FirstAggregator(); break;
							case E_aggregators_sum :ag =  new SumAggregator();break;
							case E_aggregators_mean :ag =  new MeanAggregator();break;
							case E_aggregators_min :ag =  new MinAggregator();break;
							case E_aggregators_max :ag =  new MaxAggregator();break;
							case E_aggregators_set :ag =  new SetAggregator();break;
							case E_aggregators_median :ag =  new MedianAggregator();break;
							default:THROW(*ra);break;
							}
						row->push_back(ag);
						}
					key2row.insert(make_pair(tokens[0],row));
					}
				else
					{
					row = rm->second;
					}
				
				for(size_t i=0;i< row->size();++i)
					{
					row->at(i)->visit(tokens[1]);
					}
				
				}
			for(std::map<string,vector<Aggregator*>*>::iterator r = key2row.begin();
				r!=key2row.end();
				++r)
				{
				cout << r->first;
				vector<Aggregator*>* row = r->second;
				for(size_t i=0;i< row->size();++i)
					{
					cout << this->delimiter;
					row->at(i)->print(cout);
					delete row->at(i);
					}
				cout << endl;
				delete row;
				}
			}
		
		int main(int argc,char** argv)
			{
			int optind = parseOptions(argc,argv);
			oneOrStdin(optind,argc,argv);
			return EXIT_SUCCESS;
			}
	};


int main(int argc,char** argv)
	{
	GroupBy app;
	return app.main(argc,argv);
	}
		
