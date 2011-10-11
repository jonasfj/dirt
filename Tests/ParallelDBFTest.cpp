#include <DRT/Verification/CompositeDBF.h>
#include <DRT/Verification/DiscreteDBF.h>

#include "Check.h"

using namespace DRT;
using namespace DRT::Verification;

/** Test if ParallelDBF works */
int main(){
	DiscreteDBF* dbf1 = new DiscreteDBF();
	DiscreteDBF* dbf2 = new DiscreteDBF();

	CHECK(dbf1->insert(DemandTuple(1, 5)));
	CHECK(dbf1->insert(DemandTuple(2, 10)));
	CHECK(dbf1->insert(DemandTuple(3, 15)));

	CHECK(dbf2->insert(DemandTuple(1, 6)));
	CHECK(dbf2->insert(DemandTuple(2, 9)));
	CHECK(dbf2->insert(DemandTuple(3, 15)));

	ParallelDBF pdbf;
	pdbf.addDBF(dbf1);
	pdbf.addDBF(dbf2);

	AbstractDBF::StepIterator* step = pdbf.steps();

	CHECK(step->wcet() == 0);
	CHECK(step->time() == 0);
	CHECK(step->next());

	CHECK(step->wcet() == 1);
	CHECK(step->time() == 5);
	CHECK(step->next());

	CHECK(step->wcet() == 2);
	CHECK(step->time() == 6);
	CHECK(step->next());

	CHECK(step->wcet() == 3);
	CHECK(step->time() == 9);
	CHECK(step->next());

	CHECK(step->wcet() == 4);
	CHECK(step->time() == 10);
	CHECK(step->next());

	CHECK(step->wcet() == 6);
	CHECK(step->time() == 15);
	CHECK(!step->next());
	CHECK(step->end());

	delete step;
	step = NULL;

	return 0;
}
