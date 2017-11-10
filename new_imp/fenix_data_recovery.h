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
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

int __Fenix_Data_group_create(int group_id, MPI_Comm comm,
                            int start_time_stamp, int depth);
int __Fenix_Data_member_create(int group_id, int member_id,
                             void *source_buffer, int count,
                             MPI_Datatype datatype);

int __Fenix_Data_group_delete(int group_id);
int __Fenix_Data_member_delete(int group_id, int member_id);

// extern fenix_group_t *__fenix_g_data_recovery;

int __send_metadata(int current_rank, int in_rank, MPI_Comm comm);
int __send_group_data(int current_rank, int in_rank,
                      fenix_group_entry_t *group_entry, MPI_Comm comm);
int __recover_metadata(int current_rank, int out_rank, MPI_Comm comm);
int __recover_group_data(int current_rank, int out_rank,
                         fenix_group_entry_t *group_entry, MPI_Comm comm);

int __fenix_search_member_id(int group_index, int key);
int __fenix_find_next_member_position(fenix_member_t *member);

#endif
