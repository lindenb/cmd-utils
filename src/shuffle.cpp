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
				cerr << "read rand=" << r << endl;
				string::size_type n;
				fread((void*)&n,sizeof(string::size_type),1,in);
				cerr << "read n="<< n << endl;
				string line(n,'\0');
				fread((void*)line.data(),sizeof(char),n,in);
				return new Line(r,line);
				}
			virtual void write(FILE* out,const Line* line) {
				cerr << "w r=" << line->random << endl;
			 	fwrite((void*)&(line->random),sizeof(int),1,out);
			 	string::size_type n = line->line.size();
			 	cerr << "w n=" << n << endl;
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
		
		while(getline(in,line))
			{
			int r= ::rand();
			Line* l =new Line(r,line);
			sorting.add(l);
			}
		SortingCollection<Line>::Iterator* iter = sorting.iterator();
		for(;;)
			{
			cerr << "loop1 " << endl;
			Line* l = iter->next();
			cerr << "loop2 " << endl;
			if( l == 0 ) break;
			cerr << "loop3 " << l->line.size() << endl;
			cerr << l->line << endl;
			delete l;
			}
		cerr << "done" << endl;
		iter->close();
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		unsigned int seed=0UL;
		::srand(seed);
		oneOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	Shuffle app;
	return app.main(argc,argv);
	}
