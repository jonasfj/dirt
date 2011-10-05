#ifndef TEEDRTBUILDER_H
#define TEEDRTBUILDER_H

#include "../AbstractDRTBuilder.h"

#include <vector>

namespace DRT{
namespace Builders{

/** Build two DRT's with a single builder */
class TeeDRTBuilder : public AbstractDRTBuilder{
	typedef std::vector<AbstractDRTBuilder*> BuilderList;
public:
	TeeDRTBuilder(AbstractDRTBuilder* builder1 = NULL,
				  AbstractDRTBuilder* builder2 = NULL,
				  AbstractDRTBuilder* builder3 = NULL,
				  AbstractDRTBuilder* builder4 = NULL,
				  AbstractDRTBuilder* builder5 = NULL);
	bool addBuilder(AbstractDRTBuilder* builder);
	void createTask(const TaskArgs& args);
	void addJob(const JobArgs& args);
	void addEdge(const EdgeArgs& args);
	void taskCreated(const TaskArgs& args);
	void finish();
private:
	BuilderList builders;
	bool working;
};

} /* Builders */
} /* DRT */

#endif /* TEEDRTBUILDER_H */
