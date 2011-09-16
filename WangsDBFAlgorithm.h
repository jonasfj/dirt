#ifndef WANGSDBFALGORITHM_H
#define WANGSDBFALGORITHM_H

#include "MatrixTask.h"

#include <vector>

/** Algorithm for computing the Demand-Bound-Function using Wangs algorithm */
class WangsDBFAlgorithm{
	/** Demand tuple for abstracting paths */
	struct DemandTuple{
		int wcet;								///< Accumulated worst case execution time
		int time;								///< Accumulated inter-release time + last deadline
		MatrixTask::JobId lastJob;				///< Last job in the path abstracted by this tuple
		bool dominates(const DemandTuple& tuple) const;
	};
	/** List of demand tuples */
	class DemandTupleList{
	public:
		typedef std::vector<DemandTuple>::iterator iterator;
		bool insert(const DemandTuple& tuple);
		iterator begin() { return tuples.begin(); }
		iterator end() { return tuples.end(); }
		bool pop(DemandTuple& tuple);
	private:
		std::vector<DemandTuple> tuples;		///< All generated tuples, including waiting list
	};
public:
	WangsDBFAlgorithm(const MatrixTask* task, int time);
private:
	void compute();
	int _time;									///< Length of time interval to compute DBF for
	std::vector<DemandTupleList> tupleLists;	///< DemandTupleLists for all generated tuples
	std::vector<DemandTupleList> waitingLists;	///< DemandTupleList for tuples waiting to be expanded
	const MatrixTask* _task;					///< MatrixTask being processed
	bool expandTuples(MatrixTask::JobId job);
};

#endif /* WANGSDBFALGORITHM_H */
