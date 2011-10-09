#include <DRT/Verification/DiscreteDBF.h>

#include "Check.h"

using namespace DRT::Verification;

/** Test if insert works, specifically if it detects domination */
int main(){
	DiscreteDBF dbf;
	CHECK(dbf.insert(DemandTuple(2, 3)));
	CHECK(dbf.insert(DemandTuple(3, 3)));
	CHECK(!dbf.insert(DemandTuple(3, 4)));	//Dominated
	CHECK(dbf.insert(DemandTuple(6, 7)));
	CHECK(dbf.insert(DemandTuple(4, 6)));
	CHECK(!dbf.insert(DemandTuple(4, 6)));	//Dominated
	CHECK(!dbf.insert(DemandTuple(3, 5)));	//Dominated
	return 0;
}

