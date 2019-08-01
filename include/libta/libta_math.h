#ifndef __LIBTA__MATH__HH__
#define __LIBTA__MATH__HH__

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
    T getUnbiasedMean(std::vector<T> v, size_t start, size_t size) {
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
    T getUnbiasedStdDeviation(std::vector<T> v, size_t start, size_t size) {
        assert(start+size <= v.size());
        assert(start+size-1 >= 0);
        T sumOfSquares=0;
        T sumOfValues=0;
        for(size_t i=start; i<start+size; i++) {
            const T delta=v[i] - v[start+size-1];
            sumOfValues+= delta;
            sumOfSquares+=(delta*delta);
        }
        return internalSqrt<T>( sumOfSquares/(size-1)-internalPow<T>(sumOfValues,2)/((size-1)*size) );
    }
    

    /**
    * @brief fill vec in starting from start and increasing of step
    *
    */
    template <typename T>
    void arange( std::vector<T> & vec, const T start, const T step ) {
        vec[0]=start;
        for(size_t i = 1; i<vec.size() ; i++) {
            vec[i] = vec[i-1]+step ;
        }
    }


    //Returns the sum of the generated values
    template <typename T>
    T setExponProbabilyDensityFunction(std::vector<T> & dest, const std::vector<T> & src,T rate) {
        T sum=0;
        for(size_t i=0; i<dest.size(); i++) {
            dest[i] = rate * internalPow<T>(M_E,-rate * src[i]) ;
            sum += dest[i];
        }
        return sum;
    }

    template <typename T>
    void setExponSurvivalFunction(std::vector<T> & dest, const std::vector<T> & src, T rate) {
        const T sum= setExponProbabilyDensityFunction(dest, src,  rate);
        const int size = dest.size();
        {   //Scope to limit tmp
            std::vector<T> tmp = dest;
            dest[0]=0;
            for(int i=1; i<size; i++) {
                dest[i] =  dest[i-1] + tmp[i-1]/sum;
                //We don't need dest[i-1] anymore. If dest[i-1] is less than 0, then set to zero
                dest[i-1] = 1.0 - dest[i-1];
                if(dest[i-1]< 0) {
                    dest[i-1]=0;
                }
            }
        }

        //Correct the value for the last element
        dest[size-1] = 1.0 - dest[size-1];
        if(dest[size-1]< 0) {
            dest[size-1]=0;
        }
    }


};

#endif
