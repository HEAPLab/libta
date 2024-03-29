#ifndef LIBTA_MATH_H_
#define LIBTA_MATH_H_

#include <cmath>

namespace libta {

    /**
    * @brief wrap pow to enforce type casting
    *
    */
    template <typename T>
    inline T internalPow(T base, T exp) {
        return std::is_same<T, long double>::value ? powl(base,exp) : pow(base, exp);
    }

    /**
    * @brief wrap sqrt to enforce type casting
    *
    */
    template <typename T>
    inline T internalSqrt(T v) {
        return std::is_same<T, long double>::value ? sqrtl(v) : sqrt(v);
    }

    /**
    * @brief Return the mean of the vector obtained by subtracting v.last to all the elements of v
    *
    */
    template <typename T>
    T getUnbiasedMean(const std::vector<T> &v, size_t start, size_t size) {
        assert(start+size <= v.size());
        assert(start+size-1 >= 0);
        T result=0;
        for(size_t i=start; i<start+size; i++) {
            result+=(v[i]-v[start+size-1]);
        }
        return result/size;
    }

    /**
    * @brief Return the standard devtiation of the vector obtained by subtracting v.last to all the elements of v
    *
    */
    template <typename T>
    T getUnbiasedStdDeviation(const std::vector<T> &v, size_t start, size_t size) {
        assert(start+size <= v.size());
        assert(start+size-1 >= 0);
        T sumOfSquares=0;
        T sumOfValues=0;
        for(size_t i=start; i<start+size; i++) {
            const T delta=v[i] - v[start+size-1];
            sumOfValues+= delta;
            //sumOfSquares+=(delta*delta);
        }
        T mean=sumOfValues/size;
        
        for(size_t i=start; i<start+size; i++) {
            const T delta=v[i] - v[start+size-1];
            sumOfSquares+= internalPow<T>(delta-mean,2);
        }
        //return internalSqrt<T>( sumOfSquares/(size-1)-internalPow<T>(sumOfValues,2)/((size-1)*size) );
        return internalSqrt<T>( sumOfSquares/(size-1));
    
    }
    

    /**
    * @brief fill vec in starting from start and increasing of step
    *
    */
    template <typename T>
    void arange( std::vector<T> & vec, const T start, const T step ) {
        assert(vec.size() !=0);
        assert(vec.size() > 2);

        vec[0]=start;
        for(size_t i = 1; i<vec.size() ; i++) {
            vec[i] = vec[i-1]+step ;
        }
    }


     /**
     * @brief The method transform the vector src values to Exponential Probability Density Function and
     *        it saves to 'dest vector'. For the calculation it uses 'rate' value.
     *        The function return sum value, which is equal to the sum of all values of dest vector.
     */
    template <typename T>
    T setExponProbabilyDensityFunction(std::vector<T> & dest, const std::vector<T> & src,T rate) {
        assert(src.size() != 0);
        assert(dest.size() != 0);

        T sum=0;
        for(size_t i=0; i<dest.size(); i++) {
            dest[i] = rate * internalPow<T>(M_E,-rate * src[i]) ;
            sum += dest[i];
        }
        return sum;
    }

     /**
     * @brief The method transform the vector src values to Exponential Survival Function and it
     *        saves to 'dest vector'. For the calculation it uses 'rate' value.      
     *        
     */
    template <typename T>
    void setExponSurvivalFunction(std::vector<T> & dest, const std::vector<T> & src, T rate) {
        assert(src.size() != 0);
        assert(dest.size() != 0);

        const T sum= setExponProbabilyDensityFunction(dest, src,  rate);
        const int size = dest.size();
        {   //Scope to limit tmp
            std::vector<T> tmp = dest ; 
            dest[0] = 0;
            for(int i=1;i<size;i++){
                 dest[i] = dest[i-1] + tmp[i-1]/sum;
            }
    
            for(int i=0; i<size;i++){
                T tmpvalue = 1 - dest[i];
                if(tmpvalue <= 0 ) dest[i] = 0;
                else dest[i] = tmpvalue; 
            }
      }

    }
};

#endif
