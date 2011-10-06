#include "WangsDBFAlgorithm.h"

#include <assert.h>

using namespace DRT::Matrix;

namespace DRT{
namespace Verification{

WangsDBFAlgorithm::WangsDBFAlgorithm(const MatrixTask* task, int time){
	_time = time;
	_task = task;
}

void WangsDBFAlgorithm::compute(){
	// Create initial demand tuples for each job
	for(MatrixTask::JobId j = 0; j < _task->jobs(); j++){
		DemandTuple dt;
		dt.wcet = _task->wcet(j);
		dt.time = _task->deadline(j);
		dt.lastJob = j;
		// Create tuple list and waiting list for each job
		DemandTupleList tl, wl;
		tl.insert(dt);
		wl.insert(dt);
		// Insert them in the tuple and waiting list lists
		tupleLists.push_back(tl);
		waitingLists.push_back(wl);
	}
	assert(tupleLists.size() == _task->jobs());
	assert(waitingLists.size() == _task->jobs());

	// This will at most take t steps, but we look out for when nothing new is generated
	for(int t = 0; t < _time; t++){
		bool created_new = false;
		for(MatrixTask::JobId j = 0; j < _task->jobs(); j++)
			created_new |= expandTuples(j);
		// When nothing new is generated, there's no recent to iterate any further
		if(!created_new) break;
	}

	//TODO Do something with all these demand tuples. Maybe put them all into one big list,
	// keeping only the biggest ones for each deadline
	// This could perhaps be done on-the-fly, to save memory, I don't know, it's basically
	// the dominates() method without the lastJob == tuple.lastJob condition.
}

/** Expands waiting tuples for a given job, returns true if anything new was generated */
bool WangsDBFAlgorithm::expandTuples(MatrixTask::JobId job){
	bool created_new = false;
	const int job_deadline = _task->deadline(job);
	DemandTuple dt;
	while(waitingLists[job].pop(dt)){
		for(MatrixTask::JobId j = 0; j < _task->jobs(); j++){
			int mtime = _task->mtime(job, j);
			if(mtime != INT_MAX){
				DemandTuple n;
				n.wcet = dt.wcet + _task->wcet(j);
				n.time = dt.time + mtime - job_deadline + _task->deadline(j);
				n.lastJob = j;
				if(tupleLists[j].insert(n))
					created_new |= waitingLists[j].insert(n);
			}
		}
	}
	return created_new;
}

/** True, if this tuple dominates tuple */
bool WangsDBFAlgorithm::DemandTuple::dominates(const WangsDBFAlgorithm::DemandTuple& tuple) const {
	return lastJob == tuple.lastJob && time <= tuple.time && wcet >= tuple.time;
}

/** Insert tuple in list
 * @remarks This method will insert a tuple in the list if it's not dominated by another tuple in the list
 * 			and it will remove any tuples in the list which are dominated by this tuple.
 */
bool WangsDBFAlgorithm::DemandTupleList::insert(const WangsDBFAlgorithm::DemandTuple& tuple){
	// Simpe binary search, that leaves mid pointing to where tuple should be inserted,
	// if the item before mid doesn't dominate tuple.
	iterator start	= tuples.begin(),
			 end	= tuples.end(),
			 mid	= tuples.begin();
	while(start < end){
		mid = start + (std::distance(start, end) / 2);	//Note: Integer division will always floor
		if(mid->time == tuple.time){
			// Handle special case, if tuple dominates mid default code flow will handle it
			if(mid->dominates(tuple))
				return false;
			break;
		}else if(mid->time > tuple.time)
			end = mid;
		else
			start = mid + 1;
	}

	// Check if a tuple with lower deadline dominates this tuple
	if(mid != tuples.begin()){
		iterator prev = mid - 1;
		if(prev->dominates(tuple))
			return false;			//If tuple is dominated, return false, don't insert it
	}

	// Check if a tuple with higher deadline is dominated by this tuple
	while(mid != tuples.end() && tuple.dominates(*mid))
		mid = tuples.erase(mid);

	// Insert tuple at mid
	tuples.insert(mid, tuple);

	// Return true as tuple was inserted
	return true;
}

/** Pop most recent entry from waiting list, returns true if successful */
bool WangsDBFAlgorithm::DemandTupleList::pop(WangsDBFAlgorithm::DemandTuple& tuple){
	if(tuples.empty())
		return false;
	tuple = tuples.back();
	tuples.pop_back();
	return true;
}

} /* Verification */
} /* DRT */
