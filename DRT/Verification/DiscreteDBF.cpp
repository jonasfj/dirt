#include "DiscreteDBF.h"

#include <assert.h>

namespace DRT{
namespace Verification{

/** Create DiscreteDBF from AbstractDBF using stepper */
DiscreteDBF::DiscreteDBF(const AbstractDBF& dbf){
	AbstractDBF::StepIterator* stepper = dbf.steps();
	while(stepper->next())
		_tuples.push_back(DemandTuple(stepper->wcet(), stepper->time()));
	delete stepper;
}

/** Insert tuple into function, return true if tuple was dominated by this function */
bool DiscreteDBF::insert(const DemandTuple& tuple){
	assert(tuple.valid()); // We shouldn't be inserting invalid tuples

	// Find place to insert
	TupleIterator it = search(tuple.time());

	// Check if tuple is dominated, Note: it->time >= tuple.time
	if(it != _tuples.begin() && (it-1)->dominates(tuple))
		return false;
	// Check just in case it->time == tuple.time
	if(it != _tuples.end() && it->dominates(tuple))
		return false;

	// Remove everything that we dominate, Note: it.time >= tuple.time
	while(it != _tuples.end() && tuple.dominates(*it))
		it = _tuples.erase(it);	//TODO: If we erase we should use overwrite instead of erase and insert

	// Insert tuple, before it, Note: it.time >= tuple.time
	_tuples.insert(it, tuple);

	return true;
}

/** Get demand bound of this function for given time */
int DiscreteDBF::operator() (int time) const{
	TupleIterator it = const_cast<DiscreteDBF*>(this)->search(time);
	if(it != _tuples.begin())
		return (it-1)->wcet();
	return 0;
}

/** Pop some DemandTuple from this function */
bool DiscreteDBF::pop(DemandTuple& tuple){
	if(empty()) return false;
	tuple = _tuples.back();
	_tuples.pop_back();
	return true;
}

/** Get TupleIterator pointing to first element with time >= the given time */
DiscreteDBF::TupleIterator DiscreteDBF::search(int time) {
	//TODO Test binary search, and make sure it's not off by one
	TupleIterator start = _tuples.begin(),
				  end	= _tuples.end(),
				  mid	= _tuples.begin();
	while(start < end){
		mid = start + (std::distance(start, end) / 2);	//Note: Integer division will always floor
		if(mid->time() > time)
			end = mid-1;
		else if(mid->time() < time)
			start = mid+1;
		else
			return mid;
	}
	assert(start == end);
	// Check if start->time() < time, if so add 1
	if(start != _tuples.end() && start->time() < time)
		return start + 1;
	return start;
/*
	//Alternative stupid implementation
	for(TupleIterator it = _tuples.begin(); it != _tuples.end(); it++){
		if(it->time() >= time)
			return it;
	}
	return _tuples.end();
*/
}


/** Get StepIterator for this DiscereteDBF */
AbstractDBF::StepIterator* DiscreteDBF::steps() const{
	return new DiscreteStepIterator(_tuples);
}

/** Create DiscreteStepIterator */
DiscreteDBF::DiscreteStepIterator::DiscreteStepIterator(const TupleList& tuples){
	_next = tuples.begin();
	_end = tuples.end();
}

/** Advance step iterator by one (tuple) */
bool DiscreteDBF::DiscreteStepIterator::next(){
	if(_next == _end){
		setTime(INT_MAX);
		return false;
	}
	setWcet(_next->wcet());
	setTime(_next->time());
	_next++;
	return true;
}

/** Merge two DiscreteDBFs as if there we executed in parallel
 * @remarks This can also be achived on-the-fly using ParallelDBF
 */
DiscreteDBF DiscreteDBF::parallelMerge(const DiscreteDBF& dbf1,
									   const DiscreteDBF& dbf2){
	DiscreteDBF retval;
	const TupleList& t1 = dbf1._tuples,
		  			 t2 = dbf2._tuples;
	TupleList& tr = retval._tuples;
	ConstTupleIterator i1 = t1.begin(),
					   i2 = t2.begin();

	int max1 = 0,
		max2 = 0;

	// While we have stuff to merge from both t1 and t2
	while(i1 != t1.end() && i2 != t2.end()){
		int time = -1;
		if(i1->time() <= i2->time()){
			max1 = i1->wcet();
			time = i1->time();
			i1++;
		}
		if(i1->time() >= i2->time()){
			max2 = i2->wcet();
			time = i2->time();
			i2++;
		}
		assert(time != -1);
		tr.push_back(DemandTuple(max1 + max2, time));
	}

	// Now one of them might contains something...
	while(i1 != t1.end()){
		tr.push_back(DemandTuple(i1->wcet() + max2, i1->time()));
		i1++;
	}
	while(i2 != t2.end()){
		tr.push_back(DemandTuple(max1 + i2->wcet(), i2->time()));
		i2++;
	}

	return retval;
}

/** Merge two DiscreteDBFs as if there we executed sequentially
 * @remarks This can also be achived on-the-fly using SequentialDBF
 */
DiscreteDBF DiscreteDBF::sequentialMerge(const DiscreteDBF& dbf1,
										 const DiscreteDBF& dbf2){
	DiscreteDBF retval;
	const TupleList& t1 = dbf1._tuples,
		  			 t2 = dbf2._tuples;
	TupleList& tr = retval._tuples;
	ConstTupleIterator i1 = t1.begin(),
					   i2 = t2.begin();

	int max = 0;

	// While we have stuff to merge from both t1 and t2
	while(i1 != t1.end() && i2 != t2.end()){
		int time = -1;
		if(i1->time() <= i2->time()){
			if(max < i1->wcet()){
				max = i1->wcet();
				time = i1->time();
				assert(time != -1);
			}
			i1++;
		}else if(i1->time() >= i2->time()){
			if(max < i2->wcet()){
				max = i2->wcet();
				time = i2->time();
				assert(time != -1);
			}
			i2++;
		}
		if(time != -1)
			tr.push_back(DemandTuple(max, time));
	}

	// Now one of them might contains something...
	while(i1 != t1.end()){
		if(max < i1->wcet()){
			max = i1->wcet();
			tr.push_back(DemandTuple(max, i1->time()));
		}
		i1++;
	}
	while(i2 != t2.end()){
		if(max < i2->wcet()){
			max = i2->wcet();
			tr.push_back(DemandTuple(max, i2->time()));
		}
		i2++;
	}

	return retval;
}



} /* Verification */
} /* DRT */
