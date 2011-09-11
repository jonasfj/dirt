#Use g++ unless other compiler is specified
CC = g++

#Compiler flags
CFLAGS = -Wall `pkg-config --cflags unittest++`
OPT_FLAGS = -DNDEBUG=true -O3 -mtune=native -march=native -funroll-loops -frerun-loop-opt
DEBUG_FLAGS = -g

#Linker flags (only for test)
LDFLAGS = `pkg-config --libs unittest++`

#Sources for bnc, excluding tests
SOURCES = $(shell find ./ -name "*.cpp")

#Headers for documentation
HEADERS = $(shell find ./ -name "*.h")

#Default target
all: drt

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),help)
ifneq ($(MAKECMDGOALS),bnc)
ifneq ($(MAKECMDGOALS),release)
ifneq ($(MAKECMDGOALS),debug)
-include ${addprefix deps/, $(TESTS:.cpp=.d)}
endif
endif
endif
endif
endif

#Rule for making a release
release: CFLAGS += $(OPT_FLAGS)
release: drt

#Rule for building with symbols
debug: CFLAGS += $(DEBUG_FLAGS)
debug: drt

#Rule for building an object
obj/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CC) -c $(CFLAGS) -o $@ $<

#Rule for building bnc
drt:
	@echo "Building drt..."
	@$(CC) $(CFLAGS) $(SOURCES) -o $@
	@chmod +x $@
	@echo "Build of drt completed"
	@which notify-send > /dev/null; \
	if test "$$?" = 0; then \
		notify-send "make drt" "drt build process completed..." -i info; \
	fi

#Build documentation
docs: $(SOURCES) $(HEADERS) $(TESTS) doxygen.conf
	@echo "Running doxygen"
	@mkdir -p docs
	@doxygen doxygen.conf > /dev/null
	@echo "Documentation compiled."
	@touch docs

#Rule to cleanup
clean:
	@if test -f drt; \
		then rm drt; \
		echo " - removed drt"; \
	fi
	@if test -d obj; \
		then rm -R obj; \
		echo " - removed obj/"; \
	fi
	@if test -d deps; \
		then rm -R deps; \
		echo " - removed deps/"; \
	fi
	@if test -d docs; \
		then rm -R docs; \
		echo " - removed docs/"; \
	fi

#Help information
help:
	@echo "---------------------------------------------------------------------"
	@echo "\t\tHelp for drt makefile"
	@echo "---------------------------------------------------------------------"
	@echo "make\n\tMake everything up to date.\n"
	@echo "make drt\n\tMake drt, e.g. the binary.\n"
	@echo "make debug\n\tMake the binary with debug symbols.\n"
	@echo "make release\n\tMake the binary with optimizations and without assertions.\n"
	@echo "make docs\n\tMake doxygen API documentation.\n"
	@echo "make deps\n\tMake dependency computations, stored in deps.\n"
	@echo "make clean\n\tCleanup after build, e.g. delete everything but the sources.\n"

#Rule for bulding .d files
deps/%.d: %.cpp
	@mkdir -p $(@D)
	@echo -n "$@ $(patsubst deps%,obj%,$(@D))/" > $@
	@$(CC) -MM $(CFLAGS) $< >> $@; \
	if test "$$?" != "0"; then \
		rm $@; \
		echo "Dependency computation failed for $<"; \
	fi

deps: ${addprefix deps/, $(TESTS:.cpp=.d)}
	@touch deps

.PHONY: all help clean check drt release debug

