CC     = gcc
ifeq ($(debug), y)
	CFLAGS += -std=gnu99 -Werror -DDEBUG -g
endif
ifeq ($(win32), y)
	CFLAGS += -std=gnu99 -Werror -lws2_32
endif
ifeq ($(test), y)
	CFLAGS += -std=gnu99 -Werror -DTEST
else
	CFLAGS += -std=gnu99 -Werror
endif
TARGET = dogcom
INSTALL_DIR = /usr/bin/

SOURCES = $(wildcard *.c) $(wildcard libs/*.c)
OBJS    = $(patsubst %.c, %.o, $(SOURCES))

$(TARGET):	$(OBJS)
	$(CC) $(DEBUG) $(TEST) $(OBJS) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

install:	$(TARGET)
	cp $(TARGET) $(INSTALL_DIR)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

distclean:  clean

.PHONY: all clean distclean install