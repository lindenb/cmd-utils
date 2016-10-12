#include <list>
#include "headtail.tab.h"


using namespace std;

class HeadTail: public HeadTailBase {
public:
	long nlines;
	std::list<std::string> buffer;
	HeadTail():nlines(0) {
		}
	
	virtual void processIstream(const char* f,std::istream& in) {
		string line;
		while(getline(in,line)) {
			this->nlines++;
			if(nlines <= this->headcount )
				{
				cout << line << endl;
				continue;
				} 
			buffer.push_back(line);
			while((long)this->buffer.size() > this->tailcount)
				{
				buffer.pop_front();
				}
			}
		
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		if( this->headcount <=0L) {
			cerr << "Bad value of headcount: "<<  this->headcount << endl;
			return EXIT_FAILURE;
			}
		if( this->tailcount <=0L) {
			cerr << "Bad value of tailcount: "<<  this->tailcount << endl;
			return EXIT_FAILURE;
			}
		nlines=0L;
		oneOrStdin(optind,argc,argv);
		if(!buffer.empty()) {
			if(!group_separator.empty()) cout << group_separator << endl;
			for(std::list<std::string>::iterator r = buffer.begin(); r!=buffer.end();++r)
				{
				cout << (*r) << endl;
				}
			}
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	HeadTail app;
	return app.main(argc,argv);
	}
