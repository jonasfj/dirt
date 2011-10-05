#include "DotDRTBuilder.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace DRT{
namespace Builders{

/** Create instance of DotDRTBuilder with results written to output  */
DotDRTBuilder::DotDRTBuilder(ostream& output) : out(output) {
	id = 1;		// Reserve 0 as invalid id
	out << "digraph DRT{ " << endl;
	out << "\tnode [fontsize=10];" << endl;
	out << "\tedge [fontsize=10];" << endl;
}

/** Finish the dot graph, e.g. write closing tags to output stream */
void DotDRTBuilder::finish(){
	out << "}" << endl;
}

/** Escape a string for input to dot */
string DotDRTBuilder::escape(const std::string& text) const{
	//TODO Implement some sort of text escaping...
	return text;
}

void DotDRTBuilder::createTask(const TaskArgs& args){
	jobs.clear();
	out << "\tsubgraph cluster_" << id++ << " {" << endl;
	out << "\t\tlabel=\"" << escape(args.name) << "\";" << endl;
	out << "\t\tstyle=filled;" << endl;
	out << "\t\tcolor=lightgrey;" << endl;
	out << "\t\tnode [style=filled,color=white];" << endl;
}

void DotDRTBuilder::addJob(const JobArgs& args){
	JobEntry je;
	je.id = id++;
	je.name = args.name;
	jobs.push_back(je);
	out << "\t\tjob" << je.id << " [label=\"" << escape(args.name) << " (" 
		<< args.wcet << "," << args.deadline << ")\"];" << endl;
}

/** Get job id for a job of the current task */
int DotDRTBuilder::getJobId(const string& name) const{
	for(vector<JobEntry>::const_iterator it = jobs.begin(); it != jobs.end(); it++){
		if(it->name == name)
			return it->id;
	}
	assert(false);
	return 0;	//We reserve 0 as invalid id
}

void DotDRTBuilder::addEdge(const EdgeArgs& args){
	out << "\t\tjob" << getJobId(args.src) << " -> job" << getJobId(args.dst)
	   	<< " [label=\"" << args.mtime << "\"];" << endl;
}

void DotDRTBuilder::taskCreated(const TaskArgs& args){
	jobs.clear();
	out << "\t}" << endl;
}

} /* Builders */
} /* DRT */

