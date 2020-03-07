1CC=gcc
CFLAGS=-m32 -Wall -no-pie
SRC=src
_DEPS=dbhelper.h diamonds.h registration.h screens.h errors.h exit.h help.h util.h
DEPS = $(patsubst %,$(SRC)/%,$(_DEPS))
_OBJ=dbhelper.o diamonds.o registration.o screens.o exit.o help.o util.o
OBJ = $(patsubst %,$(SRC)/%,$(_OBJ))

all: clean

$(SRC)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

service: $(OBJ)
	$(CC) $(CFLAGS) -o service/ro/diamonds $^ $(SRC)/main.c

bundle:
	tar czvf ../lucy_in_the_sky_with_diamonds.tgz service

scriptbot_scripts:
	docker build -t $(SERVICE_NAME)_scripts ./scripts

.PHONY: clean
clean:
	echo "NOP!"

#clean:
#	rm -f $(SRC)/*.o 	rm -f $(SRC)/*.o service/ro/* service/rw/diamonds.dat service/rw/user.dat

