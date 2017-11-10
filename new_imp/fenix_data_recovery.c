/*
 * Implementation of core Fenix data recovery interface functions.
 */

#include <mpi.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "fenix_data_recovery.h"
#include "fenix_util.h"
#include "fenix_metadata.h"
#include "fenix_ext.h"
#include "fenix_opt.h"

/* not sure where these globals belong yet */
/*fenix_group_t *__fenix_g_data_recovery;
int __fenix_g_role;
MPI_Comm *__fenix_g_new_world;*/

int __Fenix_Data_group_create(int group_id, MPI_Comm comm,
                            int start_time_stamp, int depth) {

   int rtn, new_group, group_position, remote_need_recovery, next_entry;
   fenix_group_t *group;
   fenix_group_entry_t *group_entry;
   MPI_Status status;

   int group_index = __fenix_search_groupid(group_id, __fenix_g_data_recovery);
   
   /* Check if the timestamp is valid.
    * Should always be 0, since versioning is not implemented?
    * May not even have to worry about timestamp value.
    */
   if (start_time_stamp < 0) {
      fprintf(stderr, "Error Fenix_Data_group_create: start_time_stamp <%d> \
must be greater than or equal to zero\n", start_time_stamp);
      rtn = -1;  //set this and all other rtn values to constants later
   } 
   /* Check if depth is valid */
   else if (depth < -1) {
      fprintf(stderr, "Error Fenix_Data_group_create: depth <%d> \
must be greater than or equal to -1\n", depth);
      rtn = -1;
   } 
   /* Check the need for data recovery. */
   else {
      
      /* If the group data is already created */
      group = __fenix_g_data_recovery;
      
      /* yet to implmenet this function */
      // __fenix_ensure_group_capacity(group);
      
      /* determine if the group index exists yet or not */
      if (group_index == -1) {
         new_group = 1;
      } else {
         new_group = 0;
      }

      /* if the group is new, initialize it */
      /* this is either a new or recovered process */
      if (new_group == 1) {

         fprintf(stderr, "This is a new group!\n");
         group->count += 1;
         
         
         /* find the next available group entry */
         next_entry = __fenix_find_next_group_position(group);
         group_entry = &(group->group_entry[next_entry]);

         /* Initialize the entry's metadata */
         group_entry->group_id = group_id;
         group_entry->comm = comm;
         group_entry->timestart = start_time_stamp;  //may not need 
         group_entry->timestamp = start_time_stamp;  //timestamp info
         group_entry->state = OCCUPIED;

         /* If the rank is recovered, set group entry flag */
         if (__fenix_g_role == 1) {
            group_entry->recovered = 1;
         } else {
            group_entry->recovered = 0;
         }

         /* Initialize rank separation.
          * Multiple policies will be implemented in the future,
          * but for the current version we will use this one.
          *
          * Copying this exactly from current implementation.
          *
          * Not sure how this works yet.
          */

         // NEED FIX - function call fails
         /*
         if (__fenix_get_world_size(*__fenix_g_new_world) / 4 > 1) {
            group_entry->rank_separation = __fenix_get_world_size(
                                           *__fenix_g_new_world) / 4;
         } else {
            group_entry->rank_separation = 1;
         }*/
         group_entry->rank_separation = 1;

         fprintf(stderr, "rank: %d, group_id: %d, time: %d, state: %d, count: %d\n",
                 __fenix_get_current_rank(comm), group_entry->group_id, 
                 group_entry->timestamp, group_entry->state, group->count); 
      }
      /* Group has already been created.  Renew MPI Comm */ 
      else {

         /* Ideally this does not happen? */
         group_entry = &(group->group_entry[group_index]);
         group_entry->comm = comm;
      }

      /* Initialize the rest of the entry's metadata */
      group_entry->current_rank = __fenix_get_current_rank(group_entry->comm);
      group_entry->comm_size = __fenix_get_world_size(group_entry->comm);

      /* Not sure what these fields are for */
      /* Is in_rank the rank that current rank stores data for?
       * And out_rank the rank that current rank sends its data too?
       */
      group_entry->in_rank = (group_entry->current_rank + group_entry->comm_size
                             - group_entry->rank_separation) %
                             group_entry->comm_size;
      group_entry->out_rank = (group_entry->current_rank + group_entry->comm_size
                             + group_entry->rank_separation) %
                             group_entry->comm_size;

      /* Tell out_rank if cur_rank need to be recovered.  Ask in_rank if it
       * needs to be recovered
       *
       * 1900 is the constant for PARTNER_STATUS_TAG.. what is this used for?
       */
      MPI_Sendrecv(&(group_entry->recovered), 1, MPI_INT, group_entry->out_rank,
                  1900, &remote_need_recovery, 1, MPI_INT, group_entry->in_rank, 
                  1900, group_entry->comm, &status);

      fprintf(stderr, "rank: %d, group_id: %d, current_rank: %d, comm_size: %d, \
in_rank: %d, out_rank: %d, recovered: %d\n", __fenix_get_current_rank(comm), 
group_entry->group_id, group_entry->current_rank, group_entry->comm_size,
group_entry->in_rank, group_entry->out_rank, group_entry->recovered);

      /* Recover group entry metadata for ranks that need it 
       * Why and how is this recovered?
       */
      if (group_entry->recovered == 1 && remote_need_recovery == 1) {

         /* In_rank needs recovery from cur_rank and cur_rank needs 
          * recovery from out_rank.
          *
          * This is not currently supported by Fenix.  Return error.
          */

         rtn = -1;  //FENIX_ERROR_NODATA_FOUND;
      } else if (group_entry->recovered == 0 && remote_need_recovery == 1) {

         /* In_rank needs recovery from cur_rank */
         rtn = __send_metadata(group_entry->current_rank, 
                               group_entry->in_rank, group_entry->comm);
         rtn = __send_group_data(group_entry->current_rank,
                                 group_entry->in_rank, group_entry,
                                 group_entry->comm); 
      } else if (group_entry->recovered == 1 && remote_need_recovery == 0) {

         /* Cur_rank needs recovery from out_rank */
         /* THIS NEEDS A FIX */
         rtn = __recover_metadata(group_entry->current_rank,
                                  group_entry->out_rank, group_entry->comm);
         rtn = __recover_group_data(group_entry->current_rank,
                                    group_entry->out_rank, group_entry,
                                    group_entry->comm);

         group_entry->recovered = 0;
      }

      /* Need to implement this still */ 
      /* WHAT IS __fenix_g_agree_op FOR ALL_REDUCE? */
      //rtn = __fenix_join_group(group, group_entry, comm);
      rtn = 1;
   }
   return rtn;
}

int __Fenix_Data_member_create(int group_id, int member_id,
                             void *source_buffer, int count,
                             MPI_Datatype datatype) {

   int rtn = -1;
   
   /* Find the requested group and member indices (determine if they exist) */
   int group_index = __fenix_search_groupid(group_id, __fenix_g_data_recovery); 
   int member_index = __fenix_search_member_id(group_index, member_id);

   if (group_index == -1) {
      fprintf(stderr, "ERROR: group_id <%d> does not exist\n", group_id);
      rtn = -1;  //should be FENIX_ERROR_INVALID_GROUPID
   } else if (member_index != -1) {
      fprintf(stderr, "ERROR: member_id <%d> already exists\n", member_id);
      rtn = -1;  //should be FENIX_ERROR_INVALID_MEMBERID
   } else {

      int remote_count, next_member_position;
      MPI_Status status;

      /* Point to group member and initialize member entry */
      fenix_group_t *group = __fenix_g_data_recovery;
      fenix_group_entry_t *group_entry = &(group->group_entry[group_index]);
      fenix_member_t *member = group_entry->member;
      fenix_member_entry_t *member_entry = NULL;
      //fenix_version_t *version = NULL  //no versioning
      
      /* check if member entry needs to be made larger */
      //__fenix_ensure_member_capacity  //still need to implement

      member->count += 1;

      /* Obtain next available member_entry slot */
      next_member_position = __fenix_find_next_member_position(member);
      
      /* Point member_entry to the obtained member_entry slot */
      member_entry = &(member->member_entry[next_member_position]);

      /* Initialize member metadata */

      /* May need to send data type to remote rank - Sendrecv */

      /* Point member entry user_data to provided data */
      member_entry->user_data = source_buffer;

      /* rest of function is versioning, so we dont' care.  may need
       * to steal some functionality from it though 
       */
   }
   
   /* need to call our global agreement function for joining */
   //similar to function call from group_create
   rtn = 1;

   return rtn;
}

int __Fenix_Data_group_delete(int group_id) {
   return 0;
}

int __Fenix_Data_member_delete(int group_id, int member_id) {
   return 0;
}

/* Need to move this to another file */
int __fenix_search_member_id(int group_index, int key) {

   int member_ndx, found;

   fenix_group_t *group = __fenix_g_data_recovery;
   fenix_group_entry_t *group_entry = &(group->group_entry[group_index]);
   fenix_member_t *member = group_entry->member;

   found = -1;

   for (member_ndx = 0; (found == -1) && (member_ndx < member->total_size);
        member_ndx++) {
      fenix_member_entry_t *member_entry = &(member->member_entry[member_ndx]);
      if (key == member_entry->member_id) {
         found = member_ndx;
      }
   }
   return found;
}

/* Also need to move this to another file */
int __fenix_find_next_member_position(fenix_member_t *member) {

   int member_ndx, found;

   fenix_member_t *this_member = member;

   found = -1;
  
   for (member_ndx = 0; (found == -1) && (member_ndx < this_member->total_size);
        member_ndx++) {
      fenix_member_entry_t *member_entry = &(this_member->member_entry[member_ndx]);
      if (member_entry->state == EMPTY || member_entry->state == DELETED) {
         found = member_ndx;
      }
   }
   return found;
}

#if 0
int main(int argc, char **argv) {

   //fprintf(stderr, "Hello world.\n");

   MPI_Comm world_comm;
   int num_ranks, rank;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_dup(MPI_COMM_WORLD, &world_comm);

   /* These are done in Fenix Init */
   __fenix_g_data_recovery = __fenix_data_group_init();
   __fenix_g_new_world = (MPI_Comm *) s_malloc(sizeof(MPI_Comm));

   /* init_rank = 0
    * recovered_rank = 1
    * survivor_rank = 2
    */ 
   __fenix_g_role = 0;   //FENIX_ROLE_INITIAL_RANK

   MPI_Comm_size(world_comm, &num_ranks);
   MPI_Comm_rank(world_comm, &rank);
   if (rank == 0) {
      fprintf(stderr, "Executing the program with %d ranks.\n", num_ranks);
   }

   Fenix_Data_group_create(0, world_comm, 0, 0);

   MPI_Finalize();
   return 0;
}
#endif 
