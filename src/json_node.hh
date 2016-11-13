#ifndef JSON_TREE_NODE_HH
#define JSON_TREE_NODE_HH
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <iostream>
#include "throw.hh"

class JsonElement
	{
	public:
		union {
			std::string* str;
			bool b;
			std::vector<JsonElement*>* array;
			std::map<std::string,JsonElement*>* object;
			} uval;
	
		enum types {STRING,NUMBER,BOOL,ARRAY,NIL,OBJECT};
		types type;
		
		
	private:
		void bad_type() const {
			THROW("bad type");
			}
		void _print(std::ostream& out,const std::string* s) const
			{
			out << "\"";
			for(std::string::size_type i=0;i< s->size();++i)
				{
				char c = s->at(i);
				switch(c)
					{
					case '\n': out << "\\n"; break;
					case '\t': out << "\\t"; break;
					case '\r': out << "\\r"; break;
					case '\\': out << "\\\\"; break;
					case '\"': out << "\\\""; break;
					default: out << c; break;
					}
				}
			out << "\"";
			}
	public:

		JsonElement() {
			}
		virtual ~JsonElement() {
			switch(type)
				{
				case NUMBER: delete uval.str; break;
				case STRING: delete uval.str; break;
				case ARRAY: delete uval.array; break;
				case OBJECT: delete uval.object; break;
				default : break;
				}
			}
		bool isA(types t) const { return type == t;}
		bool isBoolean() const { return isA(BOOL);}
		bool isString() const { return isA(STRING);}
		bool isArray() const { return isA(ARRAY);}
		bool isObject() const { return isA(OBJECT);}
		bool isNumber() const { return isA(NUMBER);}
		bool isNull() const { return isA(NIL);}
		bool isTrue() const { return isBoolean() && uval.b;}
		bool isFalse() const { return isBoolean() && !uval.b;}
		bool isComplex() const { return isArray() || isObject();}
		bool isPrimitive() const { return !isComplex();}		
		
		int intValue() const {
			if(!isNumber()) return false;
			
			return 0;
			}
		
		double doubleValue() const {
			if(!isNumber()) THROW("boum");
			char* p2;
			double d = std::strtod(uval.str->c_str(),&p2);
			return d;
			}
		float floatValue() const {
			if(!isNumber()) THROW("boum");
			char* p2;
			float f = std::strtof(uval.str->c_str(),&p2);
			return f;
			}
		
		long int longValue() const {
			if(!isNumber()) THROW("boum");
			char* p2;
			long int d = std::strtol(uval.str->c_str(),&p2,10);
			return d;
			}
			
		std::size_t size()  const {
			switch(type)
				{
				case STRING: return uval.str->size(); break;
				case ARRAY: return uval.array->size(); break;
				case OBJECT: return uval.object->size(); break;
				default : bad_type(); break;
				}
			}
		
		std::string& str() {
			if( !isA(STRING) ) bad_type();
			return *uval.str;
			}

		JsonElement* get(std::vector<JsonElement*>::size_type idx) {
			if( !isA(ARRAY) ) return 0;
			return uval.array->at(idx);
			}
		
		JsonElement* get(const char* key) {
			if( !isA(OBJECT) ) return 0;
			std::map<std::string,JsonElement*>::iterator r = uval.object->find(key);
			if( r == uval.object->end()) return 0;
			return r->second;
			}
		
		void print(std::ostream& out) const
			{
			switch(type)
				{
				case STRING: _print(out,uval.str); break;
				case NIL:  out << "null"; break;
				case BOOL: out << (uval.b?"true":"false"); break;
				case NUMBER: out << *uval.str; break;
				case ARRAY:
					out << "[";
					for(std::size_t i=0;i< uval.array->size();++i)
						{
						if(i>0) out << ",";
						uval.array->at(i)->print(out);
						}
					out << "]";
					break;
				case OBJECT:
					out << "{";
					for(std::map<std::string,JsonElement*>::const_iterator i = uval.object->begin();
						i != uval.object->end();
						++i
						)
						{
						if(i != uval.object->begin()) out << ",";
						_print(out,&(i->first));
						out <<":";
						i->second->print(out);
						}
					out << "}";
					break;
				default : bad_type(); break;
				}
			}
		
		static JsonElement* createString(const char* s);
		static JsonElement* createNumber(const char* s);
		static JsonElement* createTrue();
		static JsonElement* createFalse();
		static JsonElement* createBool(bool b);
		static JsonElement* createNull();
		static JsonElement* createArray();
		static JsonElement* createObject();
		static JsonElement* parse(std::FILE* );
		static JsonElement* parse(const char* );
	};




#endif

