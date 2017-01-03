
#include "tojson.tab.h"
#include "jsonwriter.hh"
#include "stringsplitter.hh"

using namespace std;

class ToJson: public ToJsonBase {
public:

	ToJson() {
		}
	
	virtual void write(JsonWriter& out,const string& s)
		{
		if(smart_primitives)
			{
			out.writePrimitive(s.c_str());
			}
		else
			{
			out.writeString(s.c_str());
			}
		}
	
	virtual void processIstream(const char* f,std::istream& in) {
		string line;
		if(beforestr!=0) cout << beforestr;
		JsonWriter out(cout);
		
		
		
		if(input_type==NULL || strcmp(input_type,"array")==0)
			{
			out.writeStartArray();
			while(getline(in,line)) {
				write(out,line);
				}
			out.writeEndArray();
			
			}
		else if(strcmp(input_type,"object")==0)
			{
			out.writeStartObject();
			while(getline(in,line)) {
				
				string::size_type i= line.find(this->delimiter);
				if(i==string::npos) THROW("delimiter missing in "<< line);
				string key(line.substr(0,i));
				string value(line.substr(i+1));
				
				
				out.writePropertyName(key.c_str());
				write(out,value);
				
				}
			out.writeEndObject();
			}
		else   if(strcmp(input_type,"table")==0)
			{
			StringSplitter splitter;
			splitter.set_delimiter(this->delimiter);
			string line;
			if(!getline(in,line)) THROW("cannot read first line");
			vector<string> header = splitter.split(line);
			
			vector<string> tokens;
			out.writeStartArray();
			while(getline(in,line)) {
				splitter.split(line,tokens);
				if(tokens.size()>header.size()) {
					THROW("too many columns in "<< line);
					}
				out.writeStartObject();
				for(string::size_type i= 0 ; i< header.size()  && i< tokens.size();++i)
					{
					
					out.writePropertyName(header[i].c_str());
					write(out,tokens[i]);
					
					}
				out.writeEndObject();
				}
			out.writeEndArray();
			}
		else
			{
			THROW("Illegal type of input");
			}
		
		out.close();
		if(afterstr!=0) cout << afterstr;
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		oneOrStdin(optind,argc,argv);
		
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	ToJson app;
	return app.main(argc,argv);
	}
