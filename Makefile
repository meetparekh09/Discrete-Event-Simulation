sched: main.cpp random.cpp process.cpp scheduler.h des.cpp simulation.cpp
	g++ -o sched main.cpp random.cpp process.cpp des.cpp simulation.cpp

clean:
	rm sched