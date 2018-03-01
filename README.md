# scorep_substrate_mtrace
Enables mtrace to detect memory leaks

# Compile and Install

Just run `make` within the `src` directory and copy the plugin (`libscorep_substrate_mtrace.so` to a path in `$LD_LIBRARY_PATH`)

# Usage
- Make sure `libscorep_substrate_mtrace.so` is accessible in the `$LD_LIBRARY_PATH`
- `export SCOREP_SUBSTRATE_MTRACE_FILE=(file-prefix)` will create files named `(file-prefix).(mpp_rank)`. For example if it is set to `/tmp/mem_test` and you run a test with 4 MPI ranks, there will be 4 files: `/tmp/mem_test.[0-3]`
- if `$SCOREP_SUBSTRATE_MTRACE_FILE` is not set, an error will be printed and `$MALLOC_TRACE` will be used