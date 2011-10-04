#include "XmlDRTParser.h"

#include "xmlsp/xmlsp_dom.h"
#include "AbstractDRTBuilder.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sstream>

using namespace XMLSP;
using namespace std;

/** Create DRT-Xml parser*/
XmlDRTParser::XmlDRTParser(){}

/** Release resources held by this parser */
XmlDRTParser::~XmlDRTParser(){}

/** Parse XML string to DRT-builder
 * @remarks Well formed XML is assumed, that is the order of tags actually matters!
 */
bool XmlDRTParser::parse(istream& data, AbstractDRTBuilder* builder) const {
	stringstream buffer;
	buffer << data.rdbuf();
	return parse(buffer.str(), builder);
}

/** Parse XML string to DRT-builder
 * @remarks Well formed XML is assumed, that is the order of tags actually matters!
 */
bool XmlDRTParser::parse(const string& xml, AbstractDRTBuilder* builder) const {
	// Parse the Xml string
	DOMElement* root = DOMElement::loadXML(xml);
	assert(root);
	if(root){
		parseElement(root, builder);
		delete root;
	}
	return root != NULL;
}

void XmlDRTParser::parseElement(DOMElement* element, AbstractDRTBuilder* builder) const {
	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		string name = (*it)->getElementName();
		if(name == "task")
			parseTask(*it, builder);
		else
			parseElement(*it, builder);
	}
}

void XmlDRTParser::parseTask(DOMElement* element, AbstractDRTBuilder* builder) const {
	assert(element->hasAttribute("name"));
	if(!element->hasAttribute("name")){
		fprintf(stderr, "Attribute on missing in XmlDRTParser::parseTask\n");
		return;
	}

	AbstractDRTBuilder::TaskArgs args;
	args.name = element->getAttribute("name");

	builder->createTask(args);

	EdgeList edges;

	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		string name = (*it)->getElementName();
		if(name == "job"){
			parseJob(*it, builder);
		}else if(name == "edge"){
			parseEdge(*it, edges);
		}else
			assert(false);
	}
	for(EdgeList::iterator edge = edges.begin(); edge != edges.end(); edge++)
		builder->addEdge(*edge);

	builder->taskCreated(args);
}

void XmlDRTParser::parseJob(DOMElement* element, AbstractDRTBuilder* builder) const {
	assert(element->hasAttribute("name") && element->hasAttribute("wcet") && element->hasAttribute("deadline"));
	if(!element->hasAttribute("name") || !element->hasAttribute("wcet") || !element->hasAttribute("deadline")){
		fprintf(stderr, "Attribute on missing in XmlDRTParser::parseJob\n");
		return;
	}

	AbstractDRTBuilder::JobArgs args;
	args.name = element->getAttribute("name");
	args.wcet = atoi(element->getAttribute("wcet").c_str());
	args.deadline = atoi(element->getAttribute("deadline").c_str());
	if(element->hasAttribute("x"))
		args.x = atof(element->getAttribute("x").c_str());
	if(element->hasAttribute("y"))
		args.y = atof(element->getAttribute("y").c_str());
	builder->addJob(args);
}

void XmlDRTParser::parseEdge(DOMElement* element, EdgeList& edges) const {
	assert(element->hasAttribute("source") &&
		   element->hasAttribute("destination") &&
		   element->hasAttribute("delay"));
	if(!element->hasAttribute("source") ||
	   !element->hasAttribute("destination") ||
	   !element->hasAttribute("delay")){
		fprintf(stderr, "Attribute on missing in XmlDRTParser::parseEdge\n");
		return;
	}
	AbstractDRTBuilder::EdgeArgs args;
	args.src = element->getAttribute("source");
	args.dst = element->getAttribute("destination");
	args.mtime = atoi(element->getAttribute("delay").c_str());
	edges.push_back(args);
}
