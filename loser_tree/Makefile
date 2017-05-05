SRC := $(wildcard *.c)
OBJECTS := $(SRC:%.c=%.o) 
TARGET=libjnimerge.so

INCLUDE =	-I./

LDFLAG =	-L./

CFLAGS = -g -Wall -shared -O2 # -DDEBUG 

CC = gcc

TARGET_OS := $(shell ($(CC) -dumpmachine | awk -F '-' '{print $$3}'))
JAVA_HOME := $(shell (env | grep JAVA_HOME | awk -F '=' '{print $$2}' | sed 's/ /\\ /g'))
ifeq	($(TARGET_OS), mingw32)
JAVA_INCLUDE =	-I$(JAVA_HOME)\include \
		-I$(JAVA_HOME)\include\win32
TARGET=libjnimerge.dll
else
JAVA_INCLUDE = -I$(JAVA_HOME)/include/ \
		-I$(JAVA_HOME)/include/linux
CFLAGS += -fPIC
endif
INCLUDE += $(JAVA_INCLUDE)

all:$(TARGET)

$(TARGET):$(OBJECTS)
	$(CC) $(INCLUDE) $(LDFLAG) $(CFLAGS) -o $@ $^ 

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJECTS)
