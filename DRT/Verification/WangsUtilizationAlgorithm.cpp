#include "WangsUtilizationAlgorithm.h"

#include "../Matrix/MatrixTask.h"

#include <assert.h>
#include <stdio.h>

using namespace DRT::Matrix;

namespace DRT{
namespace Verification{

/** Compute utilization of a task set */
double WangsUtilizationAlgorithm::computeUtilization(const std::vector<MatrixTask*>& tasks){
	double U = 0;
	for(std::vector<MatrixTask*>::const_iterator it = tasks.begin(); it != tasks.end(); it++)
		U += WangsUtilizationAlgorithm().computeUtilization(*it);
	return U;
}

/** Compute worst case execution time and accumulated inter-release time */
double WangsUtilizationAlgorithm::computeUtilization(const MatrixTask* task){
	assert(_task == NULL);
	assert(task != NULL);
	assert(waitingLists.size() == 0);
	assert(tupleLists.size() == 0);

	_task = task;
	bestUtilization = 0;

	// Create initial triple for each vertex
	for(size_t i = 0; i < task->jobs(); i++){
		UtilizationTuple ut;
		ut.wcet = 0;
		ut.time = 0;
		ut.start = i;
		UtilizationTupleList ul;
		ul.insert(ut);
		waitingLists.push_back(ul);
		tupleLists.push_back(ul);
		// Handle self-loops here
		if(task->edge(i, i)){
			if(bestUtilization < task->wcet(i) / (double)task->mtime(i, i))
				bestUtilization = task->wcet(i) / (double)task->mtime(i, i);
		}
	}
	assert(waitingLists.size() == task->jobs());
	assert(tupleLists.size() == task->jobs());

	for(size_t i = 0; i < task->jobs(); i++){
		bool genNew = false;
		for(MatrixTask::JobId j = 0; j < task->jobs(); j++)
			genNew |= expandTriples(j);
		if(!genNew) break;
	}

	double u = bestUtilization;
	bestUtilization = 0;
	waitingLists.clear();
	tupleLists.clear();
	_task = NULL;

	return u;
}

/** Expand triples for given place */
bool WangsUtilizationAlgorithm::expandTriples(MatrixTask::JobId job){
	assert(_task != NULL);
	bool created_new = false;
	UtilizationTuple ut;
	while(waitingLists[job].pop(ut)){
		for(MatrixTask::JobId j = 0; j < _task->jobs(); j++){
			int delay = _task->mtime(job, j);
			if(delay != INT_MAX && j != job){
				UtilizationTuple n;
				n.wcet = ut.wcet + _task->wcet(j);
				n.time = ut.time + delay;
				n.start = ut.start;
				// Don't insert loops
				if(n.start == j){
					if(bestUtilization < n.wcet / (double)n.time)
						bestUtilization = n.wcet / (double)n.time;
				}else if(tupleLists[j].insert(n))
					created_new |= waitingLists[j].insert(n);
			}
		}
	}

	return created_new;
}

/** Insert tuple in list
 * @remarks This method will insert a tuple in the list if it's not dominated by another tuple in the list
 * 			and it will remove any tuples in the list which are dominated by this tuple.
 */
bool WangsUtilizationAlgorithm::UtilizationTupleList::insert(const UtilizationTuple& tuple){
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
bool WangsUtilizationAlgorithm::UtilizationTupleList::pop(UtilizationTuple& tuple){
	if(tuples.empty())
		return false;
	tuple = tuples.back();
	tuples.pop_back();
	return true;
}

/** True, if this tuple dominates tuple */
bool WangsUtilizationAlgorithm::UtilizationTuple::dominates(const UtilizationTuple& tuple) const {
	return time <= tuple.time && wcet >= tuple.wcet;
}

} /* Verification */
} /* DRT */
