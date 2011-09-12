#include "DRTXmlParser.h"

#include "xmlsp/xmlsp_dom.h"
#include "AbstractDRTBuilder.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

using namespace XMLSP;
using namespace std;

/** Create DRT-Xml parser*/
DRTXmlParser::DRTXmlParser(){}

/** Release resources held by this parser */
DRTXmlParser::~DRTXmlParser(){}

/** Parse XML string to DRT-builder
 * @remarks Well formed XML is assumed, that is the order of tags actually matters!
 */
void DRTXmlParser::parse(const string& xml, AbstractDRTBuilder* builder) const {
	// Parse the Xml string
	DOMElement* root = DOMElement::loadXML(xml);

	parseElement(root);
}

void DRTXmlParser::parseElement(DOMElement* element, AbstractDRTBuilder* builder) const {
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

void DRTXmlParser::parseTask(DOMElement* element, AbstractDRTBuilder* builder) const {
	assert(element->hasAttribute("name"));
	if(!element->hasAttribute("name")){
		fprintf(stderr, "Attribute on missing in DRTXmlParser::parseTask\n");
		return;
	}

	AbstractDRTBuilder::TaskArgs args;
	args.name = element->getAttribute("name");

	builder->createTask(args);

	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	bool parsing_jobs = true;
	for(it = elements.begin(); it != elements.end(); it++){
		string name = (*it)->getElementName();
		if(name == "job"){
			assert(parsing_jobs);
			if(parsing_jobs)	//Don't parse jobs if not added in right order
				parseJob(*it, builder);
		}else if(name == "edge"){
			parsing_jobs = false;	//We can't go back to adding jobs after the first edge
			parseEdge(*it, builder);
		}else
			assert(false);
	}

	builder->taskCreated(args);
}

void DRTXmlParser::parseJob(DOMElement* element, AbstractDRTBuilder* builder) const {
	assert(element->hasAttribute("name") && element->hasAttribute("wcet") && element->hasAttribute("deadline"));
	if(!element->hasAttribute("name") || !element->hasAttribute("wcet") || !element->hasAttribute("deadline")){
		fprintf(stderr, "Attribute on missing in DRTXmlParser::parseJob\n");
		return;
	}

	AbstractDRTBuilder::JobArgs args;
	args.name = element->getAttribute("name");
	args.wcet = atoi(element->getAttribute("wcet").c_str());
	args.deadline = atoi(element->getAttribute("deadline").c_str());
	if(element->hasAttribute("x"))
		args.x = atof(element->getAttribute("x"));
	if(element->hasAttribute("y"))
		args.y = atof(element->getAttribute("y"));
	builder->addJob(args);
}

void DRTXmlParser::parseEdge(DOMElement* element, AbstractDRTBuilder* builder) const {
	assert(element->hasAttribute("src") && element->hasAttribute("dst") && element->hasAttribute("mtime"));
	if(!element->hasAttribute("src") || !element->hasAttribute("dst") || !element->hasAttribute("mtime")){
		fprintf(stderr, "Attribute on missing in DRTXmlParser::parseEdge\n");
		return;
	}

	AbstractDRTBuilder::EdgeArgs args;
	args.src = element->getAttribute("src");
	args.dst = element->getAttribute("dst");
	args.mtime = atoi(element->getAttribute("mtime").c_str());
	builder->addEdge(args);
}
