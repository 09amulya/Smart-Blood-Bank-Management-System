CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude

LDFLAGS = -pthread

TARGET = raktsetu


SRC = src/shell.c \
      src/process.c \
      src/states.c \
      src/scheduler.c \
      src/blood.c \
      src/logger.c \
      src/sync/race_mutex.c \
      src/sync/peterson.c \
      src/sync/bakery.c \
      src/sync/test_and_set.c


OBJ = $(SRC:.c=.o)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJ) $(TARGET)