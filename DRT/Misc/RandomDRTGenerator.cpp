#include "RandomDRTGenerator.h"

#include "../AbstractDRTBuilder.h"

#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

namespace DRT{
namespace Misc{

/** Create a random DRT generator */
RandomDRTGenerator::RandomDRTGenerator(){
	setTaskLimits(Interval<int>(1, 10));
	setJobLimits(Interval<int>(10, 50));
	setEdgeLimits(Interval<int>(30, 70));
	setDelayLimits(Interval<int>(1, 15), true);
	setWCETLimits(Interval<int>(1, 5));
	setSlackLimits(Interval<int>(2, 15));
}

/** Convert int to string */
string RandomDRTGenerator::int2str(int n) const {
	stringstream out;
	out << n;
	return out.str();
}

/** Set minimum and maximum number of tasks generated, value will not be altered if negative */
void RandomDRTGenerator::setTaskLimits(Interval<int> limit){
	_taskLimits = limit;
}

/** Set minimum and maximum number of jobs generated per task, value will not be altered if negative */
void RandomDRTGenerator::setJobLimits(Interval<int> limit){
	_jobLimits = limit;
}

/** Set minimum and maximum number of edges generated per task, value will not be altered if negative
 * @remarks This generator will never add more than V*V/2 edges as it would be inefficient to do at random
 */
void RandomDRTGenerator::setEdgeLimits(Interval<int> limit){
	_edgeLimits = limit;
}

/** Set minimum and maximum delay (minimum inter-release time), value will not be altered if negative */
void RandomDRTGenerator::setDelayLimits(Interval<int> limit, bool forceFrameSeparation){
	_delayLimits = limit;
	this->forceFrameSeparation = forceFrameSeparation;
}

/** Set minimum and maximum worst case execution time, value will not be altered if negative */
void RandomDRTGenerator::setWCETLimits(Interval<int> limit){
	_wcetLimits = limit;
}

/** Set minimum and maximum slack = deadline - wcet, value will not be altered if negative */
void RandomDRTGenerator::setSlackLimits(Interval<int> limit){
	_slackLimits = limit;
}

/** Get a random number in the interval [ min ; max ] */
/*int RandomDRTGenerator::random(int min, int max) const{
	return min + rand() % (++max - min > 0 ? max - min : 1);
}*/

/** Generator a random task set with the given parameters
 * @remarks Given 0 (default) as seed, a random seed will be used
 */
void RandomDRTGenerator::generate(AbstractDRTBuilder* builder, unsigned int seed) const{
	Interval<int>::seed(seed);

	// Create a random number of tasks
	int tasks = _taskLimits.random();
	for(int t = 0; t < tasks; t++){
		// Create a task
		TaskArgs task;
		task.name = "task" + int2str(t);
		builder->createTask(task);

		// Create a random number of jobs
		int jobs = _jobLimits.random();
		int deadlines[jobs];
		for(int j = 0; j < jobs; j++){
			JobArgs job;
			job.name = "job" + int2str(j);
			job.wcet = _wcetLimits.random();
			job.deadline = job.wcet + _slackLimits.random();
			deadlines[j] = job.deadline;
			builder->addJob(job);
		}

		// Create matrix for all edges, and initialize all edges to false
		// this way we can check if we already created an edge earlier
		bool edge_map[jobs * jobs];
		for(int entry = 0; entry < jobs * jobs; entry++)
			edge_map[entry] = false;

		// Create a random number of edges
		int edges = _edgeLimits.random();
		while(edges > 0){
			int src = Interval<int>(0, jobs-1).random(),
				dst = Interval<int>(0, jobs-1).random();
			// Check if proposed edge exists
			if(edge_map[src + dst * jobs]) continue;
			edge_map[src + dst * jobs] = true;		// Set the edge as created
			edges--;								// One edge less to create
			// Create proposed edge
			EdgeArgs edge;
			edge.mtime = _delayLimits.random();
			edge.src = "job" + int2str(src);
			edge.dst = "job" + int2str(dst);
			if(forceFrameSeparation && _delayLimits.min() < deadlines[src])
				edge.mtime = Interval<int>(deadlines[src], _delayLimits.max()).random();
			else
				edge.mtime = _delayLimits.random();
			builder->addEdge(edge);
		}

		// Done creating a task
		builder->taskCreated(task);
	}

	builder->finish();
}

} /* Misc */
} /* DRT */
