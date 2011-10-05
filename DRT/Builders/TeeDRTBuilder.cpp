#include "TeeDRTBuilder.h"

#include <assert.h>

namespace DRT{
namespace Builders{

TeeDRTBuilder::TeeDRTBuilder(AbstractDRTBuilder* builder1,
							 AbstractDRTBuilder* builder2,
							 AbstractDRTBuilder* builder3,
							 AbstractDRTBuilder* builder4,
							 AbstractDRTBuilder* builder5){
	working = false;
	if(builder1) builders.push_back(builder1);
	if(builder2) builders.push_back(builder2);
	if(builder3) builders.push_back(builder3);
	if(builder4) builders.push_back(builder4);
	if(builder5) builders.push_back(builder5);
}

/** Add builder to the TeeDRTBuilder, true if successful */
bool TeeDRTBuilder::addBuilder(AbstractDRTBuilder* builder){
	assert(builder && !working);
	if(builder && !working)
		builders.push_back(builder);
	return builder && !working;
}

void TeeDRTBuilder::createTask(const TaskArgs& args){
	working = true;
	for(BuilderList::iterator builder = builders.begin(); builder != builders.end(); builder++)
		(*builder)->createTask(args);
}

void TeeDRTBuilder::addJob(const JobArgs& args){
	for(BuilderList::iterator builder = builders.begin(); builder != builders.end(); builder++)
		(*builder)->addJob(args);
}

void TeeDRTBuilder::addEdge(const EdgeArgs& args){
	for(BuilderList::iterator builder = builders.begin(); builder != builders.end(); builder++)
		(*builder)->addEdge(args);
}

void TeeDRTBuilder::taskCreated(const TaskArgs& args){
	for(BuilderList::iterator builder = builders.begin(); builder != builders.end(); builder++)
		(*builder)->taskCreated(args);
}

/** Clears the internal list of builders */
void TeeDRTBuilder::finish(){
	working = false;
	for(BuilderList::iterator builder = builders.begin(); builder != builders.end(); builder++)
		(*builder)->finish();
	builders.clear();
}

} /* Builders */
} /* DRT */
