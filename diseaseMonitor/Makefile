CC = gcc -g

diseaseMonitor: main.o patientRecord.o RBTree.o list.o bucket.o topk.o helpFunc.o mainFunc.o
	$(CC) -o diseaseMonitor main.o patientRecord.o RBTree.o list.o bucket.o topk.o helpFunc.o mainFunc.o

main.o: main.c
	$(CC) -c main.c

patientRecord.o: patientRecord.c
	$(CC) -c patientRecord.c

RBTree.o: RBTree.c
	$(CC) -c RBTree.c

listo.o: list.c
	$(CC) -c list.c

bucket.o: bucket.c
	$(CC) -c bucket.c

topk.o: topk.c
	$(CC) -c topk.c

helpFunc.o: helpFunc.c
	$(CC) -c helpFunc.c

mainFunc.o: mainFunc.c
	$(CC) -c mainFunc.c

.PHONY: clean

clean:
	rm -f diseaseMonitor main.o patientRecord.o RBTree.o list.o bucket.o topk.o helpFunc.o mainFunc.o
	clear
