#ifndef WANGSUTILIZATIONALGORITHM_H
#define WANGSUTILIZATIONALGORITHM_H

#include "../Matrix/MatrixTask.h"

#include <vector>

namespace DRT{
namespace Verification{

/** An implementation Wangs utilization algorithm as presented in "The Digraph Real-Time Task Model" 2011 */
class WangsUtilizationAlgorithm{
	/** Utilization tuple for abstracting paths
	 * @remarks JobId of the vertex it have travelled to is held implicitly, by the container for these tuples
	 */
	struct UtilizationTuple{
		int wcet;								///< Accumulated worst case execution time
		int time;								///< Accumulated inter-release time
		Matrix::MatrixTask::JobId start;		///< Vertex from which the path abstracted by this tuple started
		bool dominates(const UtilizationTuple& tuple) const;
	};
	/** List of utilization tuples */
	class UtilizationTupleList{
	public:
		typedef std::vector<UtilizationTuple>::iterator iterator;
		bool insert(const UtilizationTuple& tuple);
		iterator begin() { return tuples.begin(); }
		iterator end() { return tuples.end(); }
		bool pop(UtilizationTuple& tuple);
	private:
		std::vector<UtilizationTuple> tuples;
	};
public:
	static double computeUtilization(const std::vector<DRT::Matrix::MatrixTask*>& tasks);
	double computeUtilization(const Matrix::MatrixTask* task);
private:
	double bestUtilization;							///< Best utilization so far (for a loop)
	std::vector<UtilizationTupleList> waitingLists;	///< Tuples waiting for to be expanded
	std::vector<UtilizationTupleList> tupleLists;	///< Tuples generated so far...
	const Matrix::MatrixTask* _task;
	bool expandTriples(Matrix::MatrixTask::JobId job);
};

} /* Verification */
} /* DRT */

#endif /* WANGSUTILIZATIONALGORITHM_H */
