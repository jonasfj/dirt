#ifndef VALIDATINGDRTBUILDER_H
#define VALIDATINGDRTBUILDER_H

#include "AbstractDRTBuilder.h"

#include <string>
#include <vector>
#include <iostream>

/** Validate the input for a DRT builder */
class ValidatingDRTBuilder : public AbstractDRTBuilder{
	typedef std::vector<std::string> StringList;
public:
	ValidatingDRTBuilder(std::ostream& err = std::cerr);
	void createTask(const AbstractDRTBuilder::TaskArgs& args);
	void addJob(const AbstractDRTBuilder::JobArgs& args);
	void addEdge(const AbstractDRTBuilder::EdgeArgs& args);
	void taskCreated(const AbstractDRTBuilder::TaskArgs& args);
	bool hasError () const;
	void reset();
private:
	void reportError(const std::string& message);
	bool hasJob(const std::string& name);
	bool _allowingJobs;						///< Is allowing new jobs to be added
	bool _hasError;							///< Has detected error
	bool _creatingTask;						///< True, if currently taskCreated haven't been called
	std::ostream& _err;						///< Error stream
	AbstractDRTBuilder::TaskArgs _cTask;	///< Current task
	StringList _jobs;						///< Jobs added
	StringList _tasks;						///< Tasks added
};

#endif /* VALIDATINGDRTBUILDER_H */
