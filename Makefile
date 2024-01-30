CC := g++
CFLAGS := -Wall -g
TARGET := test

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))



all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^  -lgpiod -lpthread
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<
clean:
	rm -rf $(TARGET) *.o


