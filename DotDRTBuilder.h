#ifndef DOTDRTBUILDER_H
#define DOTDRTBUILDER_H

#include "AbstractDRTBuilder.h"

#include <vector>
#include <string>
#include <ostream>

/** Build a dot graph with a DRT builder */
class DotDRTBuilder : public AbstractDRTBuilder {
	/** Job-id entry */
	struct JobEntry {
		std::string name;				///< Job name
		int id;							///< Job id
	};
public:
	DotDRTBuilder(std::ostream& output);
	void createTask(const AbstractDRTBuilder::TaskArgs& args);
	void addJob(const AbstractDRTBuilder::JobArgs& args);
	void addEdge(const AbstractDRTBuilder::EdgeArgs& args);
	void taskCreated(const AbstractDRTBuilder::TaskArgs& args);
	void finish();
private:
	int getJobId(const std::string& name) const;
	std::string escape(const std::string& text) const;
	std::ostream& out;					///< output stream
	int id;								///< Next unique id-available
	std::vector<JobEntry> jobs;			///< Link from job name to id
};

#endif /* DOTDRTBUILDER_H */
