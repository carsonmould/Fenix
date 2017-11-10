/*
 *
 *
 *
 */

#include "fenix_data_recovery.h"
#include "fenix_opt.h"
#include "fenix_process_recovery.h"
#include "fenix_util.h"
#include "fenix_ext.h"

int __send_metadata(int current_rank, int in_rank, MPI_Comm comm) {

   fenix_metadata_packet_t group_packet;
   fenix_group_t *group = __fenix_g_data_recovery;
      
   group_packet.count = group->count;
   group_packet.total_size = group->total_size;

   /* 1901 is RECOVER_GROUP_TAG */
   MPI_Send(&group_packet, sizeof(fenix_metadata_packet_t), MPI_BYTE,
            in_rank, 1901, comm);

   return 0;  //FENIX SUCCESS
}

int __send_group_data(int current_rank, int in_rank, 
                      fenix_group_entry_t *group_entry, MPI_Comm comm) {

   fenix_group_entry_packet_t entry_packet;
   entry_packet.group_id = group_entry->group_id;
   entry_packet.timestamp = group_entry->timestamp;
   entry_packet.rank_separation = group_entry->rank_separation;
   entry_packet.state = group_entry->state;

   /* 1902 is RECOVER_GROUP_ENTRY_TAG */
   MPI_Send(&entry_packet, sizeof(fenix_group_entry_packet_t), MPI_BYTE,
            in_rank, 1902, group_entry->comm);

   /* Send metadata for members */
   fenix_member_t *member = group_entry->member;
   fenix_metadata_packet_t member_packet;
   member_packet.count = member->count;
   member_packet.total_size = member->total_size;

   /* 1903 is RECOVER_MEMBER_TAG */
   MPI_Send(&member_packet, sizeof(fenix_metadata_packet_t), MPI_BYTE,
            in_rank, 1903, group_entry->comm);

   return 0;
}

int __recover_metadata(int current_rank, int out_rank, MPI_Comm comm) {

   MPI_Status status;
   fenix_metadata_packet_t group_packet;
   fenix_group_t *group = __fenix_g_data_recovery;

   MPI_Recv(&group_packet, sizeof(fenix_metadata_packet_t), MPI_BYTE,
            out_rank, 1901, comm, &status);
   group->count = group_packet.count;
   group->total_size = group_packet.total_size;

   /* __fenix_data_group_reinit(group, group_packet) */

   return 0;
}

int __recover_group_data(int current_rank, int out_rank,
                         fenix_group_entry_t *group_entry, MPI_Comm comm) {

   fenix_group_entry_packet_t entry_packet;
   MPI_Status status;

   MPI_Recv(&entry_packet, sizeof(fenix_group_entry_packet_t), MPI_BYTE,
            out_rank, 1902, comm, &status);

   group_entry->timestamp = entry_packet.timestamp;
   group_entry->rank_separation = entry_packet.rank_separation;
   group_entry->state = entry_packet.state;

   fenix_metadata_packet_t member_packet;
   MPI_Recv(&member_packet, sizeof(fenix_metadata_packet_t), MPI_BYTE,
            out_rank, 1903, comm, &status);

   fenix_member_t *member = group_entry->member;
   
   /* Reinit member entries, APPARENTLY NEEDS FIX */
   /* __fenix_data_member_reinit(member, member_packet, NEEDFIX) */
   
   return 0;
}
