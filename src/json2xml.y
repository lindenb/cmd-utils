%{ 
#include <cassert>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <string>
#include <vector>
#include "throw.hh"
#include "json2xml.flex.h"

using namespace std;
extern xmlTextWriterPtr xmlWriter;

struct State
	{
	char type;
	string* key;
	};

#define JSONX  "http://www.ibm.com/xmlns/prod/2009/jsonx"



static void yyerror(const char* msg)
	{
	cerr << "ERROR " << msg << endl;
	exit(EXIT_FAILURE);
	}

static vector<State> states;


static void WRITE_STR(std::string* s)
	{
	for(size_t i=1;i+1<s->size();++i)
		{
		switch(s->at(i))
			{
			case '\\':
				{
				if( i+2 <s->size())//+2 : quote
					{
					switch(s->at(i+1))
						{
						case 'n': ::xmlTextWriterWriteString(xmlWriter,BAD_CAST "\n");break;
						case 't': ::xmlTextWriterWriteString(xmlWriter,BAD_CAST "\t");break;
						case '\\': ::xmlTextWriterWriteString(xmlWriter,BAD_CAST "\\");break;
						case '\"': ::xmlTextWriterWriteString(xmlWriter,BAD_CAST "\"");break;
						case '\'': ::xmlTextWriterWriteString(xmlWriter,BAD_CAST "\'");break;
						default:break;
						}
					
					
					i++;
					}
				break;
				}
			default:
				{
				char tmp[2]={s->at(i),0};
				::xmlTextWriterWriteString(xmlWriter,BAD_CAST tmp);
				break;
				}
			}
		}
	}


static void  START_ELEMENT(const char* name)
	{

	::xmlTextWriterStartElementNS(xmlWriter,BAD_CAST NULL,BAD_CAST name,BAD_CAST (states.empty()?JSONX:NULL) );
	if(!states.empty())
		{
		State& st = states.back();
		
		if(st.type=='o' && st.key!=0) {
			::xmlTextWriterStartAttribute(xmlWriter,BAD_CAST "name");
			WRITE_STR(st.key);
			::xmlTextWriterEndAttribute(xmlWriter);
			delete st.key;
			st.key=0;
			}
		}
	State st;
	st.type = name[0];
	st.key = 0;
	states.push_back(st);
	}


static void  END_ELEMENT()
	{
	::xmlTextWriterEndElement(xmlWriter);\
	
	states.pop_back();
	}


%}

%union {
	std::string* str;
	}

%token ARRAY_OPEN
%token ARRAY_CLOSE
%token OBJECT_OPEN
%token OBJECT_CLOSE
%token<str> PRIMITIVE_STRING  PRIMITIVE_NUMBER PRIMITIVE_TRUE PRIMITIVE_FALSE
%token PRIMITIVE_NULL
%token COMMA COLON
%start input
%%

input: any;

array: ARRAY_OPEN  ARRAY_CLOSE
	{
		START_ELEMENT("array");
		END_ELEMENT();
	}
	| ARRAY_OPEN { START_ELEMENT("array"); } array_items ARRAY_CLOSE { END_ELEMENT();  }
	;
array_items:  array_items COMMA  any | any ;
object: OBJECT_OPEN OBJECT_CLOSE { START_ELEMENT("object"); END_ELEMENT(); } |
	OBJECT_OPEN { START_ELEMENT("object");} object_items OBJECT_CLOSE { END_ELEMENT();};
object_items: object_pair |  object_pair COMMA  object_items;
object_pair: PRIMITIVE_STRING { State& st = states.back(); st.key=$1; } COLON  any;
any:    PRIMITIVE_STRING {START_ELEMENT("string");WRITE_STR($1); END_ELEMENT(); delete $1;} |
	PRIMITIVE_NULL {START_ELEMENT("null");END_ELEMENT();} |
	PRIMITIVE_NUMBER {START_ELEMENT("number"); WRITE_STR($1); END_ELEMENT(); delete $1;} |
	PRIMITIVE_TRUE {START_ELEMENT("boolean"); ::xmlTextWriterWriteString(xmlWriter,BAD_CAST "true"); END_ELEMENT();}  |
	PRIMITIVE_FALSE {START_ELEMENT("boolean"); ::xmlTextWriterWriteString(xmlWriter,BAD_CAST "false"); END_ELEMENT();} |
	array | object;


%%

