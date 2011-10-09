#include <DRT/Verification/DiscreteDBF.h>

#include "Check.h"

using namespace DRT::Verification;

/** Test if StepIterator works */
int main(){
	DiscreteDBF dbf;
	CHECK(dbf.insert(DemandTuple(2,4)));
	CHECK(dbf.insert(DemandTuple(4,8)));
	CHECK(dbf.insert(DemandTuple(6,10)));
	AbstractDBF::StepIterator* step = dbf.steps();
	CHECK(0 == step->wcet());
	CHECK(0 == step->time());
	CHECK(step->next());
	CHECK(2 == step->wcet());
	CHECK(4 == step->time());
	CHECK(step->next());
	CHECK(4 == step->wcet());
	CHECK(8 == step->time());
	CHECK(step->next());
	CHECK(6 == step->wcet());
	CHECK(10 == step->time());
	CHECK(!step->next());
	CHECK(!step->end());
	delete step;
	step = NULL;
	return 0;
}

