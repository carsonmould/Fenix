/*
 *
 *
 *
 */

#ifndef __FENIX_METADATA_H__
#define __FENIX_METADATA_H__

#include "fenix_data_recovery.h"

/* Not sure yet if this functionality is required. */

/* Also need to determine which members are needed.
 * I believe depth is not because that pertains to 
 * versioning and versioning is not implemented.
 */
void __fenix_init_group_metadata(fenix_group_entry_t *group_entry,
                                 int group_id, MPI_Comm comm,
                                 int time_stamp, int depth);
void __fenix_reinit_group_metadata(fenix_group_entry_t *group_entry);

void __fenix_data_member_init_metadata(fenix_member_entry_t *member_entry,
                                       int member_id, void *data,
                                       int count, MPI_Datatype datatype);
#endif
