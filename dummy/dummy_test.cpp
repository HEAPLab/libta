#include "libta.h"

#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>

namespace libta {
	template <typename T>
    class MyTimingAnalyzer : public TimingAnalyzer<T> {

    public:

        virtual std::shared_ptr<Response> perform_analysis(std::shared_ptr<Request<T>> req) {

        	std::shared_ptr<ResponsePWCET<T>> rwcet = std::make_shared<ResponsePWCET<T>>();

            const auto &exec_times = req->get_all();

            // Do something
            //INIT
            T maxET = 0 ;
            int file_size = exec_times.size();
            int file_sizeCV = file_size; 
            int half_size =floor(file_size/2);
            int half_sizeCV =floor(file_size/2);
            int mincv = 50 ;
            int minvalues = 10 ; 
            int nelems =  0;
            int bestcv = 0 ;
            int rank_lenght = 1000000;
		    T pWCET03_15[5];
			T pWCET03_15low[5];
			T pWCET03_15high[5];

			//MALLOC
            T * ETtrace = (T * )malloc(file_size * sizeof(T));
            T * trace_sorted = (T * )malloc(file_size * sizeof(T));
            T * trace_sortedCV = (T * )malloc(file_sizeCV * sizeof(T));
            T * cv = (T * )malloc( (file_sizeCV - half_sizeCV) * sizeof(T));
            T * limitabove = (T *)malloc((file_sizeCV - half_sizeCV) * sizeof(T));
	 		T * limitbelow =(T *)malloc((file_sizeCV - half_sizeCV) * sizeof(T));
	 		T * excesses = NULL;
	 		T * rank =(T *) malloc(rank_lenght * sizeof(T));
	 		T * probCCDF = (T *) malloc(rank_lenght * sizeof(T));
			T * probCCDFlow =  (T *) malloc(rank_lenght * sizeof(T)); 
			T * probCCDFhigh = (T *) malloc(rank_lenght * sizeof(T)); 

			
            //TRANSLATE
            for(int i=0;i<exec_times.size();i++){
            	ETtrace[i] = exec_times.at(i);
            }

            //CALCULATE
            maxET = getMax(ETtrace, file_size);
            memcpy(trace_sorted ,ETtrace ,file_size * sizeof(T));
            qsort((void*)(trace_sorted),file_size, sizeof( T), cmpSort);
            memcpy(trace_sortedCV,trace_sorted,file_sizeCV * sizeof(T));
            memcpy(cv , &(trace_sortedCV)[half_sizeCV] , (file_sizeCV - half_sizeCV) * sizeof(T));
            setMinValues(cv, minvalues -1  );

            memcpy(limitabove,cv, (file_sizeCV -half_sizeCV) * sizeof(T));
			memcpy(limitbelow,cv, (file_sizeCV - half_sizeCV) * sizeof(T));
			setCV(half_size, minvalues , trace_sortedCV , cv);
			
			setLimit(limitabove,limitbelow,half_size-minvalues + 1, half_sizeCV) ;
			int startpos = getStartPos(cv,limitabove,half_size,minvalues); ;

			if(startpos < half_size - mincv-1){
				for(int i = startpos ; i<half_size-mincv+1 ;i++){
					if( absD(cv[i+minvalues-1]-1) < absD(bestcv-1) ){
						if(cv[i+minvalues-1] < limitabove[i]){
							bestcv=cv[i+minvalues-1];
	                		nelems=half_size-i;	
						}
					}
				}
			}
			for(int  i = (half_sizeCV-mincv+1) ; i<(half_sizeCV-minvalues+1); i++ ){
				  if (cv[(i+minvalues-1)] > limitabove[i]) {
		  			 bestcv=1000;
  					 nelems=0;
		  			}
			}
		 
			if(nelems>0){
				  excesses = (T *)malloc((nelems)*sizeof(T));
			      memADD( excesses , trace_sorted , -(trace_sorted)[(nelems)-1] , nelems);
				  T rate = 1/(getMean(excesses,nelems));
				  T ratelow = rate * (1 + (1.96/sqrt(nelems)));
				  T ratehigh = rate *(1 - (1.96/sqrt(nelems)));

				  T rank_end =20 * getMax(excesses,nelems) ;
				  T rank_start = 0 ; 
				  T rank_step = (rank_end - rank_start)/ (rank_lenght-1);
				  arange(rank,rank_lenght,rank_start,rank_end,rank_step);
				  setExponSurvivalFunction(probCCDF,rank,rank_lenght, rate);
				  setExponSurvivalFunction(probCCDFlow,rank,rank_lenght, ratelow);
				  setExponSurvivalFunction(probCCDFhigh,rank,rank_lenght, ratehigh);
				  T mean = getMean(trace_sorted, file_size);
				  memADD(rank,rank,mean,rank_lenght); 

				  for(int i=0;i<rank_lenght;i++){
				  	rwcet->add_value_rank(rank[i]);
				  	rwcet->add_value_probCCDF(probCCDF[i]);
				  	rwcet->add_value_probCCDFlow(probCCDFlow[i]);
				  	rwcet->add_value_probCCDFhigh(probCCDFhigh[i]);
				  }

				 long double value = 1;
	  			 value = value / 1000;

		  		 for(int i=0;i<5;i++){
			  		pWCET03_15[i]= getpWCET(rank,probCCDF,rank_lenght, value);
			  		pWCET03_15low[i]= getpWCET(rank,probCCDFlow,rank_lenght, value);
			  		pWCET03_15high[i]= getpWCET(rank,probCCDFhigh,rank_lenght, value);
			  		value=value/1000;
				 	printf("%.20Lf %.20Lf %.20Lf\n" , pWCET03_15[i] , pWCET03_15low[i] , pWCET03_15high[i]);
				  }


			}
	
			free(probCCDFhigh);
			free(probCCDFlow);
			free(probCCDF);
			free(rank);
			if(nelems>0)free(excesses);
            free(limitbelow);
            free(limitabove);
            free(cv);
            free(trace_sortedCV);
            free(trace_sorted);
            free(ETtrace);
       
            rwcet->set_wcet_value(maxET);
            rwcet->set_pwcet_value(pWCET03_15high[4]); 
    	 	rwcet->set_mincv_used(mincv);
    	 	rwcet->set_error(no_error);
    	 	std::cout << nelems << std::endl;
            return rwcet; 
        }

        private : 
        T getMax( T * data , int size){
        	T max = -1 ;
        	if (size == 0) return -1; 
        	else if (size == 1) return data[0]; 
        	else{
        		max = data[0];
        		for(int i=0;i<size;i++){
        			if(data[i] > max) max = data[i];
        		}
        		return max ; 
        	} 
         return max ; 
        }

        static int cmpSort(const void *p, const void *q) {
		return (*( T *)p <= *( T *)q);
		}	

		void setMinValues( T * data , int setMinValues ){	 
			for(int i=0; i<setMinValues ; i++){
				data[i] = 0;
			}
		}	

		void setCV(int half_size, int minvalues , T * trace_sortedCV , T * cv){
 	
 			T * sub = (T * ) malloc( half_size * sizeof(T));
			for(int i = half_size ; i > minvalues -1 ; i--){
				memADD( sub , trace_sortedCV , -trace_sortedCV[i-1], i);
				cv[minvalues+half_size-i-1]=getStd(sub , i)/(getMean(sub, i ));
				memADD( sub , sub , trace_sortedCV[i-1], i);
			}
			free(sub);	
		}
		void memADD( T * dst ,  T * src , T value, int size){
			for(int i=0;i<size;i++){
			dst[i] = src[i] + value;
			}			
		}
		T getMean(T * vec , int n){
			T sum=0;
			for(int i=0;i<n;i++){
				sum+=vec[i];
			}
			return sum/n;
		}
		T getStd( T * vec , int n){

			T mean = getMean(vec,n);
			T tmp=0;
			T sum=0;
			for(int i=0;i<n;i++){
				tmp=vec[i] - mean;
				tmp= tmp * tmp;
				sum+=tmp;
				}
		sum = sqrt(sum / (n-1));
		return sum;
		}
		void setLimit( T * limitabove, T *  limitbelow , int size , int half_sizeCV)  {
		  	T tmp = 0;
			for(int i=0;i<size;i++){
				tmp = (1.96/sqrt(half_sizeCV-i));
				limitabove[i]=1 + tmp;
				limitbelow[i]=1 - tmp;
			}
		}
		int getStartPos(T * cv , T * limitabove, int half_size, int minvalues){
			int startpos=0;
			for (int i = 0; i< half_size- minvalues+1 ;i++){
				if(cv[i+minvalues-1] > limitabove[i]) startpos = i ;
			}
			return startpos;
		}
		T absD(T value){
			if(value < 0 ) return -value;
			return value;
		}
		void arange( T * vec , int n , T start ,  T end ,  T step ){

			 T tmp  = start ;
			 vec[0] = tmp ;
			 tmp += step ;
			for(int i = 1; i<n ; i++){
				vec[i] = tmp ; 
				tmp += step ;  
			}
		}	
		long double getExp(long double  value){
			long double exp;
			 if (std::is_same<T, float>::value) { exp = powf(M_E,value );}
			 else if (std::is_same<T, double>::value) { exp = pow(M_E,value );}
			 else if (std::is_same<T, long double>::value) { exp = powl(M_E,value );}
			return exp  ; 
		}
		void setExponProbabilyDensityFunction(T * dest , T * src , int size, T rate){
			for(int i=0;i<size;i++){
				dest[i] = rate * getExp(-rate * src[i]);
			}		
		}
		void setExponCumulativeDistributionFunction(T * dest , T * src , int size, T rate){
			setExponProbabilyDensityFunction(dest ,  src , size,  rate);
			T sum =0;
			for(int i=0;i< size;i++){
				sum+=dest[i];
			}
			T * tmp =(T * ) malloc(size * sizeof(T));
			memcpy(tmp, dest , size * sizeof(T));
			dest[0]=0;
			for(int i=1;i<size;i++){
				dest[i] = dest[i-1] + tmp[i-1]/sum;
			}
			free(tmp);
		}
		void setExponSurvivalFunction(T * dest , T * src , int size , T rate){
			setExponCumulativeDistributionFunction(dest, src , size , rate);
			for(int i=0;i<size;i++){
				T tmp = 1 - dest[i] ; 
				if( tmp <= 0) dest[i] = 0;
				else dest[i] = tmp;
			}
		}
		 T getpWCET( T * rank , T *  probCCD , int n ,  long double  limit ){
		 T pWCET=0;
			for(int i = 0 ; i < n && pWCET == 0; i++){
				if(probCCD[i] <= limit){
					pWCET = rank[i];
				} 
			}
			return pWCET ; 
		}

		void printVec(long double * data , int size){
			for(int i = 0 ; i < size ; i++){
				printf("[%i] %.20Lf\n" , i , data[i]);
			}
		}


    };



};    // libta,

#define type  long double
int main(int argc, char * argv[]) {

    using namespace libta;

    MyTimingAnalyzer<type> mta;

    std::shared_ptr<Request<type>> req = std::make_shared<Request<type>>();

   //FILL REQUEST VALUE
  char * end;
  std::string line;
  std::ifstream myfile (argv[1]);
  if (myfile.is_open()){
    while ( getline (myfile,line) ){
      //std::cout << line << '\n';
      char * ptr = &line[0];
      req->add_value(strtold(ptr, &end));
 	}
    myfile.close();
  }else{
  	std::cout << "Unable to open file"; 
  }

  std::shared_ptr<ResponsePWCET<type>> rwcet = std::dynamic_pointer_cast<ResponsePWCET<type>>(mta.perform_analysis(req));
/*
  std::cout << "WCET Value: " << rwcet->get_wcet_value() << std::endl;
  std::cout << "PWCET Value: " << rwcet->get_pwcet_value() << std::endl;
  std::cout << "Micv used: " << rwcet->get_mincv_used() << std::endl;
  std::cout << "Error code : " << rwcet->get_error()  <<std::endl;
*/



}

