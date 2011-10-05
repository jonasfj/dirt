#ifndef XMLDRTBUILDER_H
#define XMLDRTBUILDER_H

#include "../AbstractDRTBuilder.h"

#include <ostream>

namespace DRT{
namespace Xml{

/** Build Xml DRT Document */
class XmlDRTBuilder : public AbstractDRTBuilder {
public:
	XmlDRTBuilder(std::ostream& output);
	void createTask(const TaskArgs& args);
	void addJob(const JobArgs& args);
	void addEdge(const EdgeArgs& args);
	void taskCreated(const TaskArgs& args);
	void finish();
private:
	std::string escape(const std::string& text) const;
	std::ostream& out;					///< Output stream
};

} /* Xml */
} /* DRT */

#endif /* XMLDRTBUILDER_H */
