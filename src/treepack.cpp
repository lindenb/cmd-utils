#include <algorithm>
#include <memory>
#include <map>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "config.h"
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
		virtual xmlNodePtr svg(TreePack* c) = 0;
	};

class DefaultTreePack : public TreePack
	{
	public:
		std::string name;
		std::string category;
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
		virtual xmlNodePtr svg(TreePack* c,xmlDocPtr document)
			{
			Hershey hershey;
	
			if( this->parent == null)
				{
				xmlNodePtr svg = ::xmlNewNode(SVG,"svg");
				svg.setAttribute("width",this->bounds.width);
				svg.setAttribute("height",this->bounds.height);
		
		
				xmlNodePtr defs = ::xmlNewNode(SVG,"defs");
				svg.appendChild(defs);
		
				xmlNodePtr style = ::xmlNewNode(SVG,"style");
				style.setAttribute("type","text/css");
				style.appendChild(document.createTextNode(
					"svg {fill:none;stroke:black;stroke-width:0.5px;}\n"+
					".r0 {fill:rgb(240,240,240);stroke:black;stroke-width:0.5px;}\n"+
					".r1 {fill:rgb(220,220,220);stroke:black;stroke-width:0.5px;}\n"+
					".lbla0 {stroke:black;stroke-width:1px;}\n"+
					".lblb0 {stroke:red;stroke-width:1px;}\n"+
					".lbla1 {stroke:gray;stroke-width:1px;}\n"+
					".lblb1 {stroke:red;stroke-width:1px;}\n"+
					""
					));

				defs.appendChild(style);
		
				xmlNodePtr title = ::xmlNewNode(SVG,"svg:title");
				title.appendChild(document.createTextNode("TreeMap"));
				svg.appendChild(title);
		
				xmlNodePtr rect =  document.createElementNS(SVG,"svg:rect");
				rect.setAttribute("x",0);
				rect.setAttribute("y",0);
				rect.setAttribute("width",(this->bounds.width-1));
				rect.setAttribute("height",(this->bounds.height-1));
				rect.setAttribute("style","fill:white;stroke:black;");
				::xmlAddChild(svg,rect);
	
		
				xmlNodePtr g = document.createElementNS(SVG,"svg:g");
				::xmlAddChild(svg,g);
		
				var L = [];
				for(var k in this->children) L.push(this->children[k]);
				packer.layout2(L,this->bounds);
		
				for(var c in L)
					{
					var d = L[c].svg(packer);
					if(d==null) continue;
					g.appendChild(d);
					}
		
				rect =  document.createElementNS(SVG,"svg:rect");
				rect.setAttribute("x",0);
				rect.setAttribute("y",0);
				rect.setAttribute("width",(this->bounds.width-1));
				rect.setAttribute("height",(this->bounds.height-1));
				rect.setAttribute("style","fill:none;stroke:black;");
				::xmlAddChild(svg,rect);
			
				return svg;
				}
			else
				{
		
				if(this->getWeight()<=0)
					   {
					   return null;
					   }
		
				   var bounds = this->getBounds();
				   var insets = bounds.inset(0.9);
				   var frameUsed=bounds;
				   
				   
				   if(bounds.getWidth()<=1 || bounds.getHeight()<=1)
					   {
					   return null;
					   }
				   var g = document.createElementNS(SVG,"svg:g");

				   
				   

				   var rect =  document.createElementNS(SVG,"svg:rect");
				   g.appendChild(rect);
				   rect.setAttribute("x",frameUsed.x);
		   		   rect.setAttribute("y",frameUsed.y);
		   		   rect.setAttribute("width",frameUsed.width);
		   		   rect.setAttribute("height",frameUsed.height);
		   		   rect.setAttribute("class", "r"+(this->getDepth()%2));
		   		   var title = document.createElementNS(SVG,"svg:title");
		   		   rect.appendChild(title);
		   		   title.appendChild( document.createTextNode(this->category+":"+this->name+"="+this->getWeight()));

		
				   if(!this->isLeaf())
					   {
					   var path =  document.createElementNS(SVG,"svg:path");
					   path.setAttribute("style","stroke:black;fill:none;");
					   path.setAttribute("d", hershey.svgPath(this->name+"="+this->getWeight(), this->getTitleFrame()) );
					   path.setAttribute("title",this->category+":"+this->name+"="+this->getWeight());

					   g.appendChild(path);
					   
					   
					   
					   var L = [];
					   for(var k in this->children) L.push(this->children[k]);
					   packer.layout2(L,this->getChildrenFrame());
					   
					   for(var c in L)
						{
						var d = L[c].svg(packer);
						if(d==null) {
							continue;
							}
						g.appendChild(d);
						}
					   
					   }
				   else
					   {

					  
					   	var f_up= new Rectangle(
					   			insets.getX(),insets.getY(),
					   			insets.getWidth(),insets.getHeight()/2.0
					   			).inset(0.9);
					   	var p1= document.createElementNS(SVG,"svg:path");
					   	 g.appendChild(p1);
					   	p1.setAttribute("class","lbla"+(this->getDepth()%2));
						p1.setAttribute("d",hershey.svgPath(this->name,f_up));
						p1.setAttribute("style","stroke-width:"+ 
							(3.0/(this->getDepth()+1) )
							);

						var f_down=new Rectangle(
								insets.getX(),insets.getCenterY(),
								insets.getWidth(),insets.getHeight()/2.0
					   			);
					   	var p2= document.createElementNS(SVG,"svg:path");
					   	p2.setAttribute("class","lblb"+(this->getDepth()%2));
					   	p2.setAttribute("d",hershey.svgPath(""+this->getWeight(),f_down));
					   	g.appendChild(p2);
					   	 
						//w.writeAttribute("d", hershey.svgPath(convertWeightToString(),f_down) );
						//w.writeAttribute("class","lblb"+(getDepth()%2));
					   }
				   
				   return g;
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
            while (mid+1<end)
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
            while (mid+1<end)
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
 
 
xmlNodePtr TreePack::svg(Packer* packer) {
	var hershey = new Hershey();
	
	if( this->parent == null)
		{
		var svg = document.createElementNS(SVG,"svg:svg");
		svg.setAttribute("width",this->bounds.width);
		svg.setAttribute("height",this->bounds.height);
		
		
		var defs = document.createElementNS(SVG,"svg:defs");
		svg.appendChild(defs);
		
		var style = document.createElementNS(SVG,"svg:style");
		style.setAttribute("type","text/css");
		style.appendChild(document.createTextNode(
			"svg {fill:none;stroke:black;stroke-width:0.5px;}\n"+
			".r0 {fill:rgb(240,240,240);stroke:black;stroke-width:0.5px;}\n"+
			".r1 {fill:rgb(220,220,220);stroke:black;stroke-width:0.5px;}\n"+
			".lbla0 {stroke:black;stroke-width:1px;}\n"+
			".lblb0 {stroke:red;stroke-width:1px;}\n"+
			".lbla1 {stroke:gray;stroke-width:1px;}\n"+
			".lblb1 {stroke:red;stroke-width:1px;}\n"+
			""
			));

		defs.appendChild(style);
		
		var title = document.createElementNS(SVG,"svg:title");
		title.appendChild(document.createTextNode("TreeMap"));
		svg.appendChild(title);
		
		var rect =  document.createElementNS(SVG,"svg:rect");
		rect.setAttribute("x",0);
		rect.setAttribute("y",0);
		rect.setAttribute("width",(this->bounds.width-1));
		rect.setAttribute("height",(this->bounds.height-1));
		rect.setAttribute("style","fill:white;stroke:black;");
		svg.appendChild(rect);
	
		
		var g = document.createElementNS(SVG,"svg:g");
		svg.appendChild(g);
		
		var L = [];
		for(var k in this->children) L.push(this->children[k]);
	        packer.layout2(L,this->bounds);
		
		for(var c in L)
			{
			var d = L[c].svg(packer);
			if(d==null) continue;
			g.appendChild(d);
			}
		
		rect =  document.createElementNS(SVG,"svg:rect");
		rect.setAttribute("x",0);
		rect.setAttribute("y",0);
		rect.setAttribute("width",(this->bounds.width-1));
		rect.setAttribute("height",(this->bounds.height-1));
		rect.setAttribute("style","fill:none;stroke:black;");
		svg.appendChild(rect);
	        
		return svg;
		}
	else
		{
		
		if(this->getWeight()<=0)
			   {
			   return null;
			   }
		
		   var bounds = this->getBounds();
		   var insets = bounds.inset(0.9);
		   var frameUsed=bounds;
		   
		   
		   if(bounds.getWidth()<=1 || bounds.getHeight()<=1)
			   {
			   return null;
			   }
		   var g = document.createElementNS(SVG,"svg:g");

		   
		   

		   var rect =  document.createElementNS(SVG,"svg:rect");
		   g.appendChild(rect);
		   rect.setAttribute("x",frameUsed.x);
   		   rect.setAttribute("y",frameUsed.y);
   		   rect.setAttribute("width",frameUsed.width);
   		   rect.setAttribute("height",frameUsed.height);
   		   rect.setAttribute("class", "r"+(this->getDepth()%2));
   		   var title = document.createElementNS(SVG,"svg:title");
   		   rect.appendChild(title);
   		   title.appendChild( document.createTextNode(this->category+":"+this->name+"="+this->getWeight()));

		
		   if(!this->isLeaf())
			   {
			   var path =  document.createElementNS(SVG,"svg:path");
			   path.setAttribute("style","stroke:black;fill:none;");
			   path.setAttribute("d", hershey.svgPath(this->name+"="+this->getWeight(), this->getTitleFrame()) );
			   path.setAttribute("title",this->category+":"+this->name+"="+this->getWeight());

			   g.appendChild(path);
			   
			   
			   
			   var L = [];
			   for(var k in this->children) L.push(this->children[k]);
	                   packer.layout2(L,this->getChildrenFrame());
			   
			   for(var c in L)
				{
				var d = L[c].svg(packer);
				if(d==null) {
					continue;
					}
				g.appendChild(d);
				}
			   
			   }
		   else
			   {

			  
			   	var f_up= new Rectangle(
			   			insets.getX(),insets.getY(),
			   			insets.getWidth(),insets.getHeight()/2.0
			   			).inset(0.9);
			   	var p1= document.createElementNS(SVG,"svg:path");
			   	 g.appendChild(p1);
			   	p1.setAttribute("class","lbla"+(this->getDepth()%2));
				p1.setAttribute("d",hershey.svgPath(this->name,f_up));
				p1.setAttribute("style","stroke-width:"+ 
					(3.0/(this->getDepth()+1) )
					);

				var f_down=new Rectangle(
						insets.getX(),insets.getCenterY(),
						insets.getWidth(),insets.getHeight()/2.0
			   			);
			   	var p2= document.createElementNS(SVG,"svg:path");
			   	p2.setAttribute("class","lblb"+(this->getDepth()%2));
			   	p2.setAttribute("d",hershey.svgPath(""+this->getWeight(),f_down));
			   	g.appendChild(p2);
			   	 
				//w.writeAttribute("d", hershey.svgPath(convertWeightToString(),f_down) );
				//w.writeAttribute("class","lblb"+(getDepth()%2));
			   }
		   
		   return g;
		   }
	}
		 
#define SETATTRIBUTE(NODE,NAME,content) do {ostringstream _os; _os << content; xmlSetProp(NODE,NAME,_os.str()); } while(0)

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
			if(header.size()<=1)
				{
				THROW("expected at least two columns.");
				}

			RootTreePack root;
			xmlDocPtr dom = ::xmlNewDoc("1.0");
			xmlNsPtr svgns = ::xmlNewNs(dom, "",NULL);
			xmlNodePtr svgroot = xmlNewNode(svgns,"svg");
			SETATTRIBUTE(svgroot,"width",image_width);
			SETATTRIBUTE(svgroot,"height",image_height);
			
				
			int nLine=0;
			while(getline(in,line)) {
				++nLine;
				vector<std::string> tokens;
				splitter.split(line,tokens);
				if(tokens.size() != header.size()) {
					THROW("line " << nLine << ":\"" << line << "\". Expected " << header.size() << " tokens but got " << tokens.size());
					}
					
				double weight = parseDouble(tokens[tokens.length-1].c_str());
					
				DefaultTreePack* curr = &root;
				
				for(vector<std::string>::size_type i=0; i+1< tokens.size();++i) {
					std::string& key = tokens[i];
					std::map<std::string,DefaultTreePack*>::iterator r = curr->children.find(key);
					if( r == curr->children.end() )
						{
						DefaultTreePack* c= new DefaultTreePack;
						c->category.assign(header[i]);
						c->name.assign(key);
						c->parent = curr;
						c->weight = weight;
						curr->children.insert( make_pair(c->name,c) );
						curr = c;
						}
					else
						{
						DefaultTreePack* c= r->second;
						c->weight += weight;
						curr = c;
						}
					}

				}
			Packer packer;
			root.bounds.x=0;
			root.bounds.y=0;
			root.bounds.width=this->image_width;
			root.bounds.height=this->image_height;
			appendChild(svgroot,root.svg(&packer));
			
			::xmlDocDump(stdout,dom);
			
			xmlMemoryDump();
			}

#define WRITE_ATTRIBUTE(ATT,VALUE) { ostringstream _os; _os << VALUE; string _s = _os.str(); ::xmlTextWriterWriteAttribute(writer, BAD_CAST ATT, BAD_CAST _s.c_str());}
 
		

		
		virtual int main(int argc, char** argv) {
			int optind = parseOptions(argc,argv);
			oneOrStdin(optind,argc,argv);
			return EXIT_SUCCESS;
			}
	};


int main(int argc,char** argv)
	{
	LIBXML_TEST_VERSION
	TreePackApp app;
	return app.main(argc,argv);
	}

