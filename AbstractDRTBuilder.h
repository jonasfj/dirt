#ifndef ABSTRACTDRTBUILDER_H
#define ABSTRACTDRTBUILDER_H

#include <string>

/** Abstract DRT Builder */
class AbstractDRTBuilder{
public:
	/** Arguments for creating a task
	 * @remarks Using this structure we can add arguments without updating all implementations
	 * DRTBuilder, and if arguments have default value, we don't even need to modify uses of DRTBuilder.
	 */
	struct TaskArgs{
		std::string name;	///< Unique name of the task
		bool operator!=(const TaskArgs& args) const { return name != args.name; }
	};
	/** Arguments for creating a new job */
	struct JobArgs{
		JobArgs() {wcet = deadline = x = y = 0;}
		std::string name;	///< Unique name of the job (within the current task)
		int wcet;			///< Worst case execution time
		int deadline;		///< Deadline after release
		double x;			///< x-coordinate of position
		double y;			///< y-coordinate of position
	};
	/** Arguments for creating a new edge */
	struct EdgeArgs{
		EdgeArgs(){mtime = 0;}
		std::string src;	///< Source job
		std::string dst;	///< Destination job
		int mtime;			///< Minimum inter-release time
	};
	/** Empty abstract destructor, might be useful for subclasses */
	virtual ~AbstractDRTBuilder(){}
	/** Create a task */
	virtual void createTask(const TaskArgs& args) = 0;
	/** Add a new job (valid between calls to createTask and taskCreated) */
	virtual void addJob(const JobArgs& args) = 0;
	/** Add a new edge (valid between calls to createTask and taskCreated)
	 * @remarks It's a assumed that all jobs are added, before the first edge is added
	 */
	virtual void addEdge(const EdgeArgs& args) = 0;
	/** Invoke this when all jobs and edges in the task have been created */
	virtual void taskCreated(const TaskArgs& args) = 0;
};

#endif /* ABSTRACTDRTBUILDER_H */
