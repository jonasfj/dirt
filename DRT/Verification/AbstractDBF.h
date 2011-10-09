#ifndef ABSTRACTDBF_H
#define ABSTRACTDBF_H

#include <limits.h>

namespace DRT{
namespace Verification{


/** Abstract interface for demand bound function */
class AbstractDBF{
public:
	/** Iterator for stepping through a demand bound function
	 * Should typically be subclassed along with each subclass of AbstractDBF
	 */
	class StepIterator{
	public:
		StepIterator(){
			_wcet = 0;
			_time = 0;
		}
		/** Virtual destructor for subclasses */
		virtual ~StepIterator() {}
		/** Worst case execution step of current step */
		int wcet() const { return _wcet; }
		/** Time bound of current step, INT_MAX if at end */
		int time() const { return _time; }
		/** True, if at end of DBF */
		bool end() const { return time() == INT_MAX; };
		/** Advance to next step, true if not at end */
		virtual bool next() = 0;
	protected:
		/** Set worst case execution time */
		void setWcet(int wcet) { _wcet = wcet; }
		/** Set time  */
		void setTime(int time) { _time = time; }
	private:
		int _wcet,			///< Worst case execution time of current step
			_time;			///< Time bound of current step
	};
	/** Virtual destructor for subclasses */
	virtual ~AbstractDBF(){}
	/** Demand given time bound */
	virtual int operator()(int time) const = 0;
	/** Get step iterator for stepping through this function
	 * @remarks StepIterator should be deleted after use.
	 */
	virtual StepIterator* steps() const = 0;
};


} /* Verification */
} /* DRT */

#endif /* ABSTRACTDBF_H */
