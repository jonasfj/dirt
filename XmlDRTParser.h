#ifndef XMLDRTPARSER_H
#define XMLDRTPARSER_H

namespace XMLSP{
	class DOMElement;
}
class AbstractDRTBuilder;

#include <string>

/** Parser for reading DRT-XML files */
class XmlDRTParser{
public:
	XmlDRTParser();
	~XmlDRTParser();
	void parse(const std::string& xml, AbstractDRTBuilder* builder) const;
private:
	void parseElement(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseTask(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseJob(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseEdge(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
};

#endif /* XMLDRTPARSER_H */
