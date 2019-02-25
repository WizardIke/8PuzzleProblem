Main.exe	: Main.o graphics.o puzzle.o algorithm.o
	g++ -O2 -std=c++11 -o Main.exe Main.o graphics.o puzzle.o algorithm.o -l gdi32 -static-libgcc -static-libstdc++
			
Main.o	: Main.cpp graphics.h puzzle.h algorithm.h 
	g++ -O2 -std=c++11 -c -Wno-write-strings Main.cpp
	
puzzle.o : puzzle.cpp puzzle.h
	g++ -O2 -std=c++11 -c -Wno-write-strings  puzzle.cpp
	
algorithm.o	: algorithm.cpp algorithm.h Deque.h HashMap.h PriorityQueue.h Vector.h StackSlabAllocator.h ArrayMap.h
	g++ -O2 -std=c++11 -c -Wno-write-strings algorithm.cpp

graphics.o  : graphics.cpp graphics.h
	g++ -O2 -std=c++11 -c -Wno-write-strings graphics.cpp
	
clean:
	del *.o
	del *.exe
	

