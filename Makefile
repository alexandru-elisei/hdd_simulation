# Elisei Alexandru, 316CC
# Structuri de Date
# Tema 1

CC = "gcc"
FLAGS = "-Wall"
PROG = "myHDD"

HEADERS = hdd.h						\
	  common.h

SOURCES = main.c					\
	  $(HEADERS:%.h=%.c)

OBJS = $(SOURCES:%.c=%.o)

.PHONY: build

build: $(PROG)

$(PROG): $(OBJS) $(HEADERS)
	$(CC) $(OBJS) -o $(PROG) $(FLAGS)

%.o: %.c
	$(CC) -c $^ -o $@ $(FLAGS)


.PHONY: clean

clean:
	rm -rf a.out $(PROG) $(OBJS)

