#include "XmlDRTBuilder.h"

using namespace std;

/** Create XmlDRTBuilder where XML stream is written to output stream */
XmlDRTBuilder::XmlDRTBuilder(ostream& output) : out(output) {
	out << "<drt>" << endl;
}

/** XML escape string */
string XmlDRTBuilder::escape(const string& text) const{
	return text;	//TODO Actually do an escape of the string...
}

void XmlDRTBuilder::createTask(const AbstractDRTBuilder::TaskArgs& args){
	out << "\t<task name=\"" << escape(args.name) << "\">" << endl;
}

void XmlDRTBuilder::addJob(const AbstractDRTBuilder::JobArgs& args){
	out << "\t\t<job"
		<< " name=\"" << escape(args.name) << "\" "
		<< " wcet=\"" << args.wcet << "\""
		<< " deadline=\"" << args.deadline << "\"\\>" << endl;
}

void XmlDRTBuilder::addEdge(const AbstractDRTBuilder::EdgeArgs& args){
	out << "\t\t<edge"
		<< " source=\"" << escape(args.src) << "\""
		<< " destination=\"" << escape(args.dst) << "\""
		<< " delay=\"" << args.mtime <<  "\"\\>" << endl;
}

void XmlDRTBuilder::taskCreated(const AbstractDRTBuilder::TaskArgs& args){
	out << "\t</task>" << endl;
}

/** Finish the XML stream */
void XmlDRTBuilder::finish(){
	out << "</drt>" << endl;
}

