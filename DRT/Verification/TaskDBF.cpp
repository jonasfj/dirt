#include "TaskDBF.h"

#include <math.h>

using namespace DRT::Matrix;

namespace DRT{
namespace Verification{

TaskDBF::TaskDBF(const Matrix::MatrixTask* task, int time)
 : task(*task) {
	 compute(time);
}

TaskDBF::~TaskDBF(){
	// Delete waitinglists, note tupleLists are owned by underlying SequentialDBF
	for(size_t i = 0; i < waitingLists.size(); i++)
		delete waitingLists[i];
	waitingLists.clear();
}

void TaskDBF::compute(int time){
	// Create initial demand tuples for each job
	for(MatrixTask::JobId j = 0; j < task.jobs(); j++){
		DemandTuple dt(task.wcet(j), task.deadline(j));
		DiscreteDBF waiting, tuples;
		waiting.insert(dt);
		tuples.insert(dt);
		// Insert them in the tuple and waiting list lists
		tupleLists.push_back(new DiscreteDBF(tuples));
		waitingLists.push_back(new DiscreteDBF(waiting));
		// Add tuple lists to underlying SequentialDBF
		SequentialDBF::addDBF(tupleLists[j]);
	}
	assert(tupleLists.size() == task.jobs());
	assert(waitingLists.size() == task.jobs());

	// This will at most take t steps, but we look out for when nothing new is generated
	for(int t = 0; t < time; t++){
		bool created_new = false;
		for(MatrixTask::JobId j = 0; j < task.jobs(); j++)
			created_new |= expandTuples(j, time);
		// When nothing new is generated, there's no recent to iterate any further
		if(!created_new) break;
	}
	//Note:	Underlying SequentialDBF should take care of presenting all the DiscreteDBFs in
	//		tupleLists as one DBF, sweet!
	//		So, nothing more to do here... Of course discretizing this TaskDBF now would save
	//		space, this discretization could also be done on-the-fly, but it's probably more
	//		interesting to be able to recompute the DBF, if say we add new tuples...
}

/** Expands waiting tuples for a given job, returns true if anything new was generated */
bool TaskDBF::expandTuples(Matrix::MatrixTask::JobId job, int time){
	bool created_new = false;
	const int job_deadline = task.deadline(job);
	DemandTuple tuple;
	while((tuple = waitingLists[job]->pop()).valid()){
		for(MatrixTask::JobId j = 0; j < task.jobs(); j++){
			int mtime = task.mtime(job, j);
			if(mtime != INT_MAX){
				int nwcet = tuple.wcet() + task.wcet(j),
					ntime = tuple.time() + mtime - job_deadline + task.deadline(j);
				if(ntime > time)
					continue;
				DemandTuple n(nwcet, ntime);
				if(tupleLists[j]->insert(n))
					created_new |= waitingLists[j]->insert(n);
			}
		}
	}
	return created_new;
}

/** Create DemandBoundFunction for DRT tasks */
AbstractDBF* TaskDBF::DRTDemandBoundFunction(const std::vector<Matrix::MatrixTask*>& tasks, int time){
	typedef std::vector<Matrix::MatrixTask*>::const_iterator ConstTaskIter;
	ParallelDBF* dbf = new ParallelDBF();
	for(ConstTaskIter it = tasks.begin(); it != tasks.end(); it++)
		dbf->addDBF(new TaskDBF(*it, time));
	return dbf;
}

/** Find upper bound on demand bound function for tasks system given utilization */
int TaskDBF::DBFBound(const std::vector<Matrix::MatrixTask*>& tasks, double utilization){
	typedef std::vector<Matrix::MatrixTask*>::const_iterator ConstTaskIter;
	int wcet = 0;		//wcet for all jobs in all tasks
	for(ConstTaskIter it = tasks.begin(); it != tasks.end(); it++){
		for(size_t i = 0; i < (*it)->jobs(); i++)
			wcet += (*it)->wcet(i);
	}
	return ceil(wcet / (1 - utilization));
}

} /* Verification */
} /* DRT */
