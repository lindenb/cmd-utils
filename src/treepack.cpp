#include <algorithm>
#include <memory>
#include <map>
#include "config.h"
#if HAS_LIB_CAIRO == 1
#include <cairo/cairo.h>
#endif
#include "stringsplitter.hh"
#include "hershey.hh"
#include "treepack.tab.h"


using namespace std;

template <typename T>
struct Rect {
		T x;
		T y;
		T width;
		T height;
		Rect(){}
		Rect(T x,T y,T width,T height):x(x),y(y),width(width),height(height) {}
		T getWidth() const { return this->width;}
		T getHeight() const { return this->height;}
		T getX() const { return this->x;}
		T getY() const { return this->y;}
		
		Rect<T> inset(double ratio) {
			T r1_width= this->getWidth()*ratio;
			T r1_x= this->getX()+(this->getWidth()-r1_width)/2;
			T r1_height= this->getHeight()*ratio;
			T r1_y=this->getY()+(this->getHeight()-r1_height)/2;
			Rect<T> r1(r1_x,r1_y,r1_width,r1_height);
			return r1;
			}
		
		Rect<T> inset() { return this->inset(0.9);}
		T getMidX() const { return (T)(getX()+getWidth()/2.0);}
		T getMidY() const { return (T)(getY()+getHeight()/2.0);}
	};


typedef Rect<double> Rectangle;


enum Orientation { VERTICAL, HORIZONTAL};
enum Direction { ASCENDING, DESCENDING};

class TreePack;

class Packer {
	public:
		void layout(const std::vector<TreePack*>* items, Rectangle bounds);
	private:
		double sum(const std::vector<TreePack*>* items, int start, int end);
		std::auto_ptr<std::vector<TreePack*> > sortDescending(const std::vector<TreePack*>* items);
		void layout(std::vector<TreePack*>* items, int start, int end, const Rectangle bounds);
		double aspect(double big, double small, double a, double b);
		double normAspect(double big, double small, double a, double b);
		void layoutBest(std::vector<TreePack*>* items, int start, int end, Rectangle bounds);
		void sliceLayout(std::vector<TreePack*>* items, int start, int end, Rectangle bounds, Orientation orientation, Direction order);
	};


class TreePack
	{
	public:
		virtual ~TreePack()  {}
		virtual void setBounds(const Rectangle& bounds)=0;
		virtual Rectangle getBounds()=0;
		virtual double getWeight() const = 0;
	};

class DefaultTreePack : public TreePack
	{
	public:
		std::string name;
		DefaultTreePack* parent;
		std::map<std::string,DefaultTreePack*> children;
		Rectangle bounds;
		double weight;

		virtual ~DefaultTreePack() {
			for(std::map<std::string,DefaultTreePack*>::iterator r= children.begin();
				r!= children.end();
				++r)
				{
				delete r->second;
				}
			}
		virtual void setBounds(const Rectangle& bounds)	 {
			this->bounds  = bounds;
			}
	
		virtual Rectangle getBounds()	 {
			return bounds;
			}
	
		virtual double  getWeight() const	 {
			if(children.empty() )
				{
				return weight;
				}
			else
				{
				double w= 0.0;
		
				for(std::map<std::string,DefaultTreePack*>::const_iterator r= children.begin();
					r!= children.end();
					++r)
					{
					w += r->second->getWeight();
					}
				return w;
				}
			}
		};


class RootTreePack : public DefaultTreePack
	{
	public:
		virtual ~RootTreePack() {}
	};



	
static bool comparator(const TreePack* pack1,const TreePack* pack2)
	{
	double w1= pack1->getWeight();
	double w2= pack2->getWeight();
	return w1>w2;
	}

    
void Packer::layout(const std::vector<TreePack*>* items, Rectangle bounds)
	{
	std::auto_ptr<std::vector<TreePack*> > L = sortDescending(items);
    layout(L.get(),0,L->size()-1,bounds);
	}
    
double Packer::sum(const std::vector<TreePack*>* items, int start, int end)
    {
    double sum=0;
    while(start<=end)//yes <=
    	{
    	sum+=items->at(start++)->getWeight();
    	}
    return sum;
    }
	
std::auto_ptr<std::vector<TreePack*> > Packer::sortDescending(const std::vector<TreePack*>* items)
    {
    std::vector<TreePack*>* L= new std::vector<TreePack*>(items->begin(),items->end());
    std::sort(L->begin(),L->end(),comparator);
    return std::auto_ptr<std::vector<TreePack*> >(L);
    }
	
void Packer::layout(std::vector<TreePack*>* items, int start, int end, const Rectangle bounds)
    {
        if (start>end) return;
            
        if (end-start<2)
        {
            layoutBest(items,start,end,bounds);
            return;
        }
        
        double x=bounds.getX(), y=bounds.getY(), w=bounds.getWidth(), h=bounds.getHeight();
        
        double total= sum(items, start, end);
        int mid=start;
        double a=items->at(start)->getWeight()/total;
        double b=a;
        
        if (w<h)
        {
            // height/width
            while (mid<=end)
            {
                double aspect=normAspect(h,w,a,b);
                double q=items->at(mid)->getWeight()/total;
                if (normAspect(h,w,a,b+q)>aspect) break;
                mid++;
                b+=q;
            }
            layoutBest(items,start,mid,Rectangle(x,y,w,h*b));
            layout(items,mid+1,end, Rectangle(x,y+h*b,w,h*(1-b)));
        }
        else
        {
            // width/height
            while (mid<=end)
            {
                double aspect=normAspect(w,h,a,b);
                double q=items->at(mid)->getWeight()/total;
                if (normAspect(w,h,a,b+q)>aspect) break;
                mid++;
                b+=q;
            }
           layoutBest(items,start,mid, Rectangle(x,y,w*b,h));
           layout(items,mid+1,end, Rectangle(x+w*b,y,w*(1-b),h));
        }
        
    }
    
double Packer::aspect(double big, double small, double a, double b)
    {
        return (big*b)/(small*a/b);
    }
    
double Packer::normAspect(double big, double small, double a, double b)
    {
        double x=aspect(big,small,a,b);
        if (x<1) return 1/x;
        return x;
    }

void Packer::layoutBest(std::vector<TreePack*>* items, int start, int end, Rectangle bounds)
    {
    sliceLayout(
    		items,start,end,bounds,
            bounds.getWidth()>bounds.getHeight() ? HORIZONTAL : VERTICAL,
            ASCENDING
            );
    }
    
    

void Packer::sliceLayout(std::vector<TreePack*>* items, int start, int end, Rectangle bounds, Orientation orientation, Direction order)
        {
            double total= sum(items, start, end);
            double a=0;
            bool vertical=(orientation==VERTICAL);
           
            for (int i=start; i<=end; i++)
            {
            	Rectangle  r;
                double b=items->at(i)->getWeight()/total;
                if (vertical)
                {
                    r.x=bounds.getX();
                    r.width=bounds.getWidth();
                    if (order==ASCENDING)
                        r.y=bounds.getY()+bounds.getHeight()*a;
                    else
                        r.y=bounds.getY()+bounds.getHeight()*(1-a-b);
                    r.height=bounds.getHeight()*b;
                }
                else
                {
                    if (order==ASCENDING)
                        r.x=bounds.getX()+bounds.getWidth()*a;
                    else
                        r.x=bounds.getX()+bounds.getWidth()*(1-a-b);
                    r.width=bounds.getWidth()*b;
                    r.y=bounds.getY();
                    r.height=bounds.getHeight();
                }
     
                items->at(i)->setBounds(r);
                a+=b;
            }
        }

class TreePackApp: public TreePackBase
	{
	public:
		TreePackApp() {}
		virtual ~TreePackApp() {}
		
		
		virtual void processIstream(const char* fname,istream& in) {
			StringSplitter splitter;
			splitter.set_delimiter(this->delimiter);
			vector<std::string> header;
			string line;
			if(!getline(in,line)) {
				THROW("Cannot read 1st line");
				}
			splitter.split(line,header);

			RootTreePack root;

			int nLine=0;
			while(getline(in,line)) {
				++nLine;
				vector<std::string> tokens;
				splitter.split(line,tokens);
				if(tokens.size() != header.size()) {
					THROW("line " << nLine << ":\"" << line << "\". Expected " << header.size() << " tokens but got " << tokens.size());
					}
				DefaultTreePack* curr = &root;
				for(vector<std::string>::size_type i=0; i< tokens.size();++i) {
					std::map<std::string,DefaultTreePack*>::iterator r = curr->children.find(tokens[i]);
					if( r == curr->children.end() )
						{
						DefaultTreePack* c= new DefaultTreePack;
						c->name.assign(tokens[i]);
						c->parent = curr;
						c->weight = 1.0;
						curr->children.insert( make_pair(c->name,c) );
						curr = c;
						}
					else
						{
						DefaultTreePack* c= r->second;
						c->weight += 1.0;
						curr = c;
						}
					}

				}
			Packer packer;
			root.bounds.x=0;
			root.bounds.y=0;
			root.bounds.width=this->image_width;
			root.bounds.height=this->image_height;
			
#if HAS_LIB_CAIRO == 1
			cairo_surface_t *surface = ::cairo_image_surface_create (CAIRO_FORMAT_ARGB32, this->image_width, this->image_height);
			if(surface==0) {
				cerr << "Cannot allocate image." << endl;
				exit(EXIT_FAILURE);
				}
			cairo_t *cr = ::cairo_create (surface);
			if(cr==0) {
				cerr << "Cannot allocate cairo." << endl;
				exit(EXIT_FAILURE);
				}

			clog << "PACKING" << endl;
			paint(&packer,&root,cr);

			if(this->output_name != 0) {
				clog << "[LOG] saving to " << this->output_name << endl;
				cairo_surface_write_to_png(surface, this->output_name);
				}

			::cairo_destroy (cr);
			::cairo_surface_destroy (surface);
#endif
			
			}

#if HAS_LIB_CAIRO == 1
		void paint(Packer* packer,DefaultTreePack* node,cairo_t *cr) {
			cairo_stroke(cr);    
			std::vector<TreePack*> items;
			for(std::map<std::string,DefaultTreePack*>::const_iterator r= node->children.begin();
						r!= node->children.end();
						++r)
						{
						items.push_back(r->second);
						}
			if(!node->children.empty()) {		
				packer->layout(&items,node->bounds);
				}

			for(std::map<std::string,DefaultTreePack*>::const_iterator r=  node->children.begin();
						r!= node->children.end();
						++r)
						{
						::cairo_set_source_rgb(cr, 0, 0, 0);
						::cairo_set_line_width(cr, 0.5);
						const Rectangle& rect= r->second->bounds;
						::cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
						::cairo_stroke(cr);   
			
						paint(packer,r->second,cr);
						}

			ostringstream os;
			os << node->name <<"=" << node->getWeight();
			string title = os.str();
			Rectangle titleRect;
			if( node->children.empty() )
				{
				titleRect = node->bounds.inset(0.9);
				}
			else
				{
				titleRect = node->bounds.inset(0.9);
				}

			double theight= 14.0;
			double twidth = title.length()*theight;
			while(twidth> titleRect.getWidth() || theight > titleRect.getHeight()) {
				theight*=0.99;
				twidth*=0.99;
				}
			titleRect.x= titleRect.x + (titleRect.getWidth()-twidth)/2.0;
			titleRect.y= titleRect.y + (titleRect.getHeight()-theight)/2.0;
			titleRect.width=twidth;
			titleRect.height=theight;

			Hershey hershey;

			hershey.paint(cr,title.c_str(),titleRect.x,titleRect.y,titleRect.width,titleRect.height);
			}
#endif
		
		virtual int main(int argc, char** argv) {
			int optind = parseOptions(argc,argv);
			oneOrStdin(optind,argc,argv);
			return EXIT_SUCCESS;
			}
	};


int main(int argc,char** argv)
	{
	TreePackApp app;
	return app.main(argc,argv);
	}

