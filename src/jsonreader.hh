#ifndef JSON_EVT_PARSER_HH
#define JSON_EVT_PARSER_HH

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include "debug.hh"

enum JsonToken
	{
	JSON_BEGIN_ARRAY
	};

#define BOUM(a) {\
	std::ostringstream _os; \
	_os << "[" <<  __FILE__ << ":" << __LINE__ << "] " <<  a ;\
	if(!this->states.empty()) _os << " Stack back = ("<< (int)this->states.back() << ").";\
	std::cerr << _os.str() << std::endl ;\
	throw _os.str();\
	}

#define CHECK_STATE_PRESENT \
	if( this->states.empty() ) \
		{\
		BOUM("State trace is empty"); \
		}

#define CHECK_ANY \
	CHECK_STATE_PRESENT \
	switch( states.back()  ) \
		{\
		case EXPECT_ANY : this->states.pop_back(); break; \
		case ARRAY_EXPECT_ANY_OR_END : this->states.back() = ARRAY_EXPECT_COMMA_OR_END ; break; \
		case ARRAY_EXPECT_ANY : this->states.back() = ARRAY_EXPECT_COMMA_OR_END ; break; \
		case OBJECT_EXPECT_ANY : this->states.back() = OBJECT_EXPECT_COMMA_OR_END ; break; \
		default: BOUM(states.back()); break; \
		}
#define MUST_READ(C) { int c_ = this->in.get(); if (c_ != C ) BOUM("Expected char \""<<(char)C<<"\" but got \"" << (char)c_ << "\"");}
class JsonReader
	{
	public:
	   enum OPCODE {
			OPCODE_BOOLEAN =1,
			OPCODE_NULL = 2,
			OPCODE_ARRAY_START = 3,
			OPCODE_ARRAY_END = 4,
			OPCODE_OBJECT_END = 5,
			OPCODE_OBJECT_START = 6,
			OPCODE_OBJECT_NAME = 7,
			OPCODE_STRING = 8,
			OPCODE_NUMBER = 9
			};
	private:
		enum state_t {
			EXPECT_ANY=0,
			ARRAY_EXPECT_ANY=1,
			ARRAY_EXPECT_ANY_OR_END=2,
			ARRAY_EXPECT_COMMA_OR_END=3,
			OBJECT_EXPECT_NAME=4,
			OBJECT_EXPECT_ANY=5,
			OBJECT_EXPECT_COLON=6,
			OBJECT_EXPECT_NAME_OR_END=7,//{}
			OBJECT_EXPECT_COMMA_OR_END=8
			};

		int opcode;
		std::istream& in;
		std::vector<state_t> states;
		bool eof_met;
		bool curr_bool;
		std::string curr_string;
		
		int read() {
		char c;
		if(!this->in.get(c)) return -1;
		return (int)c;
		}
		
		int skipws() {
		for(;;)
			{
			int c = this->read();
			if(c==-1) return -1;
			if(!std::isspace(c)) return c;
			}
		}
		
		
		
	public:
		JsonReader(std::istream& in):opcode(-1),in(in),eof_met(false),curr_bool(false)
			{
			states.push_back(EXPECT_ANY);
			}
		
		
		bool hasNext() {
			while(this->opcode == -1 && !eof_met)
				{
				this->curr_string.clear();
				
				int c = this->read();

				switch(c)
					{
					/* EOF MET */
					case -1 : 
						{

						if( ! states.empty() )
							{
							BOUM("EOF MET, but state stack is not empty.");
							}
						eof_met = true;
						return false;
						}
					/** colon state must be OBJECT_EXPECT_COLON */
					case ':' : {
						if( states.empty() || !(states.back() == OBJECT_EXPECT_COLON))
							{
							BOUM("bad stack. A Colon ':' was expected.");
							}
						
						states.back() = OBJECT_EXPECT_ANY;
						break;
						}
					/** comma: state must be in array or object */
					case ',' : 
						{
						if( states.empty() )
							{
							BOUM("bad stack. got a comma while Stack is empty.");
							}
						
						switch( states.back()  )
							{
							case( OBJECT_EXPECT_COMMA_OR_END ) : 
								{
								states.back() = OBJECT_EXPECT_NAME;
								break;
								}
							case ARRAY_EXPECT_COMMA_OR_END :
								{
								states.back() = ARRAY_EXPECT_ANY;
								break;
								}
							default:
								{
								BOUM("bad stack. Got comma ',' .");
								break;
								}
							}
						break;
						}
					/* skip whitespaces */
					case ' ': case '\r' : case '\t' : case '\n': break;
					case 'f':
						{
						CHECK_ANY
						
						MUST_READ('a');
						MUST_READ('l');
						MUST_READ('s');
						MUST_READ('e');
						this->curr_bool = false;
						this->opcode = OPCODE_BOOLEAN;
						break;
						}
					case 't':
						{
						CHECK_ANY
						MUST_READ('r');
						MUST_READ('u');
						MUST_READ('e');
						this->curr_bool = true;
						this->opcode = OPCODE_BOOLEAN;
						break;
						}
					case 'n':
						{
						CHECK_ANY
						MUST_READ('u');
						MUST_READ('l');
						MUST_READ('l');
						this->opcode = OPCODE_NULL;
						break;
						}
					case '[':
						{
						CHECK_ANY
						states.push_back(ARRAY_EXPECT_ANY_OR_END);	
						this->opcode = OPCODE_ARRAY_START;
						break;
						}
					
					case '{':
						{
						CHECK_ANY
						states.push_back( OBJECT_EXPECT_NAME_OR_END);
						this->opcode = OPCODE_OBJECT_START;
						break;
						}
					case '}':
						{
						if( states.empty() || !(states.back() == OBJECT_EXPECT_COMMA_OR_END || states.back() == OBJECT_EXPECT_NAME_OR_END))
							{
							BOUM("bad stack.  '}' was expected.");
							}
						states.pop_back();
						this->opcode = OPCODE_OBJECT_END;
						break;
						}
					case ']':
						{
						if( states.empty() || !(states.back() == ARRAY_EXPECT_ANY_OR_END || states.back() == ARRAY_EXPECT_COMMA_OR_END))
							{
							BOUM("bad stack.  ']' was expected.");
							}	
						states.pop_back();
						this->opcode = OPCODE_ARRAY_END;
						break;
						}
					case '"':
					case '\'':
						{
						int quote_symbol = c;
						CHECK_STATE_PRESENT
						if( states.back() == OBJECT_EXPECT_NAME ||  states.back() == OBJECT_EXPECT_NAME_OR_END)
							{
							states.back() = OBJECT_EXPECT_COLON;
							}
						else if ( states.back() == ARRAY_EXPECT_ANY_OR_END )
							{
							states.back() = ARRAY_EXPECT_COMMA_OR_END;
							}
						std::ostringstream sb;
						while((c = in.get())!=-1)
							{
							if(c== quote_symbol ) break;
							if( c=='\\')
								{
								c= in.get();
								if(c==-1) BOUM("");
								switch(c)
									{
									case 'n': c='\n';break;
									case 'r': c='\r';break;
									case 't': c='\t';break;
									case '\'': c='\'';break;
									case '\"': c='\"';break;
									case '\\': c='\\';break;
									default:BOUM("");break;
									}
								}
							sb << (char)c;
							}
						this->curr_string = sb.str();
						this->opcode = OPCODE_STRING;
						break;
						}
					case '+': case '-': case '.':
					case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9':
						{
						CHECK_ANY
						std::ostringstream sb;
						sb << (char)c;
						bool exit=false;
						while(!exit && (c = this->read())!=-1)
							{
							switch(c)
								{
								case '+': case '-': case '.': 
								case 'e': case 'E':
								case '0': case '1': case '2': case '3': case '4':
								case '5': case '6': case '7': case '8': case '9':
									{
									sb << (char)c;
									break;
									}
								default: 
									{
									exit=true;
									break;
									}
								}
							}
						if(c!=-1) this->in.putback((char)c);
						this->curr_string = sb.str();
						this->opcode = OPCODE_NUMBER;
						break;
						}
					}
				}
			return this->opcode!=-1;
			}
		
		int next()
			{
			if( !hasNext()) BOUM("");
			int op = this->opcode;
			this->opcode = -1;
			return op;
			}
		
		int peek() {
			if(hasNext())  return this->opcode;
			
			return -1;
			}
		
		void beginArray() {
			int i;
        	if( (i=peek()) != OPCODE_ARRAY_START ) BOUM("beginArray failed. Got opcode (" << i <<  ")");
        	this->opcode=-1;
    		}
    	
    	void beginObject() {
			int i;
        	if( (i=peek()) != OPCODE_OBJECT_START ) BOUM("beginObject failed. Got opcode (" << i <<  ")");
        	this->opcode=-1;
    		}
    	
    	void endObject() {
			int i;
        	if( (i=peek()) != OPCODE_OBJECT_END ) BOUM("endObject failed. Got opcode (" << i <<  ")");
        	this->opcode=-1;
    		}

    	void endArray() {
			int i;
        	if( (i=peek()) != OPCODE_ARRAY_END ) BOUM("endArray failed. Got opcode (" << i <<  ")");
        	this->opcode=-1;
    		}
		
		bool nextBoolean() {
			int i;
        	if( (i=peek()) != OPCODE_BOOLEAN ) BOUM("nextBoolean failed. Got opcode (" << i <<  ")");
        	bool b = this->curr_bool;
        	this->opcode=-1;
        	return b;
    		}
		
		double nextDouble() {
			int i;
        	if( (i=peek()) != OPCODE_NUMBER ) BOUM("nextDouble failed. Got opcode (" << i <<  ")");
        	double b = std::atof(this->curr_string.c_str());
        	this->opcode=-1;
        	return b;
    		}
    	
    	int nextInt() {
			int i;
        	if( (i=peek()) != OPCODE_NUMBER ) BOUM("nextInt failed. Got opcode (" << i <<  ")");
        	int b = std::atoi(this->curr_string.c_str());
        	this->opcode=-1;
        	return b;
    		}
    		
    	
    	std::string nextName() {
			int i;
        	if( (i=peek()) != OPCODE_OBJECT_NAME ) BOUM("nextName failed. Got opcode (" << i <<  ")");
        	std::string s( this->curr_string );
        	this->opcode=-1;
    		return s;
    		}
    		
    	void nextNull() {
			int i;
        	if( (i=peek()) != OPCODE_NULL ) BOUM("nextNull failed. Got opcode (" << i <<  ")");
        	this->opcode=-1;
    		}		
		
		std::string nextString() {
			int i;
        	if( (i=peek()) != OPCODE_STRING ) BOUM("nextString failed. Got opcode (" << i <<  ")");
        	std::string s( this->curr_string );
        	this->opcode=-1;
    		return s;
    		}
    	
    	
    	void skipValue() {
            if (!hasNext() || this->peek() == -1) {
               BOUM("No element left to skip");
            }
            int count = 0;
            do {
                int token = this->peek();
                this->opcode=-1;
                if (token == OPCODE_BEGIN_ARRAY || token ==OPCODE_BEGIN_OBJECT) {
                    count++;
                } else if (token == OPCODE_END_ARRAY || token == OPCODE_END_OBJECT) {
                    count--;
                }
           		} while (count != 0);
    		}
		
	};

#undef BOUM

#endif

