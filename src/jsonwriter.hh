#ifndef JSONWRITER_HH
#define JSONWRITER_HH

#include <iostream>
#include <vector>

#define JSON_WRITER_IN_VOID 0
#define JSON_WRITER_IN_OBJECT 1
#define JSON_WRITER_IN_ARRAY 2
#include "throw.hh"

class JsonWriter
	{
	private:
		std::ostream& out;
		struct State
			{
			int in_what;
			long children;
			long keyset;
			long valueset;
			State(int i):in_what(i),children(0L),keyset(0L),valueset(0L) {
				}
			bool empty() { return children==0;}
			};
		std::vector<State> states;
		
		void comma() {
			if(states.empty()) {
				THROW("illegal state in JSON Writer (states.empty )");
				}
			State& last = states.back();
			switch(last.in_what)
				{
				case JSON_WRITER_IN_VOID:
					{
					if(!last.empty() ) THROW("illegal state in JSON Writer (primitive already writtern )");
					last.children++;
					break;
					}
				case JSON_WRITER_IN_OBJECT:
					{
					if(last.children!=last.keyset )  THROW("illegal state in JSON Writer (object key was not set)");
					if(last.keyset != 1+last.valueset )  THROW("illegal state in JSON Writer (object value was already set)");
					last.valueset++;
					break;
					}
				case JSON_WRITER_IN_ARRAY:
					{
					if(!last.empty()) out << ",";
					last.children++;
					break;
					}
				default: THROW("illegal state");break;
				}
			
			}
		void quote(const char* s)
			{
			out << "\"";
			for(unsigned long i=0;s[i]!=0;++i)
				{
				switch(s[i])
					{
					case '\n': out << "\\n"; break;
					case '\r': out << "\\r"; break;
					case '\t': out << "\\t"; break;
					case '\'': out << "\\\'"; break;
					case '\"': out << "\\\""; break;
					case '\\': out << "\\\\"; break;
					default: out << s[i]; break;
					}
				}
			out << "\"";
			}
	public:
		JsonWriter(std::ostream& out):out(out) {
			State st(JSON_WRITER_IN_VOID);
			states.push_back(st);
			}
		
		
		
		JsonWriter& writeStartArray() {
			comma();
			State st(JSON_WRITER_IN_ARRAY);
			states.push_back(st);
			out << "[";
			return *this;
			}
		JsonWriter& writeEndArray() {
			if(states.empty()) THROW("illegal state in JSON Writer (empty stack)");
			State& st = states.back();
			if(st.in_what!=JSON_WRITER_IN_ARRAY) THROW("illegal state in JSON Writer (not in array)");
			out << "]";
			states.pop_back();
			return *this;
			}
		JsonWriter& writeStartObject() {
			comma();
			State st( JSON_WRITER_IN_OBJECT);
			states.push_back(st);
			out << "{";
			return *this;
			}
		JsonWriter& writeEndObject() {
			if(states.empty()) THROW("illegal state in JSON Writer (empty stack)");
			State& st = states.back();
			if(st.in_what!=JSON_WRITER_IN_OBJECT) THROW("illegal state in JSON Writer (not in object)");
			if(st.keyset && !st.valueset) THROW("illegal state in JSON Writer (object: key set missing value)");
			out << "}";
			states.pop_back();
			return *this;
			}
		JsonWriter& writeNull() {
			comma();
			out << "null";
			return *this;
			}
		
		JsonWriter& writeString(const char* s) {
			comma();
			quote(s);
			return *this;
			}
		
		JsonWriter& writePropertyName(const char* s)
			{
			
			if(states.empty()) THROW("illegal state in JSON Writer (empty stack)");
			State& st = states.back();
			if(st.in_what!=JSON_WRITER_IN_OBJECT) THROW("illegal state in JSON Writer (not in object)");
			if(st.keyset!=st.children) THROW("illegal state in JSON Writer (key defined badly)");
			if(st.keyset!=st.valueset) THROW("illegal state in JSON Writer (value missing)");
			if(!st.empty()) out << ",";
			st.children++;
			quote(s);
			out << ":";
			st.keyset++;
			return *this;
			}
		JsonWriter& writeDouble(long double f)
			{
			comma();
			out << f;
			return *this;
			}
		JsonWriter& writeInteger( long long int f)
			{
			comma();
			out << f;
			return *this;
			}
		JsonWriter& writeBool(bool b)
			{
			comma();
			out << (b?"true":"false");
			return *this;
			}
		JsonWriter& writePrimitive(const char* s)
			{
			if(s==0 || strcmp(s,"null")==0) return writeNull();
			if(std::strcmp(s,"true")==0) return writeBool(true);
			if(std::strcmp(s,"false")==0) return writeBool(false);
			char* p2;
			std::strtoll(s,&p2,10);
			if(p2!=0 && *p2==0 ) {
				comma();
				out << s;
				return *this;
				}
			std::strtold(s,&p2);
			if(p2!=0 && *p2==0 )  {
				comma();
				out << s;
				return *this;
				}
			return writeString(s);
			}
		
		void close() {
			while(!states.empty())
				{
				switch(states.back().in_what)
					{
					case JSON_WRITER_IN_OBJECT: writeEndObject(); break;
					case JSON_WRITER_IN_ARRAY: writeEndArray(); break;
					default: break;
					}
				states.pop_back();
				}
			}
		
	
	};
	
#undef JSON_WRITER_IN_VOID
#undef JSON_WRITER_IN_OBJECT
#undef JSON_WRITER_IN_ARRAY 

#endif

