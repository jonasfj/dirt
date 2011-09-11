#ifndef MATRIXDRTBUILDER_H
#define MATRIXDRTBUILDER_H

#include "AbstractDRTBuilder.h"

#include <vector>

class MatrixTask;

/** Matrix based DRT Builder */
class MatrixDRTBuilder : public AbstractDRTBuilder {
public:
	MatrixDRTBuilder();
	~MatrixDRTBuilder();
	void createTask(const AbstractDRTBuilder::TaskArgs& args);
	void addJob(const AbstractDRTBuilder::JobArgs& args);
	void addEdge(const AbstractDRTBuilder::EdgeArgs& args);
	void taskCreated(const AbstractDRTBuilder::TaskArgs& args);
	std::vector<MatrixTask*> produce();
private:
	bool hasTask;
	AbstractDRTBuilder::TaskArgs task;
	std::vector<AbstractDRTBuilder::JobArgs> jobs;
	std::vector<AbstractDRTBuilder::EdgeArgs> edges;
	std::vector<MatrixTask*> tasks;
};

#endif /* MATRIXDRTBUILDER_H */
