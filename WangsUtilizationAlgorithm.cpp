#include "WangsUtilizationAlgorithm.h"

#include "MatrixTask.h"

#include <assert.h>

/** Compute worst case execution time and accumulated inter-release time */
double WangsUtilizationAlgorithm::computeUtilization(const MatrixTask* task){
	assert(_task == NULL);
	assert(task != NULL);
	assert(places.size() == 0);

	// Create initial triple for each vertex
	for(size_t i = 0; i < task->jobs(); i++){
		UtilizationTuple ut;
		ut.wcet = 0;
		ut.time = 0;
		UtilizationTupleList ul;
		ul.insert(ut);
		places.push_back(ul);
	}
	assert(places.size() == task->jobs());

	for(size_t i = 0; i < task->jobs(); i++){
		for(MatrixTask::JobId j = 0; j < task->jobs(); j++)
			expandTriples(j);
	}

	double utilization = 0;
	for(MatrixTask::JobId j = 0; j < task->jobs(); j++){
		for(UtilizationTupleList::iterator it = places[j].begin(); it != places[j].end(); it++){
			assert(it->time != 0);
			if(it->start == j){
				if(it->wcet / it->time > utilization)
					utilization = it->wcet / it->time;
			}
		}
	}

	places.clear();
	_task = NULL;

	return utilization;
}

/** Expand triples for given place */
void WangsUtilizationAlgorithm::expandTriples(MatrixTask::JobId place){
	assert(_task != NULL);
	UtilizationTupleList::iterator it = places[place].begin();
	while(it != places[place].end()){
		// Skip if we've already be around...
		if(it->start == place && it->wcet != 0){
			it++;
			continue;
		}
		// Foreach outgoing edge, insert new tuple
		for(MatrixTask::JobId j = 0; j < _task->jobs(); j++){
			if(_task->edge(place, j)){
				UtilizationTuple ut;
				ut.wcet = it->wcet + _task->wcet(place);
				ut.time = it->time + _task->mtime(place, j);
				places[j].insert(ut);
			}
		}
		it = places[place].erase(it);
	}
}

/** Insert tuple, if it cannot be eliminated by optimizations */
void WangsUtilizationAlgorithm::UtilizationTupleList::insert(const UtilizationTuple& tuple){
	//It is possible to do this more efficiently, by having a list of tuples depending on where they started
	//and then index these list by time.
	bool dominated = false;
	UtilizationTupleList::iterator it = begin();
	while(it != end()){
		if(it->start == tuple.start){
			//Check if tuple dominates
			if(it->wcet < tuple.wcet && it->time >= tuple.time){
				it = erase(it);
				continue;
			}
			//Check if tuple is dominated
			dominated |= (it->wcet >= tuple.wcet && it->time <= tuple.time);
		}
		it++;
	}
	if(!dominated)
		tuples.push_back(tuple);
}

