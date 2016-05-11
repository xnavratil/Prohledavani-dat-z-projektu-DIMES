PROGRAM=graph-traverse
SOURCES=main.c graph.c heap.c

CC=gcc
CFLAGS=-Wall -g -pedantic -std=c99
OBJS=$(SOURCES:.c=.o)
DEPS=$(SOURCES:.c=.dep)

$(PROGRAM) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.dep : %.c
	@$(CC) $(CFLAGS) -MM -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY : clean doc

clean :
	rm -f *.o *.dep
	rm -rf html
	rm -f $(PROGRAM)

doc : clean
	doxygen Doxyfile
