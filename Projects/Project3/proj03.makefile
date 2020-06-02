# Make Example
#
# Control translation using the "make" utility program

run: proj03.student.c
	g++ proj03.student.c -o run

#proj03.student.o: proj03.student.Control
#    g++ -Wall -c proj03.student.c
