#include "MatrixDRTBuilder.h"
#include "MatrixTask.h"

#include <assert.h>
#include <limits.h>
#include <string>

namespace DRT{
namespace Matrix{

MatrixDRTBuilder::MatrixDRTBuilder(){
	hasTask = false;
}

MatrixDRTBuilder::~MatrixDRTBuilder(){
	for(size_t i = 0; i < tasks.size(); i++)
		delete tasks[i];
}

/** Create a task */
void MatrixDRTBuilder::createTask(const TaskArgs& args){
	assert(!hasTask);
	if(hasTask) taskCreated(task);
	hasTask = true;
	task = args;
}

/** Add a new job (valid between calls to createTask and taskCreated) */
void MatrixDRTBuilder::addJob(const JobArgs& args){
	jobs.push_back(args);
}

/** Add a new edge (valid between calls to createTask and taskCreated) */
void MatrixDRTBuilder::addEdge(const EdgeArgs& args){
	edges.push_back(args);
}

/** Invoke this when all jobs and edges in the task have been created */
void MatrixDRTBuilder::taskCreated(const TaskArgs& args){
	unsigned int nb_jobs = jobs.size();
	MatrixTask* t = new MatrixTask(nb_jobs);
	t->_mtimes = (int*)new int[nb_jobs * nb_jobs];
	t->_jobs = (MatrixTask::Job*)new MatrixTask::Job[nb_jobs];
	t->_name = task.name;

	for(unsigned int i = 0; i < nb_jobs; i++){
		t->_jobs[i].name = jobs[i].name;
		t->_jobs[i].wcet = jobs[i].wcet;
		t->_jobs[i].deadline = jobs[i].deadline;
	}

	for(unsigned int i = 0; i < nb_jobs; i++)
		for(unsigned int j = 0; i < nb_jobs; j++)
			t->_mtimes[i + j * nb_jobs] = INT_MAX;

	tasks.push_back(t);

	hasTask = false;
	task = TaskArgs();
	jobs.clear();
	edges.clear();
}

/** Finsh build, please call produce to get the output and reset internal state */
void MatrixDRTBuilder::finish(){
	// Reset stuff, so we can start over...
	hasTask = false;
}

/** Produce output */
std::vector<MatrixTask*> MatrixDRTBuilder::produce(){
	std::vector<MatrixTask*> retval = tasks;
	tasks.clear();
	return retval;
}

} /* Matrix */
} /* DRT */