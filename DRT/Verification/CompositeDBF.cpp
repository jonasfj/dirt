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
		prevWcet[i] = 0;
}

/** Advance parallel step iterator to next step */
bool ParallelDBF::ParallelStepIterator::next(){
	int s = -1;
	// Select stepper with lowest time
	for(size_t i = 0; i < steppers.size(); i++){
		if(steppers[i]->end()) continue;
		if(s == -1 || steppers[i]->time() < steppers[s]->time())
			s = i;
	}
	if(s == -1){
		setTime(INT_MAX);
		return false;
	}
	assert(steppers[s]->wcet() > prevWcet[s]);
	setWcet(wcet() + steppers[s]->wcet() - prevWcet[s]);
	prevWcet[s] = steppers[s]->wcet();
	setTime(steppers[s]->time());
	steppers[s]->next();
	return true;
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
	int new_time = 0;
	do{
		int s = -1;
		// Select stepper with lowest time
		for(size_t i = 0; i < steppers.size(); i++){
			if(steppers[i]->end()) continue;
			if(s == -1 || steppers[i]->time() < steppers[s]->time())
				s = i;
		}
		if(s == -1){
			setTime(INT_MAX);
			return false;
		}
		new_wcet = steppers[s]->wcet();
		new_time = steppers[s]->time();
		steppers[s]++;
	}while(wcet() >= new_wcet);
	setWcet(new_wcet);
	setTime(new_time);
	assert(new_time > 0 && new_wcet > 0);
	return true;
}


} /* Verification */
} /* DRT */
