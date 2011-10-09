#ifndef DEMANDTUPLE_H
#define DEMANDTUPLE_H

#include <assert.h>

namespace DRT{
namespace Verification{

/** Tuple for demand with time bound */
class DemandTuple{
public:
	/** Create new invalid DemandTuple */
	DemandTuple(int wcet = -1, int time = -1){
		_wcet = wcet;
		_time = time;
	}
	
	/** Copy-construct a DemandTuple */
	DemandTuple(const DemandTuple& tuple){
		_wcet = tuple.wcet();
		_time = tuple.time();
	}
	
	/** get wcet for the bound */
	int wcet() const { return _wcet; }
	
	/** get time bound for demand */
	int time() const { return _time; }
	
	/** True, if this tuple is valid */
	bool valid() const { return 0 <= wcet() && wcet() <= time();}
	
	/** Set demand for the bound */
	void setWcet(int wcet) { _wcet = wcet; }
	
	/** Set time bound for the demand */
	void setTime(int time) { _time = time; }
	
	/** Set demand tuple invalid */
	void setInvalid() { _wcet = _time = -1; }
	
	/** True, if this tuple dominates the given tuple
	 * @remarks: If two tuples are equal, they will dominate each other.
	 */
	bool dominates(const DemandTuple& tuple) const {
		return wcet() >= tuple.wcet() && time() <= tuple.time();
	}
	
	/** True, if this tuple is equal to another tuple */
	bool operator==(const DemandTuple& tuple) const {
		return wcet() == tuple.wcet() && time() == tuple.time();
	}
	
	/** Utilization, given demand over time */
	double utilization() const { return wcet() / (double)time(); }
private:
	int  _wcet,						///< Execution demand with given time bound
		 _time;						///< Time bound for the given execution demand
};

} /* Verification */
} /* DRT */

#endif /* DEMANDTUPLE_H */
