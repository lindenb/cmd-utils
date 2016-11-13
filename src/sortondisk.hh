#ifndef SORTONDISK_HH
#define SORTONDISK_HH
#include <vector>
#include <cstdio>
#include <cstring>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <algorithm>
#include "throw.hh"

template<typename T>
class SortingCollection
	{
	private:
		typedef T *ptr_type;
		typedef const T *const_ptr_type;
		class BuffFile
			{
			public:
				std::FILE* ios;
				typename SortingCollection<T>::ptr_type peeked;
				std::string filename;
				typename std::vector<SortingCollection<T>::ptr_type>::size_type nitems;
				typename std::vector<T*>::size_type curr_index;
				BuffFile():ios(0),peeked(0),nitems(0UL),curr_index(0UL)  {
					}
			};
			
		std::vector<ptr_type> buffer;
		typename std::vector<ptr_type>::size_type max_buffer_size;
		std::vector<BuffFile*> bufffiles;
		bool done_adding_flag;
		std::string tmpdir;
	public:
		class TupleBinding
			{
			public:
				TupleBinding() {}
				virtual ~TupleBinding() {}
				virtual SortingCollection<T>::ptr_type read(std::FILE* in)=0;
				virtual void write(std::FILE* in,SortingCollection<T>::const_ptr_type o)=0;
			};		
		class Comparator
			{
			public:
				Comparator() {}
				virtual ~Comparator() {}
				virtual bool lt( SortingCollection<T>::const_ptr_type a, SortingCollection<T>::const_ptr_type b) = 0;
			};
		class Iterator
			{
			public:
				Iterator() {}
				virtual ~Iterator() {}
				virtual SortingCollection<T>::ptr_type next()=0;
				virtual void close()=0;
			};
	private:
		SortingCollection<T>::Comparator* comparator;
		SortingCollection<T>::TupleBinding* binding;
		
		BuffFile* createBuffFile() {
			BuffFile* bf = new BuffFile();
			bf->filename.append(this->tmpdir);
			bf->filename.append("/");
			bf->filename.append("sorting.XXXXXX");
			int fd = ::mkstemp((char*)bf->filename.c_str());
			//std::cerr << "fd = "<< fd << std::endl;
			if( fd == -1)  THROW("Cannot open " << bf->filename << " " << std::strerror(errno));
			bf->ios = ::fdopen(fd,"wb");
			if( bf->ios == 0)  THROW("Cannot open "<< bf->filename << " " << std::strerror(errno));
			bf->nitems = 0UL;
			bufffiles.push_back(bf);
			return bf;
			}
		
		struct less_than_key
			{
			SortingCollection<T>* owner;
			less_than_key(SortingCollection<T>* owner) {
				this->owner = owner;
				}
			bool operator() (SortingCollection<T>::const_ptr_type a, SortingCollection<T>::const_ptr_type b)
				{
					return owner->comparator->lt(a,b);
				}
			};
		
		void flush_buffer_to_disk() {
			if(buffer.empty()) return;
			less_than_key ltk(this);
			std::sort(buffer.begin(),buffer.end(),ltk);
			
			BuffFile* outBuffFile = createBuffFile();
			
			typename std::vector<ptr_type>::iterator r = buffer.begin();
			while( r != buffer.end() ) {
				ptr_type item = (*r);
				binding->write(outBuffFile->ios,item);
				outBuffFile->nitems++;
				delete item;
				++r;
				}
			buffer.clear();
			std::fflush(outBuffFile->ios);
			std::fclose(outBuffFile->ios);
			outBuffFile->ios=0;
			}
	
	public:
		void add(T* t)
			{
			if(done_adding_flag) THROW("boum");
			if( t == 0) THROW("boum");
			if( max_buffer_size<1UL) THROW("boum");
			if( this->comparator == 0) THROW("boum");
			if( this->binding == 0) THROW("boum");
			buffer.push_back(t);
			if( buffer.size() > max_buffer_size) {
				flush_buffer_to_disk();
				}
			}
			
		void close() {
			done_adding_flag=true;
			for(typename std::vector<T*>::iterator r= buffer.begin();
				r!=buffer.end();
				++r)
				{
				delete *r;
				}
			buffer.clear();
			for(typename std::vector<BuffFile*>::iterator r= bufffiles.begin();
				r!=bufffiles.end();
				++r)
				{
				BuffFile* bf=(*r);
				//std::cerr << "closingA" << std::endl;
				if(bf->peeked != 0) delete bf->peeked;
				bf->peeked=0;
				//std::cerr << "closingB" << (bf->ios==0) << std::endl;
				if(bf->ios != 0) std::fclose(bf->ios);
				bf->ios=0;
				bf->curr_index = bf->nitems;
				//std::cerr << "closingC" << std::endl;
				if(!bf->filename.empty()) std::remove(bf->filename.c_str());
				}
			}
			
		class IteratorImpl: public Iterator
			{
			public:
				SortingCollection<T> *owner;
				IteratorImpl(SortingCollection<T> *owner):owner(owner) {}
				virtual ~IteratorImpl() {
					owner->close();
					}
				virtual SortingCollection<T>::ptr_type next() {
					T* best = 0;

					typename std::vector<SortingCollection<T>::BuffFile*>::iterator rbest = owner->bufffiles.end();
					for(typename std::vector<SortingCollection<T>::BuffFile*>::iterator r= owner->bufffiles.begin();
						r!=owner->bufffiles.end();
						++r)
						{
						SortingCollection<T>::BuffFile* bf=(*r);
						if( bf->peeked == 0)
							{
							if( bf->curr_index >= bf->nitems) continue;
							bf->peeked = owner->binding->read( bf->ios );
							bf->curr_index++;
							if ( bf->curr_index == bf->nitems ) {
								std::fclose(bf->ios);
								bf->ios=0;
								std::remove(bf->filename.c_str());
								bf->filename.clear();
								}
							}
						if( best == NULL || owner->comparator->lt(bf->peeked,best) )
							{
							best = bf->peeked  ;
							rbest = r;
							}
						}
					if(best!=0) {
						(*rbest)->peeked=0;
						}
					return best;
					}
				virtual void close()
					{
					owner->close();
					}
			};	
		
		IteratorImpl* myiterator;
		
		Iterator* iterator() {
			if(done_adding_flag) THROW("boum");
			if(myiterator!=0) return myiterator;
			done_adding_flag=true;
			flush_buffer_to_disk();
			for(typename std::vector<BuffFile*>::iterator r= bufffiles.begin();
				r!=bufffiles.end();
				++r)
				{
				(*r)->ios = std::fopen((*r)->filename.c_str(),"rb");
				if( (*r)->ios == NULL) {
					close();
					THROW("cannot reopen tmp file");
					}
				(*r)->curr_index = 0UL;
				}
			myiterator = new IteratorImpl(this);
			return myiterator;
			}
		
		
		
		SortingCollection():max_buffer_size(100000UL),
						done_adding_flag(false),
						comparator(0),binding(0),
						myiterator(0) {
			char const *tmpfolder = std::getenv("TMPDIR");
			tmpdir.assign(tmpfolder==0?".":tmpfolder);
			}
		~SortingCollection()
			{
			if(myiterator!=0) delete myiterator;
			close();
			
			for(typename std::vector<BuffFile*>::iterator r= bufffiles.begin();
				r!=bufffiles.end();
				++r)
				{
				BuffFile* bf=(*r);
				delete bf;
				}

			}
		
		void set_comparator(SortingCollection<T>::Comparator* c)
			{
			this->comparator = c;
			}
		void set_binding(SortingCollection<T>::TupleBinding* t)
			{
			this->binding = t;
			}
		void set_max_itemsinram(unsigned int n) {
			this->max_buffer_size = n;
		}
		
	};

#endif

