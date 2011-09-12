#ifndef DRTXMLPARSER_H
#define DRTXMLPARSER_H

namespace XMLSP{
	class DOMElement;
}
class AbstractDRTBuilder;

#include <string>

/** Parser for reading DRT-XML files */
class DRTXmlParser{
public:
	DRTXmlParser();
	~DRTXmlParser();
	void parse(const std::string& xml, AbstractDRTBuilder* builder) const;
private:
	void parseElement(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseTask(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseJob(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
	void parseEdge(XMLSP::DOMElement* element, AbstractDRTBuilder* builder) const;
};

#endif /* DRTXMLPARSER_H */
