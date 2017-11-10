/*
 *
 *
 *
 */

#ifndef __FENIX_DATA_MEMBER_H__
#define __FENIX_DATA_MEMBER_H__

#include <mpi.h>
#include "fenix_util.h"

#define __FENIX_DEFAULT_MEMBER_SIZE 512 //look at this later

/* Need to determine which fields are necessary
 * and which ones need to be removed because 
 * versioning will not be implemented
 */
typedef struct __fenix_member_entry {
   int member_id;
   enum states state;  //states from fenix util
   //fenix_version_t *version;
   void *user_data;
   MPI_Datatype current_datatype;
   int datatype_size;
   int current_count;
   int current_size;
   int current_rank;
   int remote_rank;
   int remote_rank_front;
   int remote_rank_back;
} fenix_member_entry_t;

typedef struct __fenix_member {
   size_t count;
   int temp_count;
   size_t total_size;
   fenix_member_entry_t *member_entry;
} fenix_member_t;

fenix_member_t *__fenix_data_member_init();

void __fenix_data_member_destroy(fenix_member_t *member);
void __fenix_ensure_member_capacity(fenix_member_t *member);

/* Likely not needed due to versioning */
void __fenix_ensure_version_capacity_from_member(fenix_member_t *member);

//int __fenix_search_member_id(int group_index, int key);

#endif
