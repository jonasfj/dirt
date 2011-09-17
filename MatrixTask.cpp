#include "MatrixTask.h"

#include "AbstractDRTBuilder.h"

/** Produce this task on a DRT builder */
void MatrixTask::produce(AbstractDRTBuilder* builder) const{
	// Create task arguments
	AbstractDRTBuilder::TaskArgs task;
	task.name = name();

	// Create this task
	builder->createTask(task);

	// Foreach job, create it
	for(JobId job = 0; job < jobs(); job++){
		AbstractDRTBuilder::JobArgs args;
		args.name = name(job);
		args.wcet = wcet(job);
		args.deadline = deadline(job);
		// Create the job
		builder->addJob(args);
	}

	// Foreach job, add it's edges
	for(JobId j = 0; j < jobs(); j++){
		AbstractDRTBuilder::EdgeArgs args;
		args.src = j;
		for(JobId i = 0; i < jobs(); i++){
			if((args.mtime = mtime(j, i)) != INT_MAX){
				args.dst = i;
				builder->addEdge(args);
			}
		}
	}

	// Done creating this task
	builder->taskCreated(task);
}

/** Produce a matrix DRT system to a builder  */
void MatrixTask::produce(const std::vector<MatrixTask*>& tasks, AbstractDRTBuilder* builder){
	for(std::vector<MatrixTask*>::const_iterator it = tasks.begin(); it != tasks.end(); it++)
		(*it)->produce(builder);
}

