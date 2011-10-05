set(drt_SOURCES
		Builders/DotDRTBuilder.cpp
		Builders/TeeDRTBuilder.cpp
		Builders/ValidatingDRTBuilder.cpp
		Matrix/MatrixDRTBuilder.cpp
		Matrix/MatrixTask.cpp
		Misc/RandomDRTGenerator.cpp
		Verification/WangsDBFAlgorithm.cpp
		Verification/WangsUtilizationAlgorithm.cpp
		Xml/XmlDRTBuilder.cpp
		Xml/XmlDRTParser.cpp)
add_library(drt STATIC ${drt_SOURCES})
