%{ 
#include <string>
#include "throw.hh"
#include "json_node.hh"
#include "json_tree.flex.h"

using namespace std;

static void yyerror(const char* msg)
	{
	cerr << "ERROR " << msg << endl;
	exit(EXIT_FAILURE);
	}

struct json_pair_t
	{
	std::string* k;
	JsonElement* v;
	};

static JsonElement* CURRENT_NODE = 0;
%}

%union {
	JsonElement* element;
	std::string* str;
	void* kv;
	}

%token ARRAY_OPEN
%token ARRAY_CLOSE
%token OBJECT_OPEN
%token OBJECT_CLOSE
%token<str> PRIMITIVE_STRING 
%token<element>  PRIMITIVE_TRUE PRIMITIVE_FALSE PRIMITIVE_NUMBER PRIMITIVE_NULL
%token COMMA COLON
%start input
%type<element> any array array_items object  object_items
%type<str> prim_string
%type<kv> object_pair
%%

input: any {
	CURRENT_NODE = $1;
	};

array: ARRAY_OPEN  ARRAY_CLOSE { $$ = JsonElement::createArray();}
	| ARRAY_OPEN array_items ARRAY_CLOSE {$$=$2;} 
	;
array_items : array_items COMMA any  {$$=$1; $$->uval.array->push_back($3);} | any { $$=JsonElement::createArray(); $1->uval.array->push_back($1); };
object:
	OBJECT_OPEN OBJECT_CLOSE { $$= JsonElement::createObject();} |
	OBJECT_OPEN object_items OBJECT_CLOSE { $$ = $2;}
	;
object_items: object_pair { json_pair_t* kv=(json_pair_t*)$1; $$ = JsonElement::createObject(); $$->uval.object->insert(make_pair(*(kv->k),kv->v)); delete kv->k; delete kv;} |
	object_items COMMA object_pair  {json_pair_t* kv=(json_pair_t*)$3; $$=$1; $$->uval.object->insert(make_pair(*(kv->k),kv->v)); delete kv->k; delete kv;}
	;
object_pair: prim_string  COLON  any
	{
	json_pair_t* kv = new json_pair_t;
	kv->k = $1;
	kv->v = $3; 
	$$ = (void*)kv;
	};
any: prim_string { $$ = JsonElement::createString($1->c_str()); delete $1; } |
	PRIMITIVE_NULL {$$= $1; } |
	PRIMITIVE_NUMBER { $$ =  $1; } |
	PRIMITIVE_TRUE {$$= $1;}  |
	PRIMITIVE_FALSE {$$= $1;} |
	array {$$=$1;} |
	object {$$=$1;};
prim_string: PRIMITIVE_STRING { $$ = $1;}


%%

JsonElement* JsonElement::createString(const char* s) {
	JsonElement* e = new JsonElement;
	e->uval.str = new string(s);
	e->type = JsonElement::STRING;
	return e;
	}

JsonElement* JsonElement::createNumber(const char* s) {
	JsonElement* e = new JsonElement;
	e->uval.str = new string(s);
	e->type = JsonElement::NUMBER;
	return e;
	}

JsonElement* JsonElement::createTrue() {
	return JsonElement::createBool(true);
	}
JsonElement* JsonElement::createFalse() {
	return JsonElement::createBool(false);
	}
JsonElement* JsonElement::createBool(bool b) {
	JsonElement* e = new JsonElement;
	e->uval.b = b;
	e->type = JsonElement::BOOL;
	return e;
	}

JsonElement* JsonElement::createNull() {
	JsonElement* e = new JsonElement;
	e->type = JsonElement::NIL;
	return e;
	}

JsonElement* JsonElement::createArray() {
	JsonElement* e = new JsonElement;
	e->type = JsonElement::ARRAY;
	e->uval.array = new vector<JsonElement*>;
	return e;
	}

JsonElement* JsonElement::createObject() {
	JsonElement* e = new JsonElement;
	e->type = JsonElement::OBJECT;
	e->uval.object = new map<string,JsonElement*>;
	return e;
	}
extern int json_treeparse();
extern FILE* json_treein;
JsonElement*  JsonElement::parse(std::FILE*  in) {
	json_treein = in;
	json_treeparse();
	json_treein = NULL;
	return CURRENT_NODE;
	}
JsonElement*  JsonElement::parse(const char* s) {
	//json_treein = in;
	//json_treeparse();
	//json_treein = NULL;
	return CURRENT_NODE;
	}
