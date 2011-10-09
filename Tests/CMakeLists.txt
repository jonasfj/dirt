# Test files to run
# A test is a "int main()" function that returns 0 if not failed
set(test_FILES
		DiscreteDBFInsertTest.cpp
		DiscreteDBFStepTest.cpp
		WangUtilizationTest.cpp
		ParallelDBFTest.cpp
		SequentialDBFTest.cpp
	)

# Enable testing
ENABLE_TESTING()

# Add check target
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

# Foreach test file
foreach(file ${test_FILES}) 
	# Get the name of the test
	get_filename_component(test ${file} NAME_WE)

	# Add executable, all tests are single files
	add_executable(${test} EXCLUDE_FROM_ALL ${file})

	#Set libraries
	target_link_libraries(${test} drt xmlsp)

	# Add binary as test
	add_test(${test} ${test})

	# Set binary to build along with check target
	add_dependencies(check ${test})
endforeach(file)

