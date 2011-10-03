#include "AbstractDRTBuilder.h"

#include "MatrixTask.h"
#include "MatrixDRTBuilder.h"

#include "XmlDRTBuilder.h"
#include "XmlDRTParser.h"

#include "DotDRTBuilder.h"
#include "RandomDRTGenerator.h"

#include "ValidatingDRTBuilder.h"
#include "TeeDRTBuilder.h"

// Standard C Libraries
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Standard C++ Libraries
#include <fstream>
#include <iostream>
#include <sstream>

// GNU getopt From GLibc
#include <getopt.h>

namespace Options{
	/** Primary options for main() */
	enum PrimaryOptions{
		NoOption = 0,			///< No primary option given (invalid)
		Version = 1,			///< Show version number
		Help,					///< Show help message
		Generate,				///< Generate new DRT
		Validate				///< Validate input, write results to output
	};
}

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
	std::ostream* output = &std::cout;
	std::istream* input = &std::cin;

	// Long options
	static struct option options[] = {
		// Option Name		Argument Required		Flag pointer		Value
		// Primary options
		{"help",			no_argument,			(int*)&option,		Options::Help		},
		{"generate",		no_argument,			(int*)&option,		Options::Generate	},
		{"version",			no_argument,			(int*)&option,		Options::Version	},
		{"validate",		no_argument,			(int*)&option,		Options::Validate	},
		// Optional options
		{"input",			required_argument,		NULL,				'i'					},
		{"output",			required_argument,		NULL,				'o'					 },
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
				input = (std::istream*)new std::ifstream(optarg, std::ifstream::in);
				break;
			case 'o':
				output = (std::ostream*)new std::ofstream(optarg, std::ofstream::out);
				break;
			default:
				option = Options::Help;
				break;
		}
	}

	// Validate arguments
	if(option == Options::NoOption){
		printf("Usage: %s [--generate] [--validate] [--help]\n", argv[0]);
		return 1;
	}

	// Show version information
	if(option == Options::Version){
		printf("Realtiming 0.1\n"
			   "Copyright (C) 2011 Jonas Finnemann Jensen <jopsen@gmail.com>.\n"
			   "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
			   "This is free software: you are free to change and redistribute it.\n"
			   "There is NO WARRANTY, to the extent permitted by law.\n");
		return 0;
	}

	// Show help
	if(option == Options::Help){
		printf("Usage: %s --input [FILE] [--validate] [--help]\n", argv[0]);
		//TODO Print better help message
		return 0;
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

		RandomDRTGenerator rg;
		//rg.setJobLimits(1, 5);		//TODO: Allows this to be set using options
		//rg.setTaskLimits(1,3);
		//rg.setEdgeLimits(1,6);
		rg.generate(b);
		
		xb.finish();
	}

	// Validate input
	if(option == Options::Validate){
		// Read file to string
		std::stringstream buffer;
		buffer << input->rdbuf();
		// Create validator
		ValidatingDRTBuilder validator(*output);
		// Parse input
		XmlDRTParser parser;
		parser.parse(buffer.str(), &validator);
	}
}
