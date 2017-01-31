test_elf:
	g++ -std=c++11 -o test_elf src/test.cpp
	
all: test_elf

test: all
	./test_elf $(TEST_FILE)	
	
clean:
	rm test_elf	
	
