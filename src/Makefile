ALL_PLUGINS = \
    libscorep_substrate_mtrace.so

all: $(ALL_PLUGINS)

libscorep_substrate_mtrace.so.o: scorep_substrate_mtrace.c
	gcc -std=c99 -g -c -fPIC scorep_substrate_mtrace.c   \
            -o libscorep_substrate_mtrace.so.o  \
            `scorep-config --cppflags`

libscorep_substrate_mtrace.so: libscorep_substrate_mtrace.so.o
	gcc -std=c99 -g -shared -Wl,-soname,libscorep_substrate_mtrace.so  \
            -o libscorep_substrate_mtrace.so libscorep_substrate_mtrace.so.o
            
clean:
	$(RM) libscorep_substrate_mtrace.so *.o
