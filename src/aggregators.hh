#ifndef AGGREGATORS_HH
#define AGGREGATORS_HH
#include <iostream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <set>
#include <vector>

class Aggregator
	{
	public:
		Aggregator() {}
		virtual ~Aggregator(){}
		virtual void visit(const std::string& s)=0;
		virtual void print(std::ostream& out)=0;
	};

class DoubleAggregator : public Aggregator
	{
	public:
		DoubleAggregator() {}
		virtual ~DoubleAggregator(){}
		virtual void visit(double v)=0;
		virtual void visit(const std::string& s) {
			char* p2=0;
			visit( std::strtod(s.c_str(),&p2));
			}
	};

class SimpleAggregator: public Aggregator
	{
	public:
		std::string value;
		virtual ~SimpleAggregator(){}
		virtual void visit(const std::string& s) { value.assign(s);}
		virtual void print(std::ostream& out) { out << this->value; }
	};

class FirstAggregator: public Aggregator
	{
	public:
		std::string value;
		bool seen;
		FirstAggregator():seen(false){}
		virtual ~FirstAggregator(){}
		virtual void visit(const std::string& s) {if(!seen) value.assign(s);seen=true;}
		virtual void print(std::ostream& out) {  out << this->value; }
	};

class SumAggregator: public DoubleAggregator
	{
	public:
		double total;
		SumAggregator():total(0){}
		virtual ~SumAggregator(){}
		virtual void visit(double v) {total+=v;}
		virtual void print(std::ostream& out) {  out << this->total; }
	};
class MeanAggregator: public DoubleAggregator
	{
	public:
		double total;
		int n;
		MeanAggregator():total(0),n(0){}
		virtual ~MeanAggregator(){}
		virtual void visit(double v) {total+=v;++n;}
		virtual void print(std::ostream& out) {  out << (this->total/n); }
	};

class MinAggregator: public DoubleAggregator
	{
	public:
		double value;
		bool first;
		MinAggregator():value(0),first(true){}
		virtual ~MinAggregator(){}
		virtual void visit(double d) {value=(first || d<value?d:value) ; first=false;}
		virtual void print(std::ostream& out) {  out << value; }
	};

class MaxAggregator: public DoubleAggregator
	{
	public:
		double value;
		bool first;
		MaxAggregator():value(0),first(true){}
		virtual ~MaxAggregator(){}
		virtual void visit(double d) {value=(first || d>value?d:value) ; first=false;}
		virtual void print(std::ostream& out) {  out << value; }
	};

class SetAggregator: public Aggregator
	{
	public:
		std::set<std::string> values;
		SetAggregator() {}
		virtual ~SetAggregator(){}
		virtual void visit(const std::string& s) {values.insert(s);}
		virtual void print(std::ostream& out) {
			bool first(true);
			for(std::set<std::string>::iterator r=values.begin();r!=values.end();++r)
				{
				if(!first) out <<";";
				first=false; 
				out << (*r);
				}
			}
	};

class MedianAggregator: public DoubleAggregator
	{
	public:
		std::vector<double> values;
		MedianAggregator() {}
		virtual ~MedianAggregator(){}
		virtual void visit(double d) {values.push_back(d);}
		virtual void print(std::ostream& out) {
			std::sort(values.begin(),values.end());
			unsigned int i=values.size()/2;
			if(values.size()%2==0)
				{
				out << (values[i-1]+values[i])/2.0;
				}
			else
				{
				out << values[i];
				}
			}
	};

#endif
