/*
 * Implementation of core Fenix data recovery interface functions.
 */

#include "fenix_data_recovery.h"
#include "fenix_util.h"
#include "fenix_metadata.h"

int Fenix_Data_group_create(int group_id, MPI_Comm comm,
                            int start_time_stamp, int depth) {

   //int group_index = fenix_search_groupid(group_id, fenix_g_data_recovery);

   return 0;
}

int Fenix_Data_member_create(int group_id, int member_id,
                             void *source_buffer, int count,
                             MPI_Datatype datatype) {
   return 0;
}

int Fenix_Data_group_delete(int group_id) {
   return 0;
}

int Fenix_Data_member_delete(int group_id, int member_id) {
   return 0;
}

int main(int argc, char **argv) {

   return 0;
}
