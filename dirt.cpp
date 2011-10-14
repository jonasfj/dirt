#include <DRT/AbstractDRTBuilder.h>

#include <DRT/Matrix/MatrixTask.h>
#include <DRT/Matrix/MatrixDRTBuilder.h>

#include <DRT/Xml/XmlDRTBuilder.h>
#include <DRT/Xml/XmlDRTParser.h>

#include <DRT/Builders/DotDRTBuilder.h>
#include <DRT/Misc/RandomDRTGenerator.h>
#include <DRT/Misc/Interval.h>
#include <DRT/Misc/DBFPlotter.h>

#include <DRT/Builders/ValidatingDRTBuilder.h>
#include <DRT/Builders/TeeDRTBuilder.h>

#include <DRT/Verification/WangsUtilizationAlgorithm.h>
#include <DRT/Verification/Lawler.h>

#include <DRT/Verification/TaskDBF.h>

#include "config.h"

// Standard C Libraries
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Standard C++ Libraries
#include <fstream>
#include <iostream>
#include <sstream>

// GNU getopt From GLibc
#include <getopt.h>

using namespace std;
using namespace DRT;
using namespace DRT::Misc;
using namespace DRT::Xml;
using namespace DRT::Builders;
using namespace DRT::Matrix;
using namespace DRT::Verification;

namespace Options{
	/** Primary options for main() */
	enum PrimaryOptions{
		NoOption = 0,			///< No primary option given (invalid)
		Version = 1,			///< Show version number
		Help,					///< Show help message
		Generate,				///< Generate new DRT
		Validate,				///< Validate input, write results to output
		Dot,					///< Dump to dot
		Verify					///< Verify scheduability of the DRT
	};
	/** Long options, that can only be provided as */
	enum LongOptions{
		// Start enum at 255, as chars may be used to create short options
		TaskLimitsOption = 255,	///< Min/max number of task generated
		JobLimitsOption,		///< Min/max number of jobs generated
		EdgeLimitsOption,		///< Min/max number of edges generated
		DelayLimitsOption,		///< Min/max inter-release time of generated edges
		WcetLimitsOption,		///< Min/max worst case execution time of generated jobs
		SlackLimitsOption,		///< Min/max slack for deadline of generated jobs
		SeedOption,				///< Seed for PRNG
		NumberOfOptions			///< Number of long options
	};
}

/** Enumeration of return value from dirt/main() */
enum ReturnValues{
	SuccessCode = 0,			///< Operation success, input verified or validated
	FailureCode,				///< Operation failed, input is not scheduable or invalid
	ArgumentErrorCode,			///< Argument error
	InputErrorCode				///< Input parsing error
};

/** Main method of the application
 * The command line interface allows one primary option with a set of optional options.
 * Input and output files can be provided using options, if not provided standard
 * input or output is assumed respectively.
 *
 * Implements argument parsing according to
 * GNU Coding Standards: 4.7 Standards for Command Line Interfaces
 * See: http://www.gnu.org/prep/standards/standards.html
 */
int main(int argc, char* argv[]){
	// Options
	Options::PrimaryOptions option = Options::NoOption;
	ostream* output = &cout;
	istream* input = &cin;
	Interval<int> taskLimits, jobLimits, edgeLimits,	// Limits when generating DRTs
				  delayLimits, wcetLimits, slackLimits;
	int batchmode = 0;									// Batchmode, ignore no-fatal errors
	unsigned int seed = 0;								// Seed for PRNG, 0 of none
	int seed_time = 0;									// Seed with time too
	int plot = 0;										// Plot DBF when verifying

	// Long options
	static struct option options[] = {
		// Option Name		Argument Required		Flag pointer		Value
		// Primary options
		{"help",			no_argument,			(int*)&option,		Options::Help				},
		{"generate",		no_argument,			(int*)&option,		Options::Generate			},
		{"version",			no_argument,			(int*)&option,		Options::Version			},
		{"validate",		no_argument,			(int*)&option,		Options::Validate			},
		{"dot",				no_argument,			(int*)&option,		Options::Dot				},
		{"verify",			no_argument,			(int*)&option,		Options::Verify				},
		// I/O options
		{"input",			required_argument,		NULL,				'i'							},
		{"output",			required_argument,		NULL,				'o'							},
		// Meta options
		{"batch-mode",		no_argument,			&batchmode,			1							},
		{"plot",			no_argument,			&plot,				1							},
		// Generate options
		{"task-limits",		required_argument,		NULL,				Options::TaskLimitsOption	},
		{"job-limits",		required_argument,		NULL,				Options::JobLimitsOption	},
		{"edge-limits",		required_argument,		NULL,				Options::EdgeLimitsOption	},
		{"delay-limits",	required_argument,		NULL,				Options::DelayLimitsOption	},
		{"wcet-limits",		required_argument,		NULL,				Options::WcetLimitsOption	},
		{"slack-limits",	required_argument,		NULL,				Options::SlackLimitsOption	},
		{"seed",			required_argument,		NULL,				Options::SeedOption			},
		{"seed-time",		no_argument,			&seed_time,			1							},
		{0, 0, 0, 0}
	};

	// Short options (letter followed by : if it takes an argument)
	static char opts[] = "ghi:o:";

	// Parse options using GNU getopt_long
	// See: http://www.gnu.org/s/hello/manual/libc/Getopt.html
	int c = 0;
	while((c = getopt_long(argc, argv, opts, options, NULL)) != -1){
		switch(c){
			case 0: break;
			case 'h':
				option = Options::Help;
				break;
			case 'g':
				option = Options::Generate;
				break;
			case 'i':
				input = (istream*)new ifstream(optarg, ifstream::in);
				break;
			case 'o':
				output = (ostream*)new ofstream(optarg, ofstream::out);
				break;
			case Options::TaskLimitsOption:
				taskLimits = Interval<int>::parse(optarg);
				if(!taskLimits.valid()){
					fprintf(stderr, "Can't parse argument: \"%s\" given to --task-limits\n", optarg);
					if(!batchmode) return ArgumentErrorCode;
				}
				break;
			case Options::JobLimitsOption:
				jobLimits = Interval<int>::parse(optarg);
				if(!jobLimits.valid()){
					fprintf(stderr, "Can't parse argument: \"%s\" given to --job-limits\n", optarg);
					if(!batchmode) return ArgumentErrorCode;
				}
				break;
			case Options::EdgeLimitsOption:
				edgeLimits = Interval<int>::parse(optarg);
				if(!edgeLimits.valid()){
					fprintf(stderr, "Can't parse argument: \"%s\" given to --edge-limits\n", optarg);
					if(!batchmode) return ArgumentErrorCode;
				}
				break;
			case Options::DelayLimitsOption:
				delayLimits = Interval<int>::parse(optarg);
				if(!delayLimits.valid()){
					fprintf(stderr, "Can't parse argument: \"%s\" given to --delay-limits\n", optarg);
					if(!batchmode) return ArgumentErrorCode;
				}
				break;
			case Options::WcetLimitsOption:
				wcetLimits = Interval<int>::parse(optarg);
				if(!wcetLimits.valid()){
					fprintf(stderr, "Can't parse argument: \"%s\" given to --wcet-limits\n", optarg);
					if(!batchmode) return ArgumentErrorCode;
				}
				break;
			case Options::SlackLimitsOption:
				slackLimits = Interval<int>::parse(optarg);
				if(!slackLimits.valid()){
					fprintf(stderr, "Can't parse argument: \"%s\" given to --slack-limits\n", optarg);
					if(!batchmode) return ArgumentErrorCode;
				}
				break;
			case Options::SeedOption:
				if(sscanf(optarg, "%d", &seed) != 1){
					fprintf(stderr, "Can't parse argument: \"%s\" given to --seed\n", optarg);
					if(!batchmode) return ArgumentErrorCode;
					seed = 0;
				}
				break;
			default:
				option = Options::Help;
				break;
		}
	}

	// Validate arguments
	if(option == Options::NoOption){
		printf("Usage: %s [--generate] [--validate] [--help]\n", argv[0]);
		return ArgumentErrorCode;
	}

	// Show version information
	if(option == Options::Version){
		printf("dirt %s\n"
			   "Copyright (C) 2011 Jonas Finnemann Jensen <jopsen@gmail.com>.\n"
			   "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
			   "This is free software: you are free to change and redistribute it.\n"
			   "There is NO WARRANTY, to the extent permitted by law.\n",
				PROJECT_VERSION);
		return SuccessCode;
	}

	// Show help
	if(option == Options::Help){
		printf("Usage: %s --input [FILE] [--validate] [--help]\n", argv[0]);
		//TODO Print better help message
		return SuccessCode;
	}

	// Generate a random DRT
	if(option == Options::Generate){
		XmlDRTBuilder xb(*output);
		AbstractDRTBuilder* b = &xb;
		//Only validate in debug-mode
#ifndef NDEBUG
		ValidatingDRTBuilder vb;
		TeeDRTBuilder tee(&vb, &xb);
		b = &tee;
#endif /* NDEBUG */

		// Create generator
		RandomDRTGenerator rg;
		// Set limits if provided
		if(taskLimits.valid())	rg.setTaskLimits(taskLimits);
		if(jobLimits.valid())	rg.setJobLimits(jobLimits);
		if(edgeLimits.valid())	rg.setEdgeLimits(edgeLimits);
		if(delayLimits.valid())	rg.setDelayLimits(delayLimits);
		if(wcetLimits.valid())	rg.setWCETLimits(wcetLimits);
		if(slackLimits.valid())	rg.setSlackLimits(slackLimits);

		// If we're supposed to seed with time too
		if(seed_time)
			seed += time(NULL);

		// Generate DRT
		rg.generate(b, seed);
	}

	// Validate input
	if(option == Options::Validate){
		// Create validator
		ValidatingDRTBuilder validator(*output);
		// Parse input
		XmlDRTParser parser;
		if(!parser.parse(*input, &validator))
			return InputErrorCode;
		return validator.hasError() ? FailureCode : SuccessCode;
	}

	// Dump to dot
	if(option == Options::Dot){
		// Create dot builder
		DotDRTBuilder dotter(*output);
		// Parse input
		XmlDRTParser parser;
		if(!parser.parse(*input, &dotter))
			return InputErrorCode;
		return SuccessCode;
	}

	// Verify
	if(option == Options::Verify){
		//TODO: This only computes utilization, verification isn't complete yet!

		//Parse input to matrices
		MatrixDRTBuilder mb;
		if(!XmlDRTParser().parse(*input, &mb))
			return InputErrorCode;
		vector<MatrixTask*> drt = mb.produce();
		
		double U = WangsUtilizationAlgorithm::computeUtilization(drt);
		if(U >= 1) {
			*output << "Utilization of DRT: " << U << endl;
			*output << "DRT is NOT schedulable!" << endl;
			return FailureCode;
		}
			
		//double U = Lawler::computeUtilization(drt);
		int D = TaskDBF::DBFBound(drt, U);

		if(!plot){
			*output << "Utilization of DRT: " << U << endl;
			*output << "Upper bound of DBF: " << D << endl;
		}

		AbstractDBF* dbf = TaskDBF::DRTDemandBoundFunction(drt, D);
		bool scheduable = dbf->feasible();

		if(!plot){
			*output << "Utilization of DRT: " << U << endl;
			*output << "Upper bound of DBF: " << D << endl;
			if(scheduable)
				*output << "DRT is schedulable" << endl;
			else
				*output << "DRT is NOT schedulable!" << endl;
		}else{
			DBFPlotter plotter(*output, "DRT Demand Bound Function", D);
			plotter.addDBF(*dbf);
			plotter.finish();
		}

		return scheduable ? SuccessCode : FailureCode;
	}
}
