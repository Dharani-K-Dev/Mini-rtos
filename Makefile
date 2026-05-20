CC      = gcc
CFLAGS  = -Wall -Wextra -I include
SRC     = src/mini_rtos.c src/main.c
TARGET  = mini_rtos

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
