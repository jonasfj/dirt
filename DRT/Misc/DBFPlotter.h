#ifndef DBFPLOTTER_H
#define DBFPLOTTER_H

#include <ostream>
#include <vector>

#include "../Verification/AbstractDBF.h"

namespace DRT{
namespace Misc{

/** Class for plotting demand bound function
 * Writes gnuplot commands with inline CSV
 */
class DBFPlotter{
	typedef std::vector<Verification::AbstractDBF::StepIterator*> StepIterList;
public:
	DBFPlotter(std::ostream& output, const std::string& title = "");
	~DBFPlotter();
	void addDBF(const Verification::AbstractDBF& dbf, const std::string& title = "");
	void finish();
private:
	std::string _title;				///< Title of plots produced with this class
	std::ostream& output;				///< Output stream to write data to
	StepIterList steppers;			///< StepIterators to output
};

} /* Misc */
} /* DRT */

#endif /* DBFPLOTTER_H */
