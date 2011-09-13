#ifndef TEEDRTBUILDER_H
#define TEEDRTBUILDER_H

#include "AbstractDRTBuilder.h"

/** Build two DRT's with a single builder */
class TeeDRTBuilder : public AbstractDRTBuilder{
public:
	TeeDRTBuilder(AbstractDRTBuilder* builder1, AbstractDRTBuilder* builder2);
	void createTask(const AbstractDRTBuilder::TaskArgs& args);
	void addJob(const AbstractDRTBuilder::JobArgs& args);
	void addEdge(const AbstractDRTBuilder::EdgeArgs& args);
	void taskCreated(const AbstractDRTBuilder::TaskArgs& args);
private:
	AbstractDRTBuilder* _builder1;
	AbstractDRTBuilder* _builder2;
};

#endif /* TEEDRTBUILDER_H */
