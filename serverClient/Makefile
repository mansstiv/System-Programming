CC = gcc -g -pthread

all:executable1 executable2 executable3 executable4

executable1: worker.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o
	$(CC) -o worker worker.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o

worker.o: worker.c
	$(CC) -c worker.c

RBTree.o: RBTree.c
	$(CC) -c RBTree.c

bucket.o: bucket.c
	$(CC) -c bucket.c

topk.o: topk.c
	$(CC) -c topk.c

masterFunc.o: masterFunc.c
	$(CC) -c masterFunc.c	

helpFunc.o: helpFunc.c
	$(CC) -c helpFunc.c

patientRecord.o: patientRecord.c
	$(CC) -c patientRecord.c	

workerFunc.o: workerFunc.c
	$(CC) -c workerFunc.c	

workerStructs.o: workerStructs.c
	$(CC) -c workerStructs.c	

executable2: master.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o
	$(CC) -o master master.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o

master.o: master.c
	$(CC) -c master.c

executable3: whoClient.o whoClientFunc.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o
	$(CC) -o whoClient whoClient.o whoClientFunc.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o

whoClient.o: whoClient.c
	$(CC) -c whoClient.c

whoClientFunc.o: whoClientFunc.c
	$(CC) -c whoClientFunc.c	

executable4: whoServer.o whoServerFunc.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o
	$(CC) -o whoServer whoServer.o whoServerFunc.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o

whoServer.o: whoServer.c
	$(CC) -c whoServer.c

whoServerFunc.o: whoServerFunc.c
	$(CC) -c whoServerFunc.c		 
				
.PHONY: clean

clean:
	rm -f worker master whoClient whoServer whoServer.o whoServerFunc.o whoClient.o whoClientFunc.o master.o worker.o patientRecord.o RBTree.o bucket.o topk.o helpFunc.o masterFunc.o workerFunc.o workerStructs.o
	rm worker-*
	rm log_file.*
	clear



