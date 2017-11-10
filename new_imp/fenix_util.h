/*
 *
 *
 *
 */

/* Just creating the declarations for these utility functions
 * and variables.  Need to determine which are useful for my
 * implementation.
 */

#ifndef __FENIX_UTIL__
#define __FENIX_UTIL__

#include "fenix.h"
#include "fenix_process_recovery.h"
#include <mpi.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/time.h>
#include <signal.h>
#include <libgen.h>

char *log_name;

#define LDEBUG(f...)  {LLIND("debug",f);}
#define LLIND(t,f...) {fprintf(stderr,"%s - %s (%i): %s: \n",logname,__PRETTY_FUNCTION__,getpid(),t); fprintf(stderr,f);}
#define ERRHANDLE(f...){LFATAL(f);}
#define LFATAL(f...)  {LLINF("fatal", f);}
#define LLINF(t,f...) {fprintf(stderr,"(%i): %s: ", getpid(), t); fprintf(stderr, f);}

enum states { EMPTY = 0, OCCUPIED = 1, DELETED = 2, NEEDFIX = 3 };

void __fenix_ranks_agree(int *, int *, int *, MPI_Datatype *);
int __fenix_binary_search(int *, int, int);
int __fenix_comparator(const void *, const void *);
int __fenix_get_size(MPI_Datatype);
int __fenix_get_fenix_default_rank_separation();
int __fenix_get_current_rank(MPI_Comm);
int __fenix_get_partner_rank(int, MPI_Comm);
int __fenix_get_world_size(MPI_Comm);
int __fenix_mpi_wait(MPI_Request *);
int __fenix_mpi_test(MPI_Request *);

void *s_calloc(int count, size_t size);
void *s_malloc(size_t size);
void *s_realloc(void *member, size_t size);

#endif
