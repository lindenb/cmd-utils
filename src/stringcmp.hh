#include <string>
#include <cstring>

class StringComparator {
public:
	bool smart;
	bool ignore_case;
private:
	char tr(char c) {
		return ignore_case ? toupper(c) : c;
		}
public:
	StringComparator():smart(false),ignore_case(false) {
	}
	
	int compare(const std::string& a,const std::string& b)
		{
		if(!smart)
			{
			if( ignore_case) {
				return a.compare(b);
				}
			else
				{
				strcasecmp(a.c_str(),b.c_str());
				}
			}
		
        const std::string* ss[]={&a,&b};
        char c[]={0,0};
        std::string::size_type i[]={0,0};
        for(;;)
            {
            if(i[0] == a.size() &&
               i[1] == b.length()) return 0;
            if(i[0] == a.size()) return -1;
            if(i[1] == b.size()) return 1;

            c[0]= tr(a.at(i[0]));
            c[1]= tr(b.at(i[1]));

            //search for integer
            if( isdigit(c[0]) &&
                isdigit(c[1]) )
                {
                std::string::size_type len[2]={1UL,1UL};
                for(int side=0;side<2;++side)
                    {
                    while(  i[side] + len[side] < ss[side]->length() &&
                               std::isdigit( ss[side]->at(i[side] + len[side]) )
                          )
                        {
                        len[side]++;
                        }
                    }
               
                if(len[0] < len[1] )
                	{
                	return -1;
                	}
                else if(len[0] > len[1] )
                	{
                	return 1;
                	}
                else
                	{
                	for(size_t x=0;x < len[0];++x)
                		{
                		int v0 = a[ i[0]+x ] - '0';
                		int v1 = b[ i[1]+x ] - '0';
                		if( v0 != v1 ) return (v0<v1?-1:1);
                		}
                	}
                i[0]+=len[0];
                i[1]+=len[1];
                continue;
                }
           
            int k=c[0]-c[1];
            if(k!=0) return (k<0?-1:1);
            i[0]++;
            i[1]++;
            }

		}

	bool operator() (const std::string& s1,const std::string& s2) {
		return compare(s1,s2)<0;
		}
	};
