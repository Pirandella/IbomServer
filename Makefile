CC := gcc
TARGET := ihbls
CFLAGS = -std=c99 -I $(IDIR) -Wall -g -lm

ODIR := ./obj
SDIR := ./src
IDIR := ./inc

_DEPS := index.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := main.o index.o
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $^ $(CFLAGS)

.PHONY: clean mem profile

mem:
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes --num-callers=20 --track-fds=yes ./$(TARGET)

profile:
	valgrind --tool=callgrind --branch-sim=yes --collect-systime=usec --collect-jumps=yes ./$(TARGET)

clean:
	rm -f $(ODIR)/*.o $(TARGET)
