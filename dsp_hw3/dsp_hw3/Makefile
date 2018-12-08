SRIPATH  ?= /home/jeanwu/jean/DSP/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include

vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map

test1_dir = ./result1
test2_dir = ./result2

.PHONY: all clean map test2

all:$(TARGET)
$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

run:
	[ -d $(test2_dir) ] || mkdir -p $(test2_dir);
	@for i in $(shell seq 1 10) ; do \
            echo "Running $$i.txt"; \
            ./mydisambig -text ./testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > $(test2_dir)/$$i.txt ;  \
        done;
test1:
	[ -d $(test1_dir) ] || mkdir -p $(test1_dir);
	@for i in $(shell seq 1 10) ; do \
						echo "Running $$i.txt"; \
						./disambig -keep-unk -text ./testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > $(test1_dir)/$$i.txt ;  \
				done;

map:
	python3 mapping.py $(FROM) $(TO)

clean:
	$(RM) $(OBJ) $(TARGET)
	$(RM) -f *~
