#ifndef DISCRETEDBF_H
#define DISCRETEDBF_H

#include "DemandTuple.h"
#include "AbstractDBF.h"

#include <vector>

namespace DRT{
namespace Verification{

/** Function for demand bound, basically a ordered list of DemandTuples */
class DiscreteDBF : public AbstractDBF{
	typedef std::vector<DemandTuple> TupleList;
	typedef TupleList::iterator TupleIterator;
	typedef TupleList::const_iterator ConstTupleIterator;
	/** StepIterator for stepping through discrete demand bound function */
	class DiscreteStepIterator : public StepIterator{
	public:
		DiscreteStepIterator(const TupleList& tuples);
		bool next();
	private:
		ConstTupleIterator _next,		///< Next tuple to return
						   _end;		///< End of tuple list
	};
public:
	/** Create empty demand bound function */
	DiscreteDBF(){}
	/** Copy-construct demand bound funciton */
	DiscreteDBF(const DiscreteDBF& dbf)
		: _tuples(dbf._tuples) {}
	explicit DiscreteDBF(const AbstractDBF& dbf);

	bool insert(const DemandTuple& tuple);
	int operator()(int time) const;
	StepIterator* steps() const;
	DemandTuple pop();
	static DiscreteDBF parallelMerge(const DiscreteDBF& dbf1,
									  const DiscreteDBF& dbf2);
	static DiscreteDBF sequentialMerge(const DiscreteDBF& dbf1,
										const DiscreteDBF& dbf2);
	/** Delete all tuples in this function */
	void clear() { _tuples.clear(); }
	
	/** True, if this function is empty */
	bool empty() const { return _tuples.empty(); }
private:
	TupleIterator search(int time);
	TupleList _tuples;					///< Tuples in this function
};


} /* Verification */
} /* DRT */

#endif /* DISCRETEDBF_H */
