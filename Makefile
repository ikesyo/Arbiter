SOURCES=$(shell find src -name '*.cpp')
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libArbiter.a

GTEST_DIR=external/googletest/googletest

TEST_SOURCES=$(shell find test -name '*.cpp') $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/src/gtest_main.cc
TEST_RUNNER=test/main
TEST_INCLUDES=-isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -Isrc/

EXAMPLES=examples/library_folders/library_folders
EXAMPLE_LIBRARY_FOLDERS=$(shell find examples/library_folders -name '*.c')

CXX=clang++
CXXFLAGS=-std=c++14 -pedantic -Wall -Wextra -Iinclude/
CC=clang
CFLAGS=-std=c99 -lc++ -pedantic -Wall -Wextra -Wno-unused-parameter -Iinclude/
LIBTOOL=libtool

.PHONY: bindings/swift check docs

all: build

bindings: bindings/swift

bindings/swift:
	cd $@ && xcodebuild -scheme Arbiter

build: $(LIBRARY)

check: $(TEST_RUNNER) examples bindings
	$(TEST_RUNNER)

clean:
	rm -f $(EXAMPLES)
	rm -f $(LIBRARY) $(TEST_RUNNER)
	rm -f $(OBJECTS)

docs:
	doxygen Doxyfile

examples: $(EXAMPLES)

examples/library_folders/library_folders: $(LIBRARY) $(EXAMPLE_LIBRARY_FOLDERS)
	$(CC) $(CFLAGS) $(EXAMPLE_LIBRARY_FOLDERS) $(LIBRARY) -o $@

$(LIBRARY): $(OBJECTS)
	$(LIBTOOL) $(OBJECTS) -o $@

$(TEST_RUNNER): $(TEST_SOURCES) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) $(LIBRARY) -pthread $(TEST_INCLUDES) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
