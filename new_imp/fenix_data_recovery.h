/*
 * Header file containing user-level functions for the Fenix
 * data recovery interface.
 */

#ifndef __FENIX_DATA_RECOVERY__
#define __FENIX_DATA_RECOVERY__

#include "fenix_data_group.h"
#include "fenix_data_member.h"
#include "fenix_util.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int Fenix_Data_group_create(int group_id, MPI_Comm comm,
                            int start_time_stamp, int depth);
int Fenix_Data_member_create(int group_id, int member_id,
                             void *source_buffer, int count,
                             MPI_Datatype datatype);

int Fenix_Data_group_delete(int group_id);
int Fenix_Data_member_delete(int group_id, int member_id);

//extern fenix_group_t *fenix_g_data_recovery;

#endif
