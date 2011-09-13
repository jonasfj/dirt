#include "TeeDRTBuilder.h"

#include <assert.h>

TeeDRTBuilder::TeeDRTBuilder(AbstractDRTBuilder* builder1, AbstractDRTBuilder* builder2){
	assert(builder1);
	assert(builder2);
	_builder1 = builder1;
	_builder2 = builder2;
}

void TeeDRTBuilder::createTask(const AbstractDRTBuilder::TaskArgs& args){
	_builder1->createTask(args);
	_builder2->createTask(args);
}

void TeeDRTBuilder::addJob(const AbstractDRTBuilder::JobArgs& args){
	_builder1->addJob(args);
	_builder2->addJob(args);
}

void TeeDRTBuilder::addEdge(const AbstractDRTBuilder::EdgeArgs& args){
	_builder1->addEdge(args);
	_builder2->addEdge(args);
}

void TeeDRTBuilder::taskCreated(const AbstractDRTBuilder::TaskArgs& args){
	_builder1->taskCreated(args);
	_builder2->taskCreated(args);
}


