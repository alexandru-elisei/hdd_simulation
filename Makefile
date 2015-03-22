# Elisei Alexandru, 316CC
# Structuri de Date
# Tema 1

CC = "gcc"
override CFLAGS += "-Wextra"
PROG = "myHDD"

HEADERS = hdd.h						\
	  common.h					\
	  queue.h

SOURCES = main.c					\
	  $(HEADERS:%.h=%.c)

OBJS = $(SOURCES:%.c=%.o)

.PHONY: build

build: $(PROG) $(HEADERS)

$(PROG): $(OBJS) $(HEADERS)
	$(CC) $(OBJS) -o $(PROG) $(CFLAGS)

%.o: %.c
	$(CC) -c $^ -o $@ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf a.out $(PROG) $(OBJS)

