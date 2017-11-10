/*
 *
 *
 *
 */

#include <mpi.h>
#include "fenix_data_member.h"
#include "fenix_data_group.h"

fenix_member_t *__fenix_data_member_init() {

   int member_index;   

   fenix_member_t *member = (fenix_member_t *)
                            s_calloc(1, sizeof(fenix_member_t));
   
   member->count = 0;
   member->total_size = __FENIX_DEFAULT_MEMBER_SIZE; //may need to resize 
                                                     //if value goes past max
   member->member_entry = (fenix_member_entry_t *) s_malloc(
           __FENIX_DEFAULT_MEMBER_SIZE * sizeof(fenix_member_entry_t));

   //insert default values into member entries
   //when user adds members, these values are set elsewhere
   for (member_index = 0; member_index < 
                           __FENIX_DEFAULT_MEMBER_SIZE; member_index++) {
      //point to the first member entry in the member allocated above
      fenix_member_entry_t *member_entry = &(member->member_entry[member_index]);
      member_entry->member_id = -1;
      member_entry->state = EMPTY;

      /* not implementing versioning, however may need some of its functionality */
      //member_entry->version = __fenix_data_version_init(); //not implementing version  
   }
   
   return member;
}

//called as part of fenix_finalize to free the allocated memory
void __fenix_data_member_destroy(fenix_member_t *member) {

   int ndx;
  
   /* Un-comment if versioning is implemented */
   /*for (ndx = 0; ndx < member->total_size; ndx++) {
      __fenix_data_version_destroy(member->member_entry[ndx].version);
   }*/

   free(member->member_entry);
   free(member);
}

void __fenix_ensure_ember_capacity(fenix_member_t *member) {

}

void __fenix_ensure_version_capacity_from_member(fenix_member_t *member) {

}

/*int __fenix_search_member_id(int group_index, int key) {

   //fenix_group_t *group = __fenix_g_data_recovery;

}*/
