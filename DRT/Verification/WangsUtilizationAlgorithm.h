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
	};
	/** List of utilization tuples */
	class UtilizationTupleList{
	public:
		typedef std::vector<UtilizationTuple>::iterator iterator;
		void insert(const UtilizationTuple& tuple);
		iterator begin() { return tuples.begin(); }
		iterator end() { return tuples.end(); }
		iterator erase(iterator it) { return tuples.erase(it); }
	private:
		std::vector<UtilizationTuple> tuples;
	};
public:
	double computeUtilization(const Matrix::MatrixTask* task);
private:
	std::vector<UtilizationTupleList> places;	///< A UtilizationTupleList for each vertex/place
	const Matrix::MatrixTask* _task;
	void expandTriples(Matrix::MatrixTask::JobId place);
};

} /* Verification */
} /* DRT */

#endif /* WANGSUTILIZATIONALGORITHM_H */
