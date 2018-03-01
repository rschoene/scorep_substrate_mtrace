/*
 * scorep_substrate_mtrace.c
 *
 *  Created on: 01.03.2018
 *      Author: rschoene
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <mcheck.h>

#include <scorep/SCOREP_SubstratePlugins.h>

#define PREFIX "SCOREP_SUBSTRATE_MTRACE"

/* Callback functions from Score-P */
static const SCOREP_SubstratePluginCallbacks* callbacks;

/* sets the Score-P callbacks */
static void substrate_mtrace_set_callbacks(
        const SCOREP_SubstratePluginCallbacks* callbacks_in, size_t size)
{
    assert(size >= sizeof(SCOREP_SubstratePluginCallbacks));
    callbacks = callbacks_in;
}

static char* env_metrics;

/* Called when MPP is initialized (e.g., when MPI_Init() is called) or after init when no MPP is used */
static void substrate_mtrace_init_mpp(void)
{
    /* Get the metrics from env. variable */
    env_metrics = getenv(PREFIX"_FILE");
    if (env_metrics)
    {
        char buffer[512];
        sprintf(buffer, "%s.%d", env_metrics, callbacks->SCOREP_Ipc_GetRank());
        setenv("MALLOC_TRACE", buffer, 0);
        env_metrics = strdup(buffer);
    } else
    {
        if (callbacks->SCOREP_Ipc_GetRank() == 0)
            fprintf(stderr,
            PREFIX"_FILE is not set, $MALLOC_TRACE will be used\n");
        env_metrics = getenv("$MALLOC_TRACE");
        if (env_metrics == NULL)
            env_metrics = strdup("/dev/null/");

    }
    mtrace();
}

static void substrate_mtrace_finalize()
{
    muntrace();
    // copy memory mapping
    FILE* in = fopen("/proc/self/maps", "r");
    if (in == NULL)
    {
        fprintf(stderr, "An error occured while opening /proc/self/maps\n");
        return;
    }
    char buffer[512];
    sprintf(buffer, "%s.maps", env_metrics);
    printf("%s\n", buffer);
    FILE* out = fopen(buffer, "w");
    if (out == NULL)
    {
        free(env_metrics);
        fclose(in);
        fprintf(stderr, "An error occured while opening %s\n", buffer);
        return;
    }
    char data[4096];

    size_t read, write;
    do
    {
        read = fread(data, 1, 4096, in);
        printf("%s", data);
        write = fwrite(data, 1, read, out);
        if (write < read)
        {
            fprintf(stderr, "An error occured while writing %s %d/%d\n", buffer,
                    write, read);
            break;
        }
        if (read < 4096)
        {
            if (ferror(in))
                fprintf(stderr,
                        "An error occured while reading /proc/self/maps\n");
            break;
        }
    } while (1);
    fclose(in);
    fclose(out);
    free(env_metrics);
}

/* define the plugin */
SCOREP_SUBSTRATE_PLUGIN_ENTRY (mtrace)
{
    SCOREP_SubstratePluginInfo info;
    memset(&info, 0, sizeof(SCOREP_SubstratePluginInfo));
    info.plugin_version = SCOREP_SUBSTRATE_PLUGIN_VERSION;
    info.init_mpp = substrate_mtrace_init_mpp;
    info.finalize = substrate_mtrace_finalize;
    info.set_callbacks = substrate_mtrace_set_callbacks;
    return info;
}
