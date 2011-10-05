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
	setTaskLimits(1, 10);
	setJobLimits(10, 50);
	setEdgeLimits(30, 70);
	setDelayLimits(1, 15, true);
	setWCETLimits(1, 5);
	setSlackLimits(2, 15);
}

/** Convert int to string */
string RandomDRTGenerator::int2str(int n) const {
	stringstream out;
	out << n;
	return out.str();
}

/** Set minimum and maximum number of tasks generated, value will not be altered if negative */
void RandomDRTGenerator::setTaskLimits(int min, int max){
	if(min >= 0) minTasks = min;
	if(max >= 0) maxTasks = max;
}

/** Set minimum and maximum number of jobs generated per task, value will not be altered if negative */
void RandomDRTGenerator::setJobLimits(int min, int max){
	if(min >= 0) minJobs = min;
	if(max >= 0) maxJobs = max;
}

/** Set minimum and maximum number of edges generated per task, value will not be altered if negative
 * @remarks This generator will never add more than V*V/2 edges as it would be inefficient to do at random
 */
void RandomDRTGenerator::setEdgeLimits(int min, int max){
	if(min >= 0) minEdges = min;
	if(max >= 0) maxEdges = max;
}

/** Set minimum and maximum delay (minimum inter-release time), value will not be altered if negative */
void RandomDRTGenerator::setDelayLimits(int min, int max, bool forceFrameSeparation){
	if(min >= 0) minDelay = min;
	if(max >= 0) maxDelay = max;
	this->forceFrameSeparation = forceFrameSeparation;
}

/** Set minimum and maximum worst case execution time, value will not be altered if negative */
void RandomDRTGenerator::setWCETLimits(int min, int max){
	if(min >= 0) minWCET = min;
	if(max >= 0) maxWCET = max;
}

/** Set minimum and maximum slack = deadline - wcet, value will not be altered if negative */
void RandomDRTGenerator::setSlackLimits(int min, int max){
	if(min >= 0) minSlack = min;
	if(max >= 0) maxSlack = max;
}

/** Get a random number in the interval [ min ; max ] */
int RandomDRTGenerator::random(int min, int max) const{
	return min + rand() % (++max - min > 0 ? max - min : 1);
}

/** Generator a random task set with the given parameters
 * @remarks Given 0 (default) as seed, a random seed will be used
 */
void RandomDRTGenerator::generate(AbstractDRTBuilder* builder, unsigned int seed) const{
	if(seed == 0)
		seed = time(NULL);		// Don't use clock(), it's reset at application start
	srand(seed);

	// Create a random number of tasks
	int tasks = random(minTasks, maxTasks);
	for(int t = 0; t < tasks; t++){
		// Create a task
		TaskArgs task;
		task.name = "task" + int2str(t);
		builder->createTask(task);

		// Create a random number of jobs
		int jobs = random(minJobs, maxJobs);
		int deadlines[jobs];
		for(int j = 0; j < jobs; j++){
			JobArgs job;
			job.name = "job" + int2str(j);
			job.wcet = random(minWCET, maxWCET);
			job.deadline = job.wcet + random(minSlack, maxSlack);
			deadlines[j] = job.deadline;
			builder->addJob(job);
		}

		// Create matrix for all edges, and initialize all edges to false
		// this way we can check if we already created an edge earlier
		bool edge_map[jobs * jobs];
		for(int entry = 0; entry < jobs * jobs; entry++)
			edge_map[entry] = false;

		// Create a random number of edges
		int edges = random(minEdges, maxEdges);
		while(edges > 0){
			int src = random(0, jobs-1),
				dst = random(0, jobs-1);
			// Check if proposed edge exists
			if(edge_map[src + dst * jobs]) continue;
			edge_map[src + dst * jobs] = true;		// Set the edge as created
			edges--;								// One edge less to create
			// Create proposed edge
			EdgeArgs edge;
				edge.mtime = random(minDelay, maxDelay);
			edge.src = "job" + int2str(src);
			edge.dst = "job" + int2str(dst);
			if(forceFrameSeparation && minDelay < deadlines[src])
				edge.mtime = random(deadlines[src], maxDelay);
			else
				edge.mtime = random(minDelay, maxDelay);
			builder->addEdge(edge);
		}

		// Done creating a task
		builder->taskCreated(task);
	}

	builder->finish();
}

} /* Misc */
} /* DRT */
