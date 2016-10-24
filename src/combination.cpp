
#include <cmath>
#include "combination.tab.h"


using namespace std;


class Combination: public CombinationBase {
public:

	Combination() {
		}
	
	virtual void run(int optind,int argc,char** argv,std::vector<string*>& tokens) {
		if((int)tokens.size()==argc-optind)
			{
			for(std::vector<string*>::size_type i=0;i< tokens.size();++i)
				{
				if(i>0) cout << delimiter;
				cout << *(tokens[i]);
				}
			cout << endl;
			return;
			}
		std::vector<string*>::size_type level=tokens.size();
		ifstream* fin = 0;
		istream* in = 0;
		char* filename = argv[optind+level];
		if(strcmp(filename,"-")==0)
			{
			if(!tokens.empty()) THROW("can read \"-\" only if it is first file.");
			in=&cin;
			}
		else
			{
			fin  = new ifstream(filename,ios::in);
			if(!fin->is_open())
				{
				THROW("Cannot open "<<filename << ":" << strerror(errno));
				}
			in = fin;
			}
		
		string line;
		tokens.push_back(0);
		while(getline(*in,line))
			{
			if(ltflag && level>0 && !(*(tokens[level-1]) < line)) continue;
			if(leflag && level>0 && !(*(tokens[level-1]) <= line)) continue;
			
			tokens[level]=&line;
			run(optind,argc,argv,tokens);
			}
		tokens.pop_back();
		if(fin!=0) fin->close();
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		if(ltflag && leflag) {
			cerr << "ltflag and leflag can't be both defined." << endl;
			return EXIT_FAILURE;
			}
		if(optind==argc) {
			cerr << "Illegal number of arguments. Use '-' for stdin." << endl;
			return EXIT_FAILURE;
			}
		std::vector<string*> tokens;
		run(optind,argc,argv,tokens);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	Combination app;
	return app.main(argc,argv);
	}
