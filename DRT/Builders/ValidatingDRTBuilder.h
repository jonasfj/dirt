#ifndef VALIDATINGDRTBUILDER_H
#define VALIDATINGDRTBUILDER_H

#include "../AbstractDRTBuilder.h"

#include <string>
#include <vector>
#include <iostream>

namespace DRT{
namespace Builders{

/** Validate the input for a DRT builder */
class ValidatingDRTBuilder : public AbstractDRTBuilder{
	typedef std::vector<std::string> StringList;
public:
	ValidatingDRTBuilder(std::ostream& err = std::cerr);
	void createTask(const TaskArgs& args);
	void addJob(const JobArgs& args);
	void addEdge(const EdgeArgs& args);
	void taskCreated(const TaskArgs& args);
	void finish();
	bool hasError() const;
	void reset();
private:
	void reportError(const std::string& message);
	bool hasJob(const std::string& name);
	bool _hasError;							///< Has detected error
	bool _creatingTask;						///< True, if currently taskCreated haven't been called
	std::ostream& _err;						///< Error stream
	TaskArgs _cTask;	///< Current task
	StringList _jobs;						///< Jobs added
	StringList _tasks;						///< Tasks added
};

} /* Builders */
} /* DRT */

#endif /* VALIDATINGDRTBUILDER_H */
