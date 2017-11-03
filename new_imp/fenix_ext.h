/*
 *
 *
 *
 */

#ifndef __FENIX_EXT_H__
#define __FENIX_EXT_H__

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <getopt.h>

//#include "fenix_opt.h"
#include "fenix_data_group.h"

extern fenix_group_t *__fenix_g_data_recovery;
extern int __fenix_g_role;

#endif
