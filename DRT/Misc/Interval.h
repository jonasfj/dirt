#ifndef INTERVAL_H
#define INTERVAL_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

namespace DRT{
namespace Misc{

/** Interval of type T */
template<typename T = int>
class Interval{
	T _min,			///< Minimum value of interval
	  _max;			///< Maximum value of interval
public:
	Interval(T min = 1, T max = 0){
		_min = min;
		_max = max;
	}
	Interval(const Interval<T>& interval){
		_min = interval.min();
		_max = interval.max();
	}
	/** Get min value from the interval */
	T min() const{ return _min; }
	/** Get max value from the interval */
	T max() const{ return _max; }
	/** Get a random number from the interval
	 * Assumes seed() have been called!
	 */
	T random() const{
		return min() + rand() % (max() - min() + 1 > 0 ? max() - min() + 1 : 1);
	}
	/** Parse string to interval, integers assumed */
	static Interval<T> parse(const char* string){
		int m, M;
		if(sscanf(string, "[%d;%d]", &m, &M) != 2){
			if(sscanf(string, "%d;%d", &m, &M) != 2){
				if(sscanf(string, "[%d,%d]", &m, &M) != 2){
					if(sscanf(string, "%d,%d", &m, &M) != 2){
						if(sscanf(string, "%d", &m) != 1){
							// Return invalid interval
							return Interval<T>();
						}else
							M = m;
					}
				}
			}		
		}
		return Interval<T>(m, M);
	}
	/** Seed the PRNG, given 0 to choose semi random seed */
	static void seed(unsigned int seed = 0){
		if(seed == 0)
			seed = time(NULL);
		srand(seed);
	}
	/** True, if interval is valid
	 * @remarks Interval is valid if min is larger than max
	 */
	bool valid() const { return min() <= max(); }
	/** True, if interval is empty */
	bool empty() const { return min() == max(); }
};

} /* Misc */
} /* DRT */

#endif /* INTERVAL_H */
