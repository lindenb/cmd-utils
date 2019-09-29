#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <vector>
#include <list>
#include <algorithm>
#include <getopt.h>

using namespace std;

static int dbl_compare(const void * a, const void * b) {
  if (*(double*)a > *(double*)b) return 1;
  else if (*(double*)a < *(double*)b) return -1;
  else return 0;  
}

static int scan(FILE* in,unsigned int win,unsigned int shift,bool use_average) {
	list<double> values;
	bool got_data = false;
	unsigned int values_len=0U;
	double* a;
	a = (double*)malloc(sizeof(double)*win);
	if(a==NULL) {
		fprintf(stderr,"out of memory\n");
		return EXIT_FAILURE;
		}
	int mid = win /2;
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
					list<double>::iterator r= values.begin();
					for(unsigned int i=0;i< win ;i++)
						{
						a[i] = (*r);
						r++;
						}
					
					double median;
					if( use_average) {
						double sum=0;
						for(unsigned int i=0;i< win;i++)
							{
							sum+=a[i];
							}
						median=sum/win;
						}
					else
						{
						qsort(a,win,sizeof(double),dbl_compare);
						if( win % 2 == 0)
							{
							median= (a[ mid-1 ] + a[ mid ] )/2.0 ;
							}
						else
							{
							median= a[ mid ];
							}
						}
					printf("%f\n",median);
					got_data = true;
					for(unsigned int i=0; i< shift;i++) {
						values.pop_front();
						values_len--;
						}
					}
				}
	free(a);
	if(!got_data) {
		fprintf(stderr,"No Data was displayed\n");
		return EXIT_FAILURE;
		}
	return EXIT_SUCCESS;
	}

int main(int argc,char** argv) {
 int opt = 0;
 int win = 0;
 int shift=-1;
 bool use_average=false;
 while (( opt = getopt(argc, argv, "w:s:ah")) != -1){
   switch (opt) {
     case 'h':
		printf("%s.\nUsage\n  -a: use average instead of median\n  -w window-size (required)\n\n  -s shift-size (-1 is same as window-size)\n\n",argv[0]);
		return EXIT_FAILURE;
     case 'a':
   	 	use_average=true;
   	 	break;
     case 'w':
       win = atoi(optarg);
       break;
     case 's':
       shift = atoi(optarg);
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
  if(shift <=0) {
  	shift = win;
  	}
  else if(shift > win ) {
  	fprintf(stderr,"shift cannot be greater than window size.\n");
  	return EXIT_FAILURE;
  	}
  if(optind==argc)
	{
	return scan(stdin,(unsigned int)win,(unsigned int)shift,use_average);
	}
  else
    {
    FILE* in = fopen(argv[optind],"r");
    if(in==NULL) {
    	fprintf(stderr,"Cannot open %s (%s).\n",argv[optind],strerror(errno));
    	return EXIT_FAILURE;
    	}
    int err = scan(in,(unsigned int)win,(unsigned int)shift,use_average);
    fclose(in);
    return err;
    }
}
