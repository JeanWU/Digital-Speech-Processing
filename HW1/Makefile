.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

TARGET=train

all: $(TARGET)
# type make/make all to compile test_hmm
	g++ -std=c++11 train.cpp -o train

run:
	./train 5 model_init.txt seq_model_01.txt model_01.txt

clean:
	rm ./train
	$(RM) $(TARGET)   # type make clean to remove the compiled file
