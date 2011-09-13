#include "ValidatingDRTBuilder.h"

ValidatingDRTBuilder::ValidatingDRTBuilder(std::ostream& err) : _err(err) {
	reset();
}

/** Reset validating builder */
void ValidatingDRTBuilder::reset(){
	_creatingTask = false;
	_jobs.clear();
	_tasks.clear();
	_hasError = false;
}

/** True, if an error was detected */
bool ValidatingDRTBuilder::hasError () const {
	return _hasError;
}

/** Report error to what-ever error stream we've got */
void ValidatingDRTBuilder::reportError(const std::string& message){
	_err << "ValidatingDRTBuilder: " << std::endl;
	_hasError = true;
}

/** Check if given job exists */
bool ValidatingDRTBuilder::hasJob(const std::string& name){
	bool exists = false;
	for(StringList::iterator it = _jobs.begin(); it != _jobs.end(); it++)
		exists |= (*it == name);
	return exists;
}

void ValidatingDRTBuilder::createTask(const AbstractDRTBuilder::TaskArgs& args){
	if(_creatingTask)
		reportError("createTask() called before previous task creation was finished with taskCreated()");
	_cTask = args;
	_allowingJobs = true;
	_creatingTask = true;
	_jobs.clear();
	bool nameClash = false;
	for(StringList::iterator it = _tasks.begin(); it != _tasks.end(); it++)
		nameClash |= (*it == args.name);
	if(nameClash)
		reportError("Task name must be unique");
	else
		_tasks.push_back(args.name);
}

void ValidatingDRTBuilder::addJob(const AbstractDRTBuilder::JobArgs& args){
	if(!_creatingTask)
		reportError("addJob() called before createTask() or after taskCreated()");
	else if(!_allowingJobs)
		reportError("addJob() cannot be called after addEdge()");
	else if(hasJob(args.name))
		reportError("Jobs must have unique names");
	else if(args.wcet > args.deadline)
		reportError("Relative deadline cannot be bigger than worst case execution time");
	else
		_jobs.push_back(args.name);
}

void ValidatingDRTBuilder::addEdge(const AbstractDRTBuilder::EdgeArgs& args){
	_allowingJobs = false;
	if(!_creatingTask)
		reportError("addEdge called before createTask() or after taskCreated()");
	else{
		if(!hasJob(args.src))
			reportError("Referenced source job in addEdge() doesn't exists");
		if(!hasJob(args.dst))
			reportError("Referenced source job in addEdge() doesn't exists");
		if(args.mtime < 0)
			reportError("Minimum inter-release time cannot be negative");
	}
}

void ValidatingDRTBuilder::taskCreated(const AbstractDRTBuilder::TaskArgs& args){
	if(!_creatingTask)
		reportError("taskCreated() called before createTask()");
	else if(_cTask != args)
		reportError("taskCreated() called with different arguments from createTask()");
	_jobs.clear();
}


