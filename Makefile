# Makefile for mincalc

SRCS := \
	io.c \
	strutils.c \
	lexer.c \
	parser.c \
	calc.c \
	main.c

OBJS := $(SRCS:.c=.o)

TARGET := mincalc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $+ $(LDFLAGS) $(LIBS)

clean:
	-rm $(TARGET) $(OBJS)

.PHONY: all clean
