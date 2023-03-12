SRCS:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, %.o, $(SRCS))
CC:=g++
LIBS:=-lpthread
SERVER:=server


$(SERVER):$(OBJECTS)
	$(CC) $^ -o $@  $(LIBS) -g

%.o:%.c
	$(CC) -c $^ -o $@ $(LIBS) -g
print:
	echo $(OBJECTS)
clean:
	rm -rf $(OBJECTS) $(SERVER)
