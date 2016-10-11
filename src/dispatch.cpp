#include "dispatch.tab.h"


using namespace std;
#define WORDX "XXXXXX"
class Dispatch: public DispatchBase {
public:
	class Chunk
		{
		public:
			string fname;
			unsigned int count;
			fstream out;
			Chunk(string fname):fname(fname),count(0UL),out(fname.c_str(),ios::out) {
				if(!out.is_open()) THROW("Cannot open "<< fname << " " << strerror(errno));
				}
			~Chunk() {
				out.close();
				}
		};
	virtual void processIstream(const char* f,std::istream& in) {
		vector<Chunk*> chunks;
		vector<unsigned int> counts;
		
		for(int i=0;i< this->splitcount;++i)
			{
			string fname(this->outfilename);
		
			char tmp[20];
			sprintf(tmp,"%06d",(i+1));
			for(;;)
				{
				string::size_type start_pos = fname.find(WORDX);
				if( start_pos==std::string::npos) break;
				fname.replace(start_pos, strlen(WORDX),tmp);
				}
			chunks.push_back(new Chunk(fname));
			}
		unsigned long x=0UL;
		string line;
		while(getline(in,line))
			{
			chunks[ x % chunks.size()]->out << line << endl;
			chunks[ x % chunks.size()]->count++;
			++x;
			}
		
		for(unsigned int i=0;i< chunks.size();++i)
			{
			cout << chunks[i]->fname << "\t" << chunks[i]->count << endl;
			delete chunks[i];
			}
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		if( this->splitcount <=0) {
			cerr << "Bad value of splicount: "<<  this->splitcount << endl;
			return EXIT_FAILURE;
			}
		if( this->outfilename ==NULL) {
			cerr << "Output undefined" << endl;
			return EXIT_FAILURE;
			}
		if( strstr(this->outfilename,WORDX) ==NULL) {
			cerr << "Output cannot find " << WORDX << " in " << this->outfilename << endl;
			return EXIT_FAILURE;
			}
		oneOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	Dispatch app;
	return app.main(argc,argv);
	}
