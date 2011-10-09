#include <DRT/Verification/WangsUtilizationAlgorithm.h>
#include <DRT/Matrix/MatrixDRTBuilder.h>
#include <DRT/AbstractDRTBuilder.h>

#include "Check.h"

using namespace DRT;
using namespace DRT::Matrix;
using namespace DRT::Verification;

/** Test if WangsUtilizationAlgorithm works */
int main(){
	MatrixDRTBuilder b;
	TaskArgs task;
	task.name = "task1";
	b.createTask(task);

	JobArgs job;
	job.name = "j1";
	job.wcet = 5;
	job.deadline = 100;

	b.addJob(job);

	job.name = "j2";
	b.addJob(job);

	EdgeArgs edge;
	edge.src = "j1";
	edge.dst = "j2";
	edge.mtime = 10;

	b.addEdge(edge);

	edge.src = "j2";
	edge.dst = "j1";
	b.addEdge(edge);

	//TODO Make a more complicated test... 

	b.taskCreated(task);

	std::vector<MatrixTask*> drt = b.produce();

	CHECK(drt.size() == 1);

	double U = WangsUtilizationAlgorithm::computeUtilization(drt);
	CHECK(U == 0.5);

	return 0;
}
