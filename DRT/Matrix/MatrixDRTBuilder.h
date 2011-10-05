#ifndef MATRIXDRTBUILDER_H
#define MATRIXDRTBUILDER_H

#include "../AbstractDRTBuilder.h"

#include <vector>

namespace DRT{
namespace Matrix{

class MatrixTask;

/** Matrix based DRT Builder */
class MatrixDRTBuilder : public AbstractDRTBuilder {
public:
	MatrixDRTBuilder();
	~MatrixDRTBuilder();
	void createTask(const TaskArgs& args);
	void addJob(const JobArgs& args);
	void addEdge(const EdgeArgs& args);
	void taskCreated(const TaskArgs& args);
	void finish();
	std::vector<MatrixTask*> produce();
private:
	bool hasTask;
	TaskArgs task;
	std::vector<JobArgs> jobs;
	std::vector<EdgeArgs> edges;
	std::vector<MatrixTask*> tasks;
};

} /* Matrix */
} /* DRT */

#endif /* MATRIXDRTBUILDER_H */
