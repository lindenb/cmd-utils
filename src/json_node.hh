#ifndef JSON_TREE_NODE_HH
#define JSON_TREE_NODE_HH
#include <cstdio>
#include <string>
#include <vector>
#include <map>

class JsonElement
	{
	public:
		union {
			std::string* str;
			bool b;
			std::vector<JsonElement*>* array;
			std::map<std::string,JsonElement*>* object;
			} uval;
	
		enum types {STRING,NUMBER,BOOL,ARRAY,OBJECT,NIL,OBJECT};
		types type;
		
		
	private:
		void bad_type() {
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
			std::map<std::string,JsonElement*>::iterator r = uval.array->find(key);
			if( r == uval.array->end()) return 0;
			return r->second;
			}
		
		static JsonElement* createString(const  const char* s);
		static JsonElement* createNumber(const  const char* s);
		static JsonElement* createTrue();
		static JsonElement* createFalse();
		static JsonElement* createBool(bool b);
		static JsonElement* createNull();
		static JsonElement* createArray();
		static JsonElement* createObject();
		static JsonElement* parse(std::FILE* );
	};




#endif

