CC=gcc
CFLAGS=-O3
AR=ar
RANLIB=ranlib
LIB=wlib

OBJ=\
    obj/green-init.o \
    obj/green-init-thr.o \
    obj/green-free-thr.o \
    obj/green-sleep.o \
    obj/green-sleep-thr.o \
    obj/green-wake.o \
    obj/green-wake-thr.o \
    obj/teal-init.o \
    obj/teal-init-thr.o \
    obj/teal-free-thr.o \
    obj/teal-sleep.o \
    obj/teal-sleep-thr.o \
    obj/teal-wake.o \
    obj/teal-wake-thr.o \
    obj/native-sleep.o \
    obj/native-wake.o

all: libggt.a

libggt.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RANLIB) $@

ggt.lib: $(OBJ)
	rm -f $@
	$(LIB) -n $@ $(addprefix +,$(OBJ))

obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -I. -c $< -o $@

obj/%-thr.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -I. -DGGT_SUPP_THREADS=1 -c $< -o $@

clean:
	rm -f libggt.a ggt.lib
	rm -rf obj
