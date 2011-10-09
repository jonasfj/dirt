# Add source for drt library
set(drt_SOURCES
		Builders/DotDRTBuilder.cpp
		Builders/TeeDRTBuilder.cpp
		Builders/ValidatingDRTBuilder.cpp
		Matrix/MatrixDRTBuilder.cpp
		Matrix/MatrixTask.cpp
		Misc/RandomDRTGenerator.cpp
		Verification/WangsDBFAlgorithm.cpp
		Verification/WangsUtilizationAlgorithm.cpp
		Verification/Lawler.cpp
		Verification/MBF.cpp
		Xml/XmlDRTBuilder.cpp
		Xml/XmlDRTParser.cpp
		Verification/CompositeDBF.cpp
		Verification/DiscreteDBF.cpp
		Misc/DBFPlotter.cpp
	)
# Add library target, make it a static library
add_library(drt STATIC ${drt_SOURCES})
