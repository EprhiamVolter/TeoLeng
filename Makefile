CXX = g++
CXXFLAGS = -std=c++20 -fmax-errors=1 -Ih
SRC = $(shell find cpp -name "*.cpp")
OBJ = $(SRC:cpp/%.cpp=o/%.o)
DEP = $(OBJ:%.o=%.d)

run: main.exe
	@./main.exe
	
main : main.exe

main.exe: $(OBJ)
	@echo compiling main
	@$(CXX) $(CXXFLAGS) -o $@ $^

o/%.o: cpp/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<
	
-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) main main.exe