#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <vector>
#include <list>
#include <algorithm>
#include <getopt.h>

using namespace std;

static int scan(FILE* in,unsigned int win,bool use_average) {
	list<double> values;
	unsigned int values_len=0U;
	while(!feof(in))
				{
				double num;
				if(fscanf(in,"%lf",&num)!=1) {
					int c;
					if((c=std::fgetc(in))==EOF) break;
					fprintf(stderr,"Bad number, got character (%c)\n", c);
					return EXIT_FAILURE;
					}
				values.push_back(num);
				values_len++;
				if(values_len>=win) {
					vector<double> a;
					list<double>::iterator r= values.begin();
					for(unsigned int i=0;i< win ;i++)
						{
						a.push_back(*r);
						r++;
						}
					sort(a.begin(),a.end());
					std::vector<double>::size_type mid = a.size() /2;
					double median;
					if( use_average) {
						double sum=0;
						for(std::vector<double>::size_type i=0;i< a.size();i++)
							{
							sum+=a[i];
							}
						median=sum/a.size();
						}
					else if( a.size() % 2 == 0)
						{
						median= (a[ mid-1 ] + a[ mid ] )/2.0 ;
						}
					else
						{
						median= a[ mid ];
						}

					printf("%f\n",median);
					
					values.pop_front();
					values_len--;
					}
				}
	return EXIT_SUCCESS;
	}

int main(int argc,char** argv) {
 int opt = 0;
 int win = 0;
 bool use_average=false;
 while (( opt = getopt(argc, argv, "w:a")) != -1){
   switch (opt) {
   	 case 'a':
   	 	use_average=true;
   	 	break;
     case 'w':
       win = atoi(optarg);
       break;
     default:
       fprintf(stderr,"Bad option:%c\n",optopt);
       return EXIT_FAILURE;
     }
   }
  if(win <=0) {
  	fprintf(stderr,"Bad window size.\n");
  	return EXIT_FAILURE;
  	}
  if(optind==argc)
	{
	return scan(stdin,(unsigned int)win,use_average);
	}
  else
    {
    FILE* in = fopen(argv[optind],"r");
    if(in==NULL) {
    	fprintf(stderr,"Cannot open %s (%s).\n",argv[optind],strerror(errno));
    	return EXIT_FAILURE;
    	}
    int err = scan(in,(unsigned int)win,use_average);
    fclose(in);
    return err;
    }
}
