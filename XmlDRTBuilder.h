#ifndef XMLDRTBUILDER_H
#define XMLDRTBUILDER_H

#include "AbstractDRTBuilder.h"

#include <ostream>

/** Build Xml DRT Document */
class XmlDRTBuilder : public AbstractDRTBuilder {
public:
	XmlDRTBuilder(std::ostream& output);
	void createTask(const AbstractDRTBuilder::TaskArgs& args);
	void addJob(const AbstractDRTBuilder::JobArgs& args);
	void addEdge(const AbstractDRTBuilder::EdgeArgs& args);
	void taskCreated(const AbstractDRTBuilder::TaskArgs& args);
	void finish();
private:
	std::string escape(const std::string& text) const;
	std::ostream& out;					///< Output stream
};

#endif /* XMLDRTBUILDER_H */
