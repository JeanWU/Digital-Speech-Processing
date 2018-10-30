.PHONY: all clean
CC=g++
CFLAGS+=-std=c++11
LDFLAGS+=-lm

TARGET= ./train

all: $(TARGET)

train:

	$(CC) $(CFLAGS) train.cpp -o train
	$(CC) $(CFLAGS) test.cpp -o test
	$(CC) $(CFLAGS) accuracy.cpp -o accuracy
	./train 800 model_init.txt seq_model_01.txt model_01.txt
	./train 800 model_init.txt seq_model_02.txt model_02.txt
	./train 800 model_init.txt seq_model_03.txt model_03.txt
	./train 800 model_init.txt seq_model_04.txt model_04.txt
	./train 800 model_init.txt seq_model_05.txt model_05.txt
	./test modellist.txt testing_data1.txt result1.txt
	./test modellist.txt testing_data2.txt result2.txt
	./accuracy

clean:

	rm ./train
	rm ./test
	rm ./accuracy

