#include <set>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "waitforfiles.tab.h"


using namespace std;

class WaitForFiles: public WaitForFilesBase {
public:

	WaitForFiles() {
		}
	
	virtual int main(int argc,char** argv)
		{
		
		int optind = parseOptions(argc,argv);
		if( this->timeout <-1L) {
			cerr << "Bad value of timeout: "<<  this->timeout << endl;
			return EXIT_FAILURE;
			}
		std::set<std::string> files;
		while(optind < argc) {
			string fname(argv[optind]);
			files.insert(fname);
			++optind;
		}
		
		if( this->listfile != 0) {
			ifstream fin(this->listfile,ios::in);
			if(!fin.is_open())
				{
				THROW("Cannot open "<<this->listfile << ":" << strerror(errno));
				}
			string line;
			while(getline(fin,line))
				{
				if(line.empty() || line[0]=='#') continue;
				files.insert(line);
				}
			fin.close();
			}

		
		
		clock_t start= ::clock();
		for(;;) {
			if( this->timeout !=-1L ) {
					clock_t now = ::clock();
					if (this->timeout * CLOCKS_PER_SEC > (now -start) ) {
					cerr << argv[0] << " : Timeout readched"<<endl;
					return EXIT_FAILURE;
					}
			}
		
		
			int n=0;
			for(std::set<std::string>::iterator r= files.begin();
			r!=files.end();
			++r) {
				struct stat buf;
				if( stat(r->c_str(),&buf) != 0) {
					continue;
					}
				++n;
				}
			if(n==(int)files.size()) {
				for(std::set<std::string>::iterator r= files.begin();
				r!=files.end();
				++r) {
				cout << *r << endl;
				}
			return EXIT_SUCCESS;
			}
		::sleep(this->wait);
		}
		
		return EXIT_FAILURE;
		}
};

int main(int argc,char** argv)
	{
	WaitForFiles app;
	return app.main(argc,argv);
	}
