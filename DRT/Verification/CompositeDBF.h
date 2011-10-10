#ifndef PARALLELDBF_H
#define PARALLELDBF_H

#include "AbstractDBF.h"

#include <vector>

namespace DRT{
namespace Verification{

/** Abstract base class for on-the-fly composition of demand bound functions */
class CompositeDBF : public virtual AbstractDBF{
protected:
	typedef std::vector<const AbstractDBF*> DBFList;
	typedef DBFList::const_iterator ConstDBFIterator;
	/** Abstract base class for on-the-fly composite step iterators */
	class CompositeStepIterator : public StepIterator{
	protected:
		typedef std::vector<StepIterator*> StepIterList;
	public:
		CompositeStepIterator(const DBFList& dbfs);
		~CompositeStepIterator();
	protected:
		StepIterList steppers;		///< StepIterators
	};
public:
	~CompositeDBF();
	void addDBF(const AbstractDBF* dbf);
protected:
	DBFList dbfs;
};


/** On-the-fly parallel representation of two demand bound functions */
class ParallelDBF : public CompositeDBF{
	class ParallelStepIterator : public CompositeStepIterator{
	public:
		ParallelStepIterator(const DBFList& dbfs);
		bool next();
	private:
		std::vector<int> prevWcet;			///< Previous wcet's of the steppers
	};
public:
	int operator()(int time) const;
	StepIterator* steps() const;
};

/** On-the-fly sequential representation of two demand bound functions */
class SequentialDBF : public CompositeDBF{
	class SequentialStepIterator : public CompositeStepIterator{
	public:
		SequentialStepIterator(const DBFList& dbfs) : CompositeStepIterator(dbfs) {}
		bool next();
	};
public:
	int operator()(int time) const;
	StepIterator* steps() const;
};


} /* Verification */
} /* DRT */

#endif /* PARALLELDBF_H */
