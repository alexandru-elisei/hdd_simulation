# Elisei Alexandru, 316CC
# Structuri de Date
# Tema 1

CC = "gcc"
override CFLAGS += "-Wall"
PROG = "myHDD"

HEADERS = hdd.h						\
	  common.h

SOURCES = main.c					\
	  $(HEADERS:%.h=%.c)

OBJS = $(SOURCES:%.c=%.o)

.PHONY: build

build: $(PROG) $(HEADERS)

$(PROG): $(OBJS)
	$(CC) $(OBJS) -o $(PROG) $(CFLAGS)

%.o: %.c
	$(CC) -c $^ -o $@ $(CFLAGS)


.PHONY: clean

clean:
	rm -rf a.out $(PROG) $(OBJS)

