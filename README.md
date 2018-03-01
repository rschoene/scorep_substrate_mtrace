# scorep_substrate_mtrace
Enables mtrace to detect memory leaks

# Compile and Install

Just run `make` within the `src` directory and copy the plugin (`libscorep_substrate_mtrace.so` to a path in `$LD_LIBRARY_PATH`).

# Usage
- Make sure `libscorep_substrate_mtrace.so` is accessible in the `$LD_LIBRARY_PATH`
- register the substrate `export SCOREP_SUBSTRATE_PLUGINS=mtrace`
- `export SCOREP_SUBSTRATE_MTRACE_FILE=(file-prefix)` will create files named `(file-prefix).(mpp_rank)`. For example if it is set to `/tmp/mem_test` and you run a test with 4 MPI ranks, there will be 4 files: `/tmp/mem_test.[0-3]`
- if `$SCOREP_SUBSTRATE_MTRACE_FILE` is not set, an error will be printed and `$MALLOC_TRACE` will be used
- the resulting files can be viewed via `mtrace` or `scorep_substrate_mtrace_parse.py`. The latter will first print the lines where sth. was allocated and later an overview of memory leaks

# Beta
If you want to start collecting data earlier, set the environment variable `SCOREP_SUBSTRATE_MTRACE_EARLY` to anything

#Problems
The lines reported by `scorep_substrate_mtrace_parse.py` can have an offset of 1 or 2. Probably due to an error in `addr2line`