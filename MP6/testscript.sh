# Deeb Armaly
# MP6 Test Script
# ECE 2230 Fall 2021


#test unit drivers
	./lab6 -u 1
	./lab6 -u 2
#test retrieve driver for test plan
	./lab6 -r -m 6 -h double
	./lab6 -r -m 11 -h quad
#test retrieve drivers for performance eval
	./lab6 -r -m 65537 -h linear
	./lab6 -r -m 65537 -h double
	./lab6 -r -m 65536 -h quad
	./lab6 -r -a 0.85 -m 65537 -h linear -i rand
	./lab6 -r -a 0.85 -m 65537 -h linear -i seq
	./lab6 -r -a 0.85 -m 65537 -h linear -i fold
	./lab6 -r -a 0.85 -m 65537 -h linear -i worst
	./lab6 -r -a 0.85 -m 65537 -h double -i rand
	./lab6 -r -a 0.85 -m 65537 -h double -i seq
	./lab6 -r -a 0.85 -m 65537 -h double -i fold
	./lab6 -r -a 0.85 -m 65537 -h double -i worst
	./lab6 -r -a 0.85 -m 65536 -h quad -i rand
	./lab6 -r -a 0.85 -m 65536 -h quad -i seq
	./lab6 -r -a 0.85 -m 65536 -h quad -i fold
	./lab6 -r -a 0.85 -m 65536 -h quad -i worst
#test equilibrium driver
	./lab6 -e -m 65537 -t 50000
	./lab6 -e -m 65537 -t 100000
#test rehash driver
	./lab6 -b -h linear
	./lab6 -b -h double
	./lab6 -b -h quad
#test with valgrind to show no memory leaks
	valgrind ./lab6 -b
	valgrind ./lab6 -e
	valgrind ./lab6 -r

