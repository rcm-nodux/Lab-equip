all: lab

lab: main.o lab_equipment.o
	g++ main.o lab_equipment.o

main.o: main.cpp
	g++ -c main.cpp

lab_equipment.o: lab_equipment.cpp
	g++ -c lab_equipment.cpp
	
clean:
	rm *.o