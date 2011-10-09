#include "CompositeDBF.h"

#include <assert.h>

namespace DRT{
namespace Verification{

/** Add demand bound function to composite DBF */
void CompositeDBF::addDBF(const AbstractDBF* dbf){
	assert(dbf);
	dbfs.push_back(dbf);
}

/** Construct composite step iterator
 * Insert a StepIterator into steppers for each DBF, and advance them 1
 */
CompositeDBF::CompositeStepIterator::CompositeStepIterator(const CompositeDBF::DBFList& dbfs){
	for(size_t i = 0; i < dbfs.size(); i++){
		steppers.push_back(dbfs[i]->steps());
		assert(steppers[i]->time() == 0 && steppers[i]->wcet() == 0);
		steppers[i]->next();
	}
}

/** Release resources held by composite step iterator */
CompositeDBF::CompositeStepIterator::~CompositeStepIterator(){
	for(size_t i = 0; i < steppers.size(); i++)
		delete steppers[i];
	steppers.clear();
}

/** Get demand bound within time for parallel execution of DBFs */
int ParallelDBF::operator()(int time) const{
	int sum = 0;
	for(ConstDBFIterator it = dbfs.begin(); it != dbfs.end(); it++){
		sum += (**it)(time);
	}
	return sum;
}

/** Create step iterator for this demand bound function */
AbstractDBF::StepIterator* ParallelDBF::steps() const{
	return new ParallelStepIterator(dbfs);
}

/** Create parallel step iterator
 * @remarks: Step iterators must be at time 0
 */
ParallelDBF::ParallelStepIterator::ParallelStepIterator(const DBFList& dbfs)
 : CompositeStepIterator(dbfs){
	for(size_t i = 0; i < dbfs.size(); i++)
		prevWcet.push_back(0);
}

/** Advance parallel step iterator to next step */
bool ParallelDBF::ParallelStepIterator::next(){
	int t = INT_MAX;
	// Select time for next step
	for(size_t i = 0; i < steppers.size(); i++){
		if(steppers[i]->time() <= t)
			t = steppers[i]->time();
	}
	// Set selected time
	setTime(t);
	if(t == INT_MAX)
		return false;

	// Add steps with given time, and move to next step
	for(size_t i = 0; i < steppers.size(); i++){
		assert(steppers[i]->time() >= t);
		if(steppers[i]->time() == t){
			assert(steppers[i]->wcet() > prevWcet[i]);
			setWcet(wcet() + steppers[i]->wcet() - prevWcet[i]);
			prevWcet[i] = steppers[i]->wcet();
			steppers[i]->next();
		}
	}
	// Return true, if we're not at end
	return t != INT_MAX;
}

/** Get demand bound within time for sequential execution of DBFs */
int SequentialDBF::operator()(int time) const{
	int max = 0;
	for(ConstDBFIterator it = dbfs.begin(); it != dbfs.end(); it++){
		int d = (**it)(time);
		if(max < d) max = d;
	}
	return max;
}

/** Create step iterator for this demand bound function */
AbstractDBF::StepIterator* SequentialDBF::steps() const{
	return new SequentialStepIterator(dbfs);
}

/** Advance sequantial step iterator to next step */
bool SequentialDBF::SequentialStepIterator::next(){
	int new_wcet = 0;
	do{
		int t = INT_MAX;
		// Select next time instance
		for(size_t i = 0; i < steppers.size(); i++){
			if(steppers[i]->time() <= t)
				t = steppers[i]->time();
		}
		// Set selected time
		setTime(t);
		if(time() == INT_MAX)
			return false;

		for(size_t i = 0; i < steppers.size(); i++){
			assert(steppers[i]->time() >= t);
			if(steppers[i]->time() == t){
				if(new_wcet < steppers[i]->wcet())
					new_wcet = steppers[i]->wcet();
				steppers[i]->next();
			}
		}
	}while(wcet() >= new_wcet);
	setWcet(new_wcet);
	assert(new_wcet > 0);
	return true;
}


} /* Verification */
} /* DRT */
