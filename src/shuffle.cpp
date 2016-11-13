#include <ctime>
#include "shuffle.tab.h"
#include "sortondisk.hh"

using namespace std;

class Shuffle: public ShuffleBase {
public:
	
	class Line
		{
		public:
			int random;
			std::string line;
			Line(int random,std::string line):random(random),line(line)
				{

				
				}
			
		};
	
	class LineBinding : public SortingCollection<Line>::TupleBinding
		{
		public:
			virtual Line* read(std::FILE* in)  {
				int r;
				fread((void*)&r,sizeof(int),1,in);
				string::size_type n;
				fread((void*)&n,sizeof(string::size_type),1,in);
				string line(n,'\0');
				fread((void*)line.data(),sizeof(char),n,in);
				return new Line(r,line);
				}
			virtual void write(FILE* out,const Line* line) {
			 	fwrite((void*)&(line->random),sizeof(int),1,out);
			 	string::size_type n = line->line.size();
			 	fwrite((void*)&n,sizeof(string::size_type),1,out);
				fwrite((void*)line->line.data(),sizeof(char),n,out);
				}

		};
	
	class LineCmp : public SortingCollection<Line>::Comparator
		{
		public:
			virtual bool lt(const Line* a,const  Line* b)
				{
				return a->random < b->random;
				}
		};
	
	virtual void processIstream(const char* f,std::istream& in)  {
		LineBinding binding;
		LineCmp cmp;
		string line;
		SortingCollection<Line> sorting;
		sorting.set_comparator(&cmp);
		sorting.set_binding(&binding);
		sorting.set_max_itemsinram(this->maxitemsinram);
		size_t prefix_len=(this->ignoreLineStartingWith==0?0UL:strlen(this->ignoreLineStartingWith));
		while(getline(in,line))
			{
			if(prefix_len>0UL && line.size()>=prefix_len && line.compare(0,prefix_len,this->ignoreLineStartingWith,prefix_len)==0) {
				cout << line << endl;
				continue;
				}
			int r= ::rand();
			Line* l =new Line(r,line);
			sorting.add(l);
			}
		SortingCollection<Line>::Iterator* iter = sorting.iterator();
		for(;;)
			{
			Line* l = iter->next();
			if( l == 0 ) break;
			cout << l->line << endl;
			delete l;
			}
		iter->close();
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		if(this->rseed==0)
			{
			srand(time(NULL));
			}
		else
			{
			::srand(this->rseed);
			}
		oneOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	Shuffle app;
	return app.main(argc,argv);
	}
