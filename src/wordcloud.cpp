#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "wordcloud.tab.h"
#include "hershey.hh"
#include "stringutils.hh"

using namespace std;


struct MinMax
	{
	double m,M;
	MinMax():m(0),M(-1000000000) {}
	double distance() { return M-m;}
	void watch(double v) {
		m = std::min(v,m);
		M = std::max(v,M);
		}
	};

struct Point
{
    double x;
    double y;
};
 

struct Line
	{
	Point p1;
	Point p2;
	bool intersects(Line& cp) {
		return true;
		}
	};

class Text
	{
	private:
		class ScreenDrawer: public Hershey::Drawer
			{
			public:
				vector<Line>* lines;
				double last_x,last_y;
				MinMax minmax_x,minmax_y;
				ScreenDrawer(vector<Line>* lines):lines(lines),last_x(0),last_y(0) {
					}
				virtual ~ScreenDrawer() {
					}
				
				void line(double x1, double y1, double x2, double y2)
					{
					Line L;
					L.p1.x = x1;
					L.p1.y = y1;
					L.p2.x = x2;
					L.p2.y = y2;
					
					minmax_x.watch(x1);
					minmax_x.watch(x2);
					minmax_y.watch(y1);
					minmax_y.watch(y2);
					
					this->lines->push_back(L);
					}
				virtual void lineto(double x,double y) {
					line(last_x,last_y,x,y);
					last_x=x;
					last_y=y;
					
					}
				virtual void moveto(double x,double y)
					{
					last_x=x;
					last_y=y;
					}
	
			};

	public:
		double x,y,width,height;
		vector<Line> lines;
		string content;
		unsigned long weight;
		Text(std::string& s,double fontsize,unsigned long weight):x(0),y(0),width(0),height(0),content(s),weight(weight)
			{
			Hershey h;
			ScreenDrawer sd(&lines);
			h.draw(&sd,s.c_str(),0,0,fontsize*s.size(),fontsize);
			x = sd.minmax_x.m;
			y = sd.minmax_y.m;
			width =  sd.minmax_x.distance();
			height = sd.minmax_y.distance();
			}
		
		bool intersects(Text* cp) {
			if(x+width < cp->x) return false;
			if(x > cp->x+cp->width) return false;
			
			if(y+height < cp->y) return false;
			if(y > cp->y+cp->height) return false;
			return true;
			}
		void scale(double f)
			{
			this->width *= f;
			this->height *= f;
			for(size_t k=0;k<this->lines.size();++k)
				{
				this->lines[k].p1.x*=f;
				this->lines[k].p1.y*=f;
				this->lines[k].p2.x*=f;
				this->lines[k].p2.y*=f;
				}
			}
		void shift(double dx,double dy)
			{
			this->x += dx;
			this->y += dy;
			for(size_t k=0;k<this->lines.size();++k)
				{
				this->lines[k].p1.x+= dx;
				this->lines[k].p1.y+= dy;
				this->lines[k].p2.x+= dx;
				this->lines[k].p2.y+= dy;
				}
			}
		
	};
	
static bool comparator(const Text* pack1,const Text* pack2)
	{
	double w1= pack1->weight;
	double w2= pack2->weight;
	return w1>w2;
	}



class WordCloud: public WordCloudBase {
public:
	vector<Text*> texts;
	WordCloud() {
		}
	~WordCloud() {
		for(vector<Text*>::size_type i=0;i< texts.size();++i) {
			delete texts[i];
			}
		}
	virtual void processIstream(const char* f,std::istream& in) {
		StringUtils strutils;
		string line;
		while(getline(in,line)) {
			unsigned long count = 0UL;
			string::size_type i1=0;
			strutils.fromUniq(line,&count);
			
			
			for(i1=0;i1<line.size();++i1) {
				if(line[i1]=='\t') line[i1]=' ';
				}
			
			strutils.trim(line);
			if(line.empty() || count==0UL) {
				cerr << "Ignoring empty line" << endl;
				continue;
				}
			Text* t = new Text(line,12,count);
			if(t->lines.empty()) {
				delete t;
				continue;
				}
			this->texts.push_back(t);
			}	
		}
	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		manyOrStdin(optind,argc,argv);
		if(texts.empty())
			{
			cerr << "No Data." << endl;
			return EXIT_FAILURE;
			}
		unsigned long maxweight = 0UL;
		
		//get max weight
		for(vector<Text*>::size_type i=0;i< texts.size();++i) {
			maxweight = std::max( maxweight , texts[i]->weight );
			//force on 0
			texts[i]->shift(-texts[i]->x,-texts[i]->y);
			}
		//sort, get biggest first
		std::sort(texts.begin(),texts.end(),comparator);
		
		//scale on biggest
		for(vector<Text*>::size_type i=1;i< texts.size();++i) {
			double ratio = (double)texts[i]->weight/(double)texts[0]->weight;
			
			//scale
			texts[i]->scale( ratio) ;
			}
		
		//adjust position of each text
		for(vector<Text*>::size_type i=0;i< texts.size();++i)
			{
			double radius = 0;
			double angle_deg=0.0;
			double sign=(i%2==0?-1:1);
			double d_angle = 1*sign;
			
			
			for(;;)

				{
				bool ok=true;
				double radian = angle_deg/360.0 *  (M_PI*2.0); 
				double dx= -texts[i]->width/2.0 + radius*cos(radian);
				double dy= -texts[i]->height/2.0+ radius*sin(radian);
				texts[i]->shift(dx,dy);
				
				//does this intersect with previous ?
				for(vector<Text*>::size_type j=0;j<i;++j) 
					{
					if(texts[i]->intersects(texts[j]))
						{
						ok=false;
						break;
						}
					}
				if(ok)  {
					break;
					}
				texts[i]->shift(-dx,-dy);
				angle_deg+=d_angle;
				radius += (d_angle/360.0);
				}
			
			}
		MinMax minmax_x;
		MinMax minmax_y;
		for(vector<Text*>::size_type i=0;i< texts.size();++i) {
			double v = texts[i]->x;
			minmax_x.watch(v);
			v =  texts[i]->x+ texts[i]->width;
			minmax_x.watch(v);
			
			
			v = texts[i]->y;
			minmax_y.watch(v);
			v =  texts[i]->y+ texts[i]->height;
			minmax_y.watch(v);
			}
		for(vector<Text*>::size_type i=0;i< texts.size();++i) 
			{
			texts[i]->shift(-minmax_x.m,-minmax_y.m);
			}
		
		xmlTextWriterPtr w = ::xmlNewTextWriterFilename("-",0);
		::xmlTextWriterStartDocument(w,"1.0","UTF-8",NULL);
		::xmlTextWriterStartElementNS(w,NULL,BAD_CAST "svg",BAD_CAST "http://www.w3.org/2000/svg");
		::xmlTextWriterWriteFormatAttribute(w,BAD_CAST "width","%d",(int)(minmax_x.distance())+1);
		::xmlTextWriterWriteFormatAttribute(w,BAD_CAST "height","%d",(int)(minmax_y.distance())+1);
		
		for(vector<Text*>::size_type i=0;i< texts.size();++i) {
			::xmlTextWriterStartElement(w,BAD_CAST "path");
			::xmlTextWriterWriteFormatAttribute(w,BAD_CAST "id","w%d",(int)i);
			::xmlTextWriterWriteFormatAttribute(w,BAD_CAST "style","stroke:%s; fill:none;",(i%2==0?"red":"blue"));
			::xmlTextWriterStartAttribute(w,BAD_CAST "d");
			for(vector<Text*>::size_type j=0;j<texts[i]->lines.size();++j) 
				{
				if(j>0 &&
				    texts[i]->lines[j-1].p2.x == texts[i]->lines[j].p1.x && 
				    texts[i]->lines[j-1].p2.y == texts[i]->lines[j].p1.y )
				    {
				    ::xmlTextWriterWriteFormatString(w," %.2f,%.2f",texts[i]->lines[j].p2.x,texts[i]->lines[j].p2.y);
				    }
				else
					{
					::xmlTextWriterWriteFormatString(w,"%sM%.2f,%.2f",(j==0?"":" "),texts[i]->lines[j].p1.x,texts[i]->lines[j].p1.y);
					::xmlTextWriterWriteFormatString(w," L%.2f,%.2f",texts[i]->lines[j].p2.x,texts[i]->lines[j].p2.y);
					}
				}
					
			::xmlTextWriterEndAttribute(w);
			::xmlTextWriterStartElement(w,BAD_CAST "title");
			::xmlTextWriterWriteString(w,BAD_CAST texts[i]->content.c_str());
			::xmlTextWriterWriteFormatString(w," (%d)",(int) texts[i]->weight);
			::xmlTextWriterEndElement(w);
			
			::xmlTextWriterEndElement(w);
			}
		::xmlTextWriterEndElement(w);
		::xmlTextWriterEndDocument(w);
		::xmlFreeTextWriter(w);

		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	LIBXML_TEST_VERSION
	WordCloud app;
	return app.main(argc,argv);
	}
