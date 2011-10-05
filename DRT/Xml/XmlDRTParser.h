#ifndef XMLDRTPARSER_H
#define XMLDRTPARSER_H

namespace XMLSP{
	class DOMElement;
}

#include "../AbstractDRTBuilder.h"

#include <string>
#include <vector>
#include <istream>

namespace DRT{
namespace Xml{

/** Parser for reading DRT-XML files */
class XmlDRTParser{
	typedef std::vector<EdgeArgs> EdgeList;
public:
	XmlDRTParser();
	~XmlDRTParser();
	bool parse(const std::string& xml, AbstractDRTBuilder* builder) const;
	bool parse(std::istream& data, AbstractDRTBuilder* builder) const;
private:
	void parseElement(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseTask(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseJob(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseEdge(XMLSP::DOMElement* element, EdgeList& edges) const;
};

} /* Xml */
} /* DRT */

#endif /* XMLDRTPARSER_H */
