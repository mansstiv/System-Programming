CC = gcc -g

all:executable1 executable2

executable1: worker.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o parentFunc.o workerFunc.o workerStructs.o
	$(CC) -o worker worker.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o parentFunc.o workerFunc.o workerStructs.o

worker.o: worker.c
	$(CC) -c worker.c

RBTree.o: RBTree.c
	$(CC) -c RBTree.c

bucket.o: bucket.c
	$(CC) -c bucket.c

topk.o: topk.c
	$(CC) -c topk.c

parentFunc.o: parentFunc.c
	$(CC) -c parentFunc.c	

helpFunc.o: helpFunc.c
	$(CC) -c helpFunc.c

patientRecord.o: patientRecord.c
	$(CC) -c patientRecord.c	

workerFunc.o: workerFunc.c
	$(CC) -c workerFunc.c	

workerStructs.o: workerStructs.c
	$(CC) -c workerStructs.c	

executable2: parentProcess.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o parentFunc.o workerFunc.o workerStructs.o
	$(CC) -o diseaseAggregator parentProcess.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o parentFunc.o workerFunc.o workerStructs.o

parentProcess.o: parentProcess.c
	$(CC) -c parentProcess.c
				
.PHONY: clean

clean:
	rm -f worker diseaseAggregator parentProcess.o worker.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o parentFunc.o workerFunc.o workerStructs.o
	rm worker-*
	rm log_file.*
	clear



