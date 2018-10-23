#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <vector>

using namespace std;

static vector<int> thestack;

int peek(size_t n) {
	while(thestack.size()<=n)
		{
		int c = fgetc(stdin);
		if(c==EOF) return EOF;
		thestack.push_back(c);
		}
	return thestack[n];
	}
void consumme(size_t n)
	{
	thestack.erase(thestack.begin(),thestack.begin()+n);
	}

int main(int argc,char** argv) {
int c;
printf("<html><head><meta charset=\"UTF-8\"/></head><body><pre style=\"background-color:black;color:white;\">");
while((c=peek(0))!=EOF) {
	if(c== 0xE2 && c ==EOF ) {
		int c1 = peek(1);
		int c2 = peek(2) ;
		consumme(3);
		printf("&#x%02X%02X;",
			(unsigned int)(c1 & 0xFF),
			(unsigned int)(c2 & 0xFF)
			);
		}
	else if(c==27 && peek(1)==91)
		{
		consumme(2);
		int m=0;
		while((c=peek(0))!=EOF && c!='m')
			{
			m= m*10 + (c-'0');
			consumme(1);
			}
		if(c=='m') {
			consumme(1);
			switch(m) {
				case 0: fputs("</span>",stdout);break;
				case 30: fputs("<span style=\"color:black;\">",stdout);break;
				case 31: fputs("<span style=\"color:red;\">",stdout);break;
				case 32: fputs("<span style=\"color:green;\">",stdout);break;
				case 33: fputs("<span style=\"color:yellow;\">",stdout);break;
				case 34: fputs("<span style=\"color:blue;\">",stdout);break;
				case 35: fputs("<span style=\"color:magenta;\">",stdout);break;
				case 36: fputs("<span style=\"color:cyan;\">",stdout);break;
				case 37: fputs("<span style=\"color:white;\">",stdout);break;
				default: fputs("<span>",stdout);break;
				}
			} 
		}
	else
		{
		switch(c)
			{
			case '\n': fputs("<br/>",stdout);break;
			case '<': fputs("&lt;",stdout);break;
			case '>': fputs("&gt;",stdout);break;
			case '&': fputs("&amp;",stdout);break;
			default: fputc(c,stdout);break;
			}
		consumme(1);
		}
	}
printf("</pre></body></html>\n");
return EXIT_FAILURE;
}
