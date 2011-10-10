#ifndef TASKDBF_H
#define TASKDBF_H

#include "AbstractDBF.h"
#include "CompositeDBF.h"
#include "DiscreteDBF.h"

#include "../Matrix/MatrixTask.h"

#include <vector>

namespace DRT{
namespace Verification{

/** Demand bound function for a task */
class TaskDBF : public virtual AbstractDBF, private SequentialDBF{
	typedef std::vector<DiscreteDBF*> DBFList;
public:
	TaskDBF(const Matrix::MatrixTask* task, int time);
	~TaskDBF();
	using SequentialDBF::operator();
	using SequentialDBF::steps;
	static AbstractDBF* DRTDemandBoundFunction(const std::vector<Matrix::MatrixTask*>& tasks, int time);
	static int DBFBound(const std::vector<Matrix::MatrixTask*>& tasks, double utilization);
private:
	void compute(int time);
	bool expandTuples(Matrix::MatrixTask::JobId job, int time);
	DBFList tupleLists,					///< Tuple lists for all generated tuples
			waitingLists;				///< Tuple lists for tuples waiting to be expanded
	const Matrix::MatrixTask& task;		///< Task this DBF represents
};

} /* Verification */
} /* DRT */

#endif /* TASKDBF_H */
