#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "json2xml.tab.h"
#include "json2xml.bison.h"
#include "json2xml.flex.h"

using namespace std;

extern int json2xmlparse();
xmlTextWriterPtr xmlWriter =NULL;

class JsonToXml: public JsonToXmlBase {
public:

	virtual void processFILE(const char* f,std::FILE* in) {
		json2xmlin = in;
		xmlWriter = ::xmlNewTextWriterFilename("-",0);
		if( indent ) {
				xmlTextWriterSetIndent(xmlWriter,1);	
				}

		::xmlTextWriterStartDocument(xmlWriter,"1.0","UTF-8",NULL);
		json2xmlparse();
		::xmlTextWriterEndDocument(xmlWriter);
		::xmlFreeTextWriter(xmlWriter);
		xmlWriter=0;
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		fOneOrStdin(optind,argc,argv);
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	LIBXML_TEST_VERSION
	JsonToXml app;
	
	app.main(argc,argv); 
		
	}
