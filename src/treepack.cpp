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


struct Rectangle {
		double x;
		double y;
		double width;
		double height;
		Rectangle(){}
		Rectangle(double x,double y,double width,double height):x(x),y(y),width(width),height(height) {}
		double getWidth() const { return this->width;}
		double getHeight() const { return this->height;}
		double getX() const { return this->x;}
		double getY() const { return this->y;}
		
		Rectangle inset(double ratio) {
			double r1_width= this->getWidth()*ratio;
			double r1_x= this->getX()+(this->getWidth()-r1_width)/2;
			double r1_height= this->getHeight()*ratio;
			double r1_y=this->getY()+(this->getHeight()-r1_height)/2;
			Rectangle r1(r1_x,r1_y,r1_width,r1_height);
			return r1;
			}
		
		Rectangle inset() { return this->inset(0.9);}
		double getMidX() const { return (double)(getX()+getWidth()/2.0);}
		double getMidY() const { return (double)(getY()+getHeight()/2.0);}
		double getCenterX() const { return getMidX();}
		double getCenterY() const { return getMidY();}
	};

/*
static std::ostream& operator<<(std::ostream& out, const Rectangle& r)
	{
	out << "rect(" << r.x << "," << r.y << "," << r.width << "," << r.height <<")";
	return out;
	}
*/


enum Orientation { VERTICAL, HORIZONTAL};
enum Direction { ASCENDING, DESCENDING};






class TreePack
	{
	public:
		std::string name;
		std::string category;
		TreePack* parent;
		std::map<std::string,TreePack*> children;
		Rectangle bounds;
		double weight;

		TreePack():parent(NULL)
			{
			}

		virtual ~TreePack() {
			for(std::map<std::string,TreePack*>::iterator r= children.begin();
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
			TreePack* curr = this;
			while(curr->parent!=NULL) {
				++d;
				curr = curr->parent;
				}
			return d;
			}
		
		Rectangle getTitleFrame()
			{
			Rectangle r= this->bounds; 
			Rectangle frame;
			frame.height=r.getHeight()*0.1;
			frame.y=r.getY();
			frame.width=r.getWidth();
			frame.x=r.getX();
			return frame.inset(0.9);
			}
		
		virtual void setBounds(const Rectangle& bounds)	 {
			this->bounds  = bounds;
			}
	
		virtual Rectangle getBounds()	 {
			return bounds;
			}
		
		virtual Rectangle getChildrenFrame()	 {
			Rectangle r= this->bounds; 
			Rectangle frame;
			frame.height=r.getHeight()*0.9;//yes again after
			frame.y=r.y+r.height-frame.height;
			frame.height=r.getHeight()*0.85;//yes again 
			frame.width=r.getWidth()*0.95;
			frame.x=r.getX()+(r.getWidth()-frame.width)/2.0;
			return frame;
			}
			
	
		virtual double  getWeight() const	 {
			if(isLeaf() )
				{
				return weight;
				}
			else
				{
				double w= 0.0;
		
				for(std::map<std::string,TreePack*>::const_iterator r= children.begin();
					r!= children.end();
					++r)
					{
					w += r->second->getWeight();
					}
				return w;
				}
			}

		};

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

#define SETATTRIBUTE(NODE,NAME,CONTENT) do {if(NODE!=0){ostringstream _os; _os << CONTENT; string _s(_os.str());  ::xmlSetProp(NODE,BAD_CAST NAME,BAD_CAST _s.c_str()); }} while(0)

class TreePackApp: public TreePackBase
	{
	public:
		xmlNsPtr SVG;
		Hershey hershey;
		TreePackApp():SVG(NULL) {}
		virtual ~TreePackApp() {}
		
		std::string svg(std::string s,Rectangle r)
			{
			if(s.size()==0 || r.width<=0 || r.height<=0) {
				return "";
				}
			double dx=r.width/s.size();
			if( dx > r.height + 1.0)
				{
				double len = s.size()*r.height;
				double mid = r.x+ r.width/2.0;
				Rectangle r2(mid-len/2.0,r.y,len,r.height);
				return svg(s,r2);
				}
			if( r.height > r.width )
				{
				double nh= r.width - 1.0;
				double mid = r.y+r.height/2.0;
				Rectangle r2(r.x,mid-nh/2.0,r.width,nh);
				return svg(s,r2);
				} 
			

			return hershey.svg(s.c_str(),r.x,r.y,r.width,r.height);
			}
		
		double parseDouble(const string& s) {
			char* p2=NULL;
			double v = strtod(s.c_str(),&p2);
			while(isspace(*p2)) ++p2;
			if(*p2!=0) THROW("BAD number "<< s);
			return v;
			}
		
		double fromUniq(std::string& s)
			{
			long n=0;
			size_t i=0;
			while(i<s.size() && s[i]==' ') ++i;
			while(i<s.size() && isdigit(s[i])) { n= n*10 + s[i]-'0';++i;}
			if(n==0 || i>=s.size() ||  s[i]!=' ') THROW("expected a space after number in " << s);
			s.erase(0,i);
			return (double)n;
			}

		xmlNodePtr svg(xmlDocPtr dom,TreePack* node,Packer* packer)
			{
			if( node->parent == NULL)
				{
				xmlNodePtr svg = ::xmlNewDocNode(dom,NULL,BAD_CAST"svg",BAD_CAST NULL);
				//xmlNodePtr svgroot = ::xmlNewNode( NULL,BAD_CAST"svg");
				this->SVG = ::xmlNewNs(svg, BAD_CAST"http://www.w3.org/2000/svg",BAD_CAST NULL);
				::xmlDocSetRootElement(dom,svg); 
				SETATTRIBUTE(svg,"width",this->image_width);
				SETATTRIBUTE(svg,"height",this->image_height);
			
				

		
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
				for(map<string,TreePack*>::iterator k=node->children.begin();
					k!=node->children.end();
					++k
					) {
					L.push_back(k->second);
					}
				
				packer->layout(&L,node->bounds);
		
				for(vector<TreePack*>::size_type c=0;c< L.size();++c)
					{
					xmlNodePtr d = this->svg(dom,(TreePack*)L[c],packer);
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
		   		   {
		   		     ostringstream _os;
		   		     _os <<  "r"<<(node->getDepth()%2);
		   		     string _s(_os.str());
		   		   SETATTRIBUTE(rect,"class",_s);
		   		   }
		   		   xmlNodePtr title = ::xmlNewNode(SVG,BAD_CAST"title");
		   		   
		   		    ::xmlAddChild(rect,title);
		   		   

		   		   
		   		    {
		   		    ostringstream _os;
		   		    _os << node->category<<":"<<node->name<<"="<<node->getWeight();
		   		    string _s(_os.str());
		   		    ::xmlAddChild(title,::xmlNewText(BAD_CAST _s.c_str()));
					}
				
				   if(!node->isLeaf())
					   {
					   xmlNodePtr path = ::xmlNewNode(SVG,BAD_CAST"path");
					   SETATTRIBUTE(path,"style","stroke:black;fill:none;");
					   
					   {
					    ostringstream _os;
					    _os << node->name<<"="<<node->getWeight();
					   SETATTRIBUTE(path,"d", svg(_os.str(), node->getTitleFrame()) );
					   }
					   
					   {
					   ostringstream _os;
					   _os << node->category<<":"<<node->name+"="<<node->getWeight();
					   SETATTRIBUTE(path,"title",_os.str());
					   }

					   ::xmlAddChild(g,path);
					   
					   
					   vector<TreePack*> L;
					  for(map<string,TreePack*>::iterator k=node->children.begin();
							k!=node->children.end();
							++k
							)
							{
					   		L.push_back(k->second);
					   		}
					   packer->layout(&L,node->getChildrenFrame());
					   
					   for(size_t  c=0;c<L.size();++c)
						{
						xmlNodePtr d = svg(dom,(TreePack*) L[c],packer);
						if(d==NULL) {
							continue;
							}

						::xmlAddChild(g,d);
						}
					 
					   }
				   else
					   {
					  
					   	Rectangle f_up(
					   			insets.getX(),insets.getY(),
					   			insets.getWidth(),insets.getHeight()/2.0
					   			);
					   	f_up = f_up.inset(0.9);
					   	xmlNodePtr p1= ::xmlNewNode(SVG,BAD_CAST"path");

					   	::xmlAddChild(g,p1); 
					   	 
					   	SETATTRIBUTE(p1,"class","lbla"<<(node->getDepth()%2));
						SETATTRIBUTE(p1,"d",svg(node->name,f_up));
						SETATTRIBUTE(p1,"style","stroke-width:"<<
							(3.0/(node->getDepth()+1) )
							);

						Rectangle f_down(
								insets.getX(),insets.getCenterY(),
								insets.getWidth(),insets.getHeight()/2.0
					   			);
					   	xmlNodePtr p2= ::xmlNewNode(SVG,BAD_CAST"path");
					   	SETATTRIBUTE(p2,"class","lblb"<<(node->getDepth()%2));
					   	
					   	{
					   	 ostringstream _os;
					   	 _os << node->getWeight();
					   	 string _s(_os.str());
					   	SETATTRIBUTE(p2,"d",svg(_s,f_down));
					   	}
					   	
					   	::xmlAddChild(g,p2); 
					  

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
			if(input_from_unique) {
				 fromUniq(line);
				}
			splitter.split(line,header);
			if(header.size()<=1 && !input_from_unique)
				{
				THROW("expected at least two columns.");
				}

			TreePack root;
			xmlDocPtr dom = ::xmlNewDoc(BAD_CAST "1.0");
			
			
				
			int nLine=0;
			while(getline(in,line)) {
				++nLine;
				double weight ;
				vector<std::string> tokens;
				if(input_from_unique)
					{
					weight = fromUniq(line);
					}
				
				splitter.split(line,tokens);
				if(tokens.size() != header.size()) {
					THROW("line " << nLine << ":\"" << line << "\". Expected " << header.size() << " tokens but got " << tokens.size());
					}
					
					
				if(!input_from_unique)
					{
					weight = parseDouble(tokens[tokens.size()-1].c_str());
					}
				if( weight <= 0 ) {
					continue;
					}		
				TreePack* curr = &root;
				
				for(vector<std::string>::size_type i=0;
					i+(input_from_unique?0:1) < tokens.size();
					++i) {
					std::string& key = tokens[i];
					std::map<std::string,TreePack*>::iterator r = curr->children.find(key);
					if( r == curr->children.end() )
						{
						TreePack* c= new TreePack;
						c->category.assign(header[i]);
						c->name.assign(key);
						c->parent = curr;
						c->weight = weight;
						curr->children.insert( make_pair(c->name,c) );
						curr = c;
						}
					else
						{
						TreePack* c= r->second;
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
			svg(dom,&root,&packer);
			
			FILE* out = stdout;
			if( output_name!=NULL)
				{
				out = fopen(output_name,"w");
				if( out == NULL) {
					THROW("Cannot open " << output_name << ". " << strerror(errno) );
					}
				
				}
			::xmlDocDump(out,dom);
			fflush(out);
			if( output_name!=NULL)
				{
				fclose(out);
				}
			::xmlFreeDoc(dom);
			::xmlMemoryDump();
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

