#ifndef RANDOMDRTGENERATOR_H
#define RANDOMDRTGENERATOR_H

#include <string>

namespace DRT{
	class AbstractDRTBuilder;
namespace Misc{

/** Generator for building random DRTs */
class RandomDRTGenerator{
public:
	RandomDRTGenerator();
	void generate(AbstractDRTBuilder* builder, unsigned int seed = 0) const;
	void setTaskLimits(int min = -1, int max = -1);
	void setJobLimits(int min = -1, int max = -1);
	void setEdgeLimits(int min = -1, int max = -1);
	void setDelayLimits(int min = -1, int max = -1, bool forceFrameSeparation = true);
	void setWCETLimits(int min = -1, int max = -1);
	void setSlackLimits(int min = -1, int max = -1);
private:
	int random(int min, int max) const;
	std::string int2str(int n) const;
	bool forceFrameSeparation;
	int minTasks;				///< Minimum number of tasks
	int maxTasks;				///< Maximum number of tasks
	int minJobs;				///< Minimum number of jobs
	int maxJobs;				///< Maximum number of jobs
	int minEdges;				///< Minimum number of edges
	int maxEdges;				///< Maximum number of edges
	int minDelay;				///< Minimum minimum inter-release time
	int maxDelay;				///< Maximum minimum inter-release time
	int minWCET;				///< Minimum worst case execution time
	int maxWCET;				///< Maximum worst case execution time
	int minSlack;				///< Minimum slack < deadline - wcet
	int maxSlack;				///< Maximum slack > deadline - wcet
};

} /* Misc */
} /* DRT */

#endif /* RANDOMDRTGENERATOR_H */
