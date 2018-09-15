CXX=clang++
CFLAGS=-std=c++1z -pedantic -W -Wall -Wcast-qual -Wconversion -Werror -Wextra -Wwrite-strings
IFLAGS=
LFLAGS=
OFLAGS=

SOURCES=test/test.cpp
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
DEPENDENCIES=$(OBJECTS:.o=.d)

# COMMANDS
build: test/test

benchmark: test/test
	./test/test | pv --average-rate > /dev/null

test: test/test
	./test/test

clean:
	$(RM) $(DEPENDENCIES) $(OBJECTS) test/test

# INFERENCES
%.o: %.cpp
	$(CXX) $(CFLAGS) $(OFLAGS) $(IFLAGS) -MMD -MP -c $< -o $@

test/test: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LFLAGS) $(OFLAGS) -o $@ -pthread

-include $(DEPENDENCIES)
