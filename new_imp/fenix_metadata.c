/*
 *
 *
 *
 */

#include "fenix_data_recovery.h"
#include "fenix_util.h"
#include "fenix_metadata.h"

/* Need to determine when and what these functions are used for. */

inline void __fenix_init_group_metadata(fenix_group_entry_t *group_entry, 
                                        int group_id, MPI_Comm comm,
                                        int time_stamp, int depth) {

}

inline void __fenix_reinit_group_metadata(fenix_group_entry_t *group_entry) {

}

inline void __fenix_data_member_init_metadata(fenix_member_entry_t *member_entry, 
                                              int member_id, void *data, 
                                              int count, MPI_Datatype datatype) {

}

