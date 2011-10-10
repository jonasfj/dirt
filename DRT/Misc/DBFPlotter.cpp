#include "DBFPlotter.h"

using namespace std;

namespace DRT{
namespace Misc{

/** Create demand bound function plotter, with given time bound and plot title, empty string for no title */
DBFPlotter::DBFPlotter(std::ostream& output, const std::string& title, int timeBound)
 : output(output) {
	output << "set terminal png" 	<< endl
		   << "set border 3" 		<< endl
		   << "set xtic 1 nomirror" << endl
		   << "set ytic 1 nomirror" << endl;
	if(!title.empty())
		output << "set title \"" << title << "\"" << endl;		//TODO: Escape " to \"
	_timeBound = timeBound;
}

/** Release resources held by DBFPlotter */
DBFPlotter::~DBFPlotter(){
	//Just in case finish isn't called for some obscure reason
	for(size_t i = 0; i < steppers.size(); i++)
		delete steppers[i];
	steppers.clear();
}

/** Plot demand bound function with given title, empty string for notitle
 * @remarks: Do not modify any added DBFs before finished is called!
 * This method will not read the demand bound function imidiately...
 */
void DBFPlotter::addDBF(const Verification::AbstractDBF& dbf, const std::string& title){
	if(steppers.size() == 0)
		output << "plot";
	else
		output << ",";
	output << " '-' ";
	if(!title.empty())
		output << "title \"" << title << "\"";				//TODO: Escape " to \"
	else
		output << "notitle";
	output << " with steps";
	steppers.push_back(dbf.steps());
}

/** Finish the plot */
void DBFPlotter::finish(){
	output << endl;
	for(size_t i = 0; i < steppers.size(); i++){
		int last_time = 0;
		do{
			last_time = steppers[i]->time();
			output << " " << steppers[i]->time() << ", " << steppers[i]->wcet() << endl;
		}while(steppers[i]->next() && steppers[i]->time() <= _timeBound);
		if(last_time < _timeBound && _timeBound != INT_MAX)
			output << " " << _timeBound << ", " << steppers[i]->wcet() << endl;
		output << " e" << endl;
		delete steppers[i];
	}
	steppers.clear();
}

} /* Misc */
} /* DRT */
