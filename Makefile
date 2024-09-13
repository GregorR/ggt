CC=cc
CFLAGS=-O3
AR=ar
RANLIB=ranlib
LIB=wlib

OBJS=\
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

libggt.a: $(OBJS)
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@

lib: $(OBJS)
	rm -f ggt.lib
	$(LIB) -n ggt.lib $(addprefix +,$(OBJS))

zcclib: $(OBJS)
	$(CC) -x -o ggt.lib $(OBJS)

obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -I. -DGGT_COMP_THREADS=0 -c $< -o $@

obj/%-thr.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -I. -DGGT_COMP_THREADS=1 -DGGT_SUPP_THREADS=1 -c $< -o $@

clean:
	rm -f libggt.a ggt.lib
	rm -rf obj
