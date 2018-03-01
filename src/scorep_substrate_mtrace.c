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
static void
substrate_mtrace_set_callbacks (
    const SCOREP_SubstratePluginCallbacks* callbacks_in, size_t size)
{
  assert(size >= sizeof(SCOREP_SubstratePluginCallbacks));
  callbacks = callbacks_in;
}

/* Called when MPP is initialized (e.g., when MPI_Init() is called) or after init when no MPP is used */
static void
substrate_mtrace_init_mpp (void)
{
  /* Get the metrics from env. variable */
  char* env_metrics = getenv (PREFIX"_FILE");
  if (env_metrics)
  {
    char buffer[512];
    sprintf(buffer,"%s.%d",env_metrics,callbacks->SCOREP_Ipc_GetRank());
    setenv("MALLOC_TRACE", env_metrics, 0);
  }
  else
  {
    if ( callbacks->SCOREP_Ipc_GetRank() == 0 )
      fprintf(stderr,PREFIX"_FILE is not set, $MALLOC_TRACE will be used\n");
  }
  mtrace();
}


/* define the plugin */
SCOREP_SUBSTRATE_PLUGIN_ENTRY (mtrace)
{
  SCOREP_SubstratePluginInfo info;
  memset (&info, 0, sizeof(SCOREP_SubstratePluginInfo));
  info.plugin_version = SCOREP_SUBSTRATE_PLUGIN_VERSION;
  info.init_mpp = substrate_mtrace_init_mpp;
  info.set_callbacks = substrate_mtrace_set_callbacks;
  return info;
}
