#include <vector>
#include "hershey.tab.h"
#include "hershey.hh"

using namespace std;

class ScreenDrawer: public Hershey::Drawer
	{
	private:
		vector<vector<char> > yxscreen;
		
		void set(int x,int y,char c) {
			size_t len = ( yxscreen.empty() ? 0UL : yxscreen[0].size() ); 
		
			while( y >= (int)yxscreen.size())
				{
				vector<char> row(len,' ');
				yxscreen.push_back(row);
				}
			if(!yxscreen.empty() && x>= (int)len  ) {
				for(size_t t=0; t< yxscreen.size();++t)
					{
					yxscreen[t].resize(x+1,' ');
					}
				}
			yxscreen[y][x]=c;
			}
		
	public:
		char fillchar;
		double last_x,last_y;
		ScreenDrawer():last_x(0),last_y(0) {
		}
		virtual ~ScreenDrawer() {}
		
	
		void line(int x0, int y0, int x1, int y1) {
			  int dx = std::abs(x1-x0), sx = x0<x1 ? 1 : -1;
			  int dy = std::abs(y1-y0), sy = y0<y1 ? 1 : -1; 
			  int err = (dx>dy ? dx : -dy)/2, e2;
			 
			  for(;;){
				set(x0,y0,fillchar);
				if (x0==x1 && y0==y1) break;
				e2 = err;
				if (e2 >-dx) { err -= dy; x0 += sx; }
				if (e2 < dy) { err += dx; y0 += sy; }
			  }
			}
		
		virtual void lineto(double x,double y) {
			line((int)last_x,(int)last_y,(int)x,(int)y);
			last_x=x;
			last_y=y;
			}
		virtual void moveto(double x,double y)
			{
			last_x=x;
			last_y=y;
			}	
		virtual void print()
			{
			for(size_t y=0;y< yxscreen.size();++y)
				{
				for(size_t x=0;x< yxscreen[y].size();++x)
					{
					cout << yxscreen[y][x];
					}
				cout << endl;
				}
			}
	};

class HersheyBanner: public HersheyBannerBase {
	public:

	
	virtual int main(int argc,char** argv)
		{
		int optind = parseOptions(argc,argv);
		char* cols= getenv("COLUMNS");
		if( term_width <0 && cols!=0) term_width=atoi(cols);
		if( term_width <0) term_width=80;
		
		Hershey h;
		for(int i=optind;i < argc;++i)
			{
			if(i>optind) cout << endl;
			ScreenDrawer sd;
			sd.fillchar = this->fillchar;
			int height = this->term_height;
			if( height <0) height= 10;
			h.draw(&sd,argv[i],0,0,term_width,height);
			sd.print();
			}
		return EXIT_SUCCESS;
		}
};

int main(int argc,char** argv)
	{
	HersheyBanner app;
	return app.main(argc,argv);
	}
