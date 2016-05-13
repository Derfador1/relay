CFLAGS+=-std=c11
CFLAGS+=-pthread -Wall -Wextra -Wpedantic
CFLAGS+=-Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline

all: listener dispatcher

listener: listener.o

dispatcher: dispatcher.o

profile:CFLAGS+=-pg
profile:LDFLAGS+=-pg
profile:all

.PHONY: clean debug profile

clean:
	rm listener dispatcher *.o

debug: CFLAGS+=-g
debug: listener dispatcher
