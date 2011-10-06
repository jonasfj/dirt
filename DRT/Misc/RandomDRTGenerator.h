#ifndef RANDOMDRTGENERATOR_H
#define RANDOMDRTGENERATOR_H

#include <string>

#include "Interval.h"

namespace DRT{
	class AbstractDRTBuilder;
namespace Misc{

/** Generator for building random DRTs */
class RandomDRTGenerator{
public:
	RandomDRTGenerator();
	void generate(AbstractDRTBuilder* builder, unsigned int seed = 0) const;
	void setTaskLimits(Interval<int> limit);
	void setJobLimits(Interval<int> limit);
	void setEdgeLimits(Interval<int> limit);
	void setDelayLimits(Interval<int> limit, bool forceFrameSeparation = true);
	void setWCETLimits(Interval<int> limit);
	void setSlackLimits(Interval<int> limit);
private:
//	int random(int min, int max) const;
	std::string int2str(int n) const;
	bool forceFrameSeparation;
	Interval<int> _taskLimits;	///< Min/max number of tasks
	Interval<int> _jobLimits;	///< Min/max number of jobs
	Interval<int> _edgeLimits;	///< Min/max number of edges
	Interval<int> _delayLimits;	///< Min/max minimum inter-release time
	Interval<int> _wcetLimits;	///< Min/max worst case execution time
	Interval<int> _slackLimits;	///< Min/max slack, wcet + slack = deadline
};

} /* Misc */
} /* DRT */

#endif /* RANDOMDRTGENERATOR_H */
