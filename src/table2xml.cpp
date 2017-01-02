#include <algorithm>
#include <memory>
#include <map>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "config.h"
#include "stringsplitter.hh"
#include "table2xml.tab.h"


using namespace std;

class TableToXml: public TableToXmlBase
	{
	public:
		TableToXml() {}
		virtual ~TableToXml() {}
		
		virtual void processIstream(const char* fname,istream& in) {
			xmlTextWriterPtr w = ::xmlNewTextWriterFilename("-",0);
			if( indent ) {
				xmlTextWriterSetIndent(w,1);	
				}
			if( !this->hidexmlheader)
				{
				xmlTextWriterStartDocument(w,"1.0","UTF-8",NULL);
				}
			xmlTextWriterStartElementNS(w,NULL,BAD_CAST "table",BAD_CAST xmlns);
				
			StringSplitter splitter;
			splitter.set_delimiter(this->delimiter);
			vector<std::string> header;
			string line;
			
			if( !ignoreheader)
				{
				if(!getline(in,line)) {
					THROW("Cannot read 1st line");
					}
				splitter.split(line,header);
				::xmlTextWriterStartElement(w,BAD_CAST "thead");
				::xmlTextWriterStartElement(w,BAD_CAST "tr");
				for(vector<std::string>::size_type i=0;
						i < header.size();
						++i) {
						::xmlTextWriterStartElement(w,BAD_CAST "th");
						::xmlTextWriterWriteFormatAttribute(w,BAD_CAST "column","%d",(i+1));
						::xmlTextWriterWriteString(w,BAD_CAST header[i].c_str());
						::xmlTextWriterEndElement(w);
						}
				::xmlTextWriterEndElement(w);
				::xmlTextWriterEndElement(w);
				}
			
			::xmlTextWriterStartElement(w,BAD_CAST "tbody");
			if( !indent && print_nl_row )
				{
				::xmlTextWriterWriteString(w,BAD_CAST "\n");
				}
			int nLine=0;
			while(getline(in,line)) {
				++nLine;
				vector<std::string> tokens;
				splitter.split(line,tokens);
						
				::xmlTextWriterStartElement(w,BAD_CAST "tr");
				::xmlTextWriterWriteFormatAttribute(w,BAD_CAST "row","%d",nLine);
				for(vector<std::string>::size_type i=0;
					i < tokens.size();
					++i) {
					if( useheadername && !ignoreheader && i< header.size())
						{
						::xmlTextWriterStartElement(w,BAD_CAST header[i].c_str());
						}
					else
						{
						::xmlTextWriterStartElement(w,BAD_CAST "td");
						}
					if( (int)tokens[i].size() < cdatalen || (tokens[i].find_first_of("&<>\"'")==string::npos)) {
						::xmlTextWriterWriteString(w,BAD_CAST tokens[i].c_str());
						}
					else
						{
						::xmlTextWriterStartCDATA(w); 
						::xmlTextWriterWriteString(w,BAD_CAST tokens[i].c_str());
						::xmlTextWriterEndCDATA(w); 
						}
					::xmlTextWriterEndElement(w);
					}
				::xmlTextWriterEndElement(w);
				if(  !indent && print_nl_row )
					{
					::xmlTextWriterWriteString(w,BAD_CAST "\n");
					}
				}
			::xmlTextWriterEndElement(w);

			::xmlTextWriterStartElement(w,BAD_CAST "tfoot");
			::xmlTextWriterWriteFormatAttribute(w,BAD_CAST "rows","%d",nLine);
			::xmlTextWriterEndElement(w);
			
			::xmlTextWriterEndElement(w);
			::xmlTextWriterEndDocument(w);
			::xmlFreeTextWriter(w);
			}

		
		virtual int main(int argc, char** argv) {
			int optind = parseOptions(argc,argv);
			oneOrStdin(optind,argc,argv);
			return EXIT_SUCCESS;
			}
	};


int main(int argc,char** argv)
	{
	LIBXML_TEST_VERSION
	TableToXml app;
	return app.main(argc,argv);
	}

