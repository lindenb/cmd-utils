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
		T getCenterX() const { return getMidX();}
		T getCenterY() const { return getMidY();}
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
		virtual bool isLeaf() const {
			return this->children.empty();
			}
		virtual int getDepth() {
			int d=0;
			DefaultTreePack* curr=this;
			while(curr->parent!=NULL) {
				++d;
				curr = curr->parent;
				}
			return d;
			}
		Rectangle getTitleFrame()
		{//TODO
		Rectangle r;
		return r;
		}
		virtual void setBounds(const Rectangle& bounds)	 {
			this->bounds  = bounds;
			}
	
		virtual Rectangle getBounds()	 {
			return bounds;
			}
				virtual Rectangle getChildrenFrame()	 {
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

#define SETATTRIBUTE(NODE,NAME,content) do {ostringstream _os; _os << content; ::xmlSetProp(NODE,BAD_CAST NAME,BAD_CAST _os.str().c_str()); } while(0)

class TreePackApp: public TreePackBase
	{
	public:
		xmlNsPtr SVG;
		TreePackApp():SVG(NULL) {}
		virtual ~TreePackApp() {}
		
		double parseDouble(const string& s) {
			return 0.0;
			}

		xmlNodePtr svg(DefaultTreePack* node,Packer* packer)
			{
			Hershey hershey;
	
			if( node->parent == NULL)
				{
				xmlNodePtr svg = ::xmlNewNode(SVG,BAD_CAST "svg");
				SETATTRIBUTE(svg,"width",node->bounds.width);
				SETATTRIBUTE(svg,"height",node->bounds.height);
		
		
				xmlNodePtr defs = ::xmlNewNode(SVG,BAD_CAST "defs");
				::xmlAddChild(svg,defs);
		
				xmlNodePtr style = ::xmlNewNode(SVG,BAD_CAST "style");
				SETATTRIBUTE(style,"type","text/css");
				::xmlAddChild(style,::xmlNewText(BAD_CAST 
					"svg {fill:none;stroke:black;stroke-width:0.5px;}\n"
					".r0 {fill:rgb(240,240,240);stroke:black;stroke-width:0.5px;}\n"
					".r1 {fill:rgb(220,220,220);stroke:black;stroke-width:0.5px;}\n"
					".lbla0 {stroke:black;stroke-width:1px;}\n"
					".lblb0 {stroke:red;stroke-width:1px;}\n"
					".lbla1 {stroke:gray;stroke-width:1px;}\n"
					".lblb1 {stroke:red;stroke-width:1px;}\n"
					""
					));
				::xmlAddChild(defs,style);
		
				xmlNodePtr title = ::xmlNewNode(SVG,BAD_CAST "title");
				::xmlAddChild(title,::xmlNewText(BAD_CAST "TreeMap"));
				::xmlAddChild(svg,title);
		
				xmlNodePtr rect = ::xmlNewNode(SVG,BAD_CAST "rect");
				SETATTRIBUTE(rect,"x",0);
				SETATTRIBUTE(rect,"y",0);
				SETATTRIBUTE(rect,"width",(node->bounds.width-1));
				SETATTRIBUTE(rect,"height",(node->bounds.height-1));
				SETATTRIBUTE(rect,"style","fill:white;stroke:black;");
				::xmlAddChild(svg,rect);
	
		
				xmlNodePtr g = ::xmlNewNode(SVG,BAD_CAST "svg");
				::xmlAddChild(svg,g);
		
				vector<TreePack*> L;
				for(map<string,DefaultTreePack*>::iterator k=node->children.begin();
					k!=node->children.end();
					++k
					) {
					L.push_back(k->second);
					}
				
				packer->layout(&L,node->bounds);
		
				for(size_t c=0;c< L.size();++c)
					{
					xmlNodePtr d = this->svg((DefaultTreePack*)L[c],packer);
					if(d==NULL) continue;
					::xmlAddChild(g,d);
					}
		
				rect =   ::xmlNewNode(SVG,BAD_CAST "rect");
				SETATTRIBUTE(rect,"x",0);
				SETATTRIBUTE(rect,"y",0);
				SETATTRIBUTE(rect,"width",(node->bounds.width-1));
				SETATTRIBUTE(rect,"height",(node->bounds.height-1));
				SETATTRIBUTE(rect,"style","fill:none;stroke:black;");
				::xmlAddChild(svg,rect);
			
				return svg;
				}
			else
				{
		
				if(node->getWeight()<=0)
					   {
					   return NULL;
					   }
		
				   Rectangle bounds = node->getBounds();
				   Rectangle insets = bounds.inset(0.9);
				   Rectangle frameUsed=bounds;
				   
				   
				   if(bounds.getWidth()<=1 || bounds.getHeight()<=1)
					   {
					   return NULL;
					   }
				   xmlNodePtr g = ::xmlNewNode(SVG,BAD_CAST "g");

				   
				   

				   xmlNodePtr rect =  ::xmlNewNode(SVG,BAD_CAST "rect");
				   ::xmlAddChild(g,rect);
				   SETATTRIBUTE(rect,"x",frameUsed.x);
		   		   SETATTRIBUTE(rect,"y",frameUsed.y);
		   		   SETATTRIBUTE(rect,"width",frameUsed.width);
		   		   SETATTRIBUTE(rect,"height",frameUsed.height);
		   		   SETATTRIBUTE(rect,"class", "r"+(node->getDepth()%2));
		   		   xmlNodePtr title = ::xmlNewNode(SVG,BAD_CAST"title");
		   		   
		   		    ::xmlAddChild(rect,title);
		   		   
		   		    ::xmlAddChild(title,rect);
		   		   
		   		    {
		   		    ostringstream _os;
		   		    _os << node->category<<":"<<node->name<<"="<<node->getWeight();
		   		    ::xmlAddChild(title,::xmlNewText(BAD_CAST _os.str().c_str()));
					}
		
				   if(!node->isLeaf())
					   {
					   xmlNodePtr path = ::xmlNewNode(SVG,BAD_CAST"path");
					   SETATTRIBUTE(path,"style","stroke:black;fill:none;");
					   
					   {
					    ostringstream _os;
					    _os << node->name<<"="<<node->getWeight();
					   SETATTRIBUTE(path,"d", hershey.svgPath(_os.str(), node->getTitleFrame()) );
					   }
					   
					   {
					   ostringstream _os;
					   _os << node->category<<":"<<node->name+"="<<node->getWeight();
					   SETATTRIBUTE(path,"title",_os.str());
						}

					   ::xmlAddChild(g,path);
					   
					   
					   vector<TreePack*> L;
					  for(map<string,DefaultTreePack*>::iterator k=node->children.begin();
							k!=node->children.end();
							++k
							)
							{
					   		L.push_back(k->second);
					   		}
					   packer->layout(&L,node->getChildrenFrame());
					   
					   for(size_t  c=0;c<L.size();++c)
						{
						xmlNodePtr d = svg((DefaultTreePack*) L[c],packer);
						if(d==NULL) {
							continue;
							}

						::xmlAddChild(g,d);
						}
					   
					   }
				   else
					   {

					  
					   	Rectangle f_up= Rectangle(
					   			insets.getX(),insets.getY(),
					   			insets.getWidth(),insets.getHeight()/2.0
					   			).inset(0.9);
					   	xmlNodePtr p1= ::xmlNewNode(SVG,BAD_CAST"path");

					   	::xmlAddChild(g,p1); 
					   	 
					   	SETATTRIBUTE(p1,"class","lbla"+(node->getDepth()%2));
						SETATTRIBUTE(p1,"d",hershey.svgPath(node->name,f_up));
						SETATTRIBUTE(p1,"style","stroke-width:"+ 
							(3.0/(node->getDepth()+1) )
							);

						Rectangle f_down(
								insets.getX(),insets.getCenterY(),
								insets.getWidth(),insets.getHeight()/2.0
					   			);
					   	xmlNodePtr p2= document.createElementNS(SVG,"svg:path");
					   	SETATTRIBUTE(p2,"class","lblb"+(node->getDepth()%2));
					   	SETATTRIBUTE(p2,"d",hershey.svgPath(""+node->getWeight(),f_down));
					   	::xmlAddChild(g,p2); 
					   	 
						//w.writeAttribute("d", hershey.svgPath(convertWeightToString(),f_down) );
						//w.writeAttribute("class","lblb"+(getDepth()%2));
					   }
				   
				   return g;
				   }
			}

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
			xmlDocPtr dom = ::xmlNewDoc(BAD_CAST "1.0");
			xmlNsPtr svgns = NULL;//::xmlNewNs(dom, BAD_CAST"",NULL);
			xmlNodePtr svgroot = xmlNewNode(svgns,BAD_CAST"svg");
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
					
				double weight = parseDouble(tokens[tokens.size()-1].c_str());
					
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
			svg(&root,&packer);

			
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

