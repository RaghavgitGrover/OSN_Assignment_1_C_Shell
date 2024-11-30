CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -D_POSIX_C_SOURCE=200809L 
TARGET = shell
SRCS = main.c prompt.c input.c command.c hop.c log.c reveal.c proclore.c seek.c bg.c iMan.c alias.c activities.c ping.c fg.c redirect.c pipe.c pipdirect.c neonate.c
OBJS = $(SRCS:.c=.o)
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(TARGET) $(OBJS)
run: $(TARGET)
	./$(TARGET)
.PHONY: all clean run
