/*
//@HEADER
// ************************************************************************
//
//
//            _|_|_|_|  _|_|_|_|  _|      _|  _|_|_|  _|      _|
//            _|        _|        _|_|    _|    _|      _|  _|
//            _|_|_|    _|_|_|    _|  _|  _|    _|        _|
//            _|        _|        _|    _|_|    _|      _|  _|
//            _|        _|_|_|_|  _|      _|  _|_|_|  _|      _|
//
//
//
//
// Copyright (C) 2016 Rutgers University and Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author Marc Gamell, Eric Valenzuela, Keita Teranishi, Manish Parashar
//        and Michael Heroux
//
// Questions? Contact Keita Teranishi (knteran@sandia.gov) and
//                    Marc Gamell (mgamell@cac.rutgers.edu)
//
// ************************************************************************
//@HEADER
*/

//#include "fenix.h"
#include "fenix_constants.h"
#include "fenix_process_recovery.h"
#include "fenix_data_recovery.h"
#include "fenix_hash.h"
#include "fenix_opt.h"
#include "fenix_util.h"
#include <mpi.h>

extern int *rank_roles;
int __fenix_g_fenix_init_flag = 0;
struct __fenix_hash_table *outstanding_request = NULL;
//struct __fenix_callback_list *__fenix_g_callback_list = NULL;
__fenix_callback_list *__fenix_g_callback_list;
__fenix_debug_options __fenix_options;


/**
 * @brief
 * @param role
 * @param comm
 * @param new_comm
 * @param argc
 * @param argv 
 * @param spare_ranks
 * @param info
 * @param error
 * @param jump_environment
 */
int preinit(int *role, MPI_Comm comm, MPI_Comm *new_comm, int *argc, char ***argv,
            int spare_ranks,
            int spawn,
            MPI_Info info, int *error, jmp_buf *jump_environment) {

  int ret;
  *role = __fenix_g_role;
  *error = 0;

  if (new_comm != NULL) {
    __fenix_g_user_world = new_comm;
    __fenix_g_replace_comm_flag = 0;
  } else {
    __fenix_g_replace_comm_flag = 1;
  }
  //__fenix_g_original_comm = comm; 
  __fenix_g_spare_ranks = spare_ranks;
  __fenix_g_spawn_policy = spawn;
  __fenix_g_recover_environment = jump_environment;
  __fenix_g_role = FENIX_ROLE_INITIAL_RANK;
  __fenix_g_resume_mode = __FENIX_RESUME_AT_INIT;
  __fenix_g_repair_result = 0;

  __fenix_init_opt(*argc, *argv);

  MPI_Op_create((MPI_User_function *) __fenix_ranks_agree, 1, &__fenix_g_agree_op);

  /* Check the values in info */
  if (info != MPI_INFO_NULL) {
    char value[MPI_MAX_INFO_VAL + 1];
    int vallen = MPI_MAX_INFO_VAL;
    int flag;
    MPI_Info_get(info, "FENIX_RESUME_MODE", vallen, value, &flag);

    if (flag == 1) {
      if (strcmp(value, "Fenix_init") == 0) {
        __fenix_g_resume_mode = __FENIX_RESUME_AT_INIT;
        if (__fenix_options.verbose == 0) {
          verbose_print("rank: %d, role: %d, value: %s\n",
            __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role, value);
        }
      } else if (strcmp(value, "NO_JUMP") == 0) {
        __fenix_g_resume_mode = __FENIX_RESUME_NO_JUMP;
        if (__fenix_options.verbose == 0) {
           verbose_print("rank: %d, role: %d, value: %s\n",
            __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role, value);
        }

      } else {
        /* No support. Setting it to Fenix_init */
        __fenix_g_resume_mode = __FENIX_RESUME_AT_INIT;
      }
    }
  }

  if (__fenix_g_spare_ranks >= __fenix_get_world_size(comm)) {
    debug_print("Fenix: <%d> spare ranks requested are unavailable\n",
                __fenix_g_spare_ranks);
  }

  __fenix_g_world = (MPI_Comm *) s_malloc(sizeof(MPI_Comm));
  MPI_Comm_dup(comm, __fenix_g_world);
  MPI_Comm_dup(comm, &__fenix_g_original_comm);

  __fenix_g_data_recovery = __fenix_init_group();

  __fenix_g_new_world = (MPI_Comm *) s_malloc(sizeof(MPI_Comm));

  /*****************************************************/
  /* Note: __fenix_g_new_world is only valid for the   */
  /*       active MPI ranks. Spare ranks do not        */
  /*       allocate any communicator content with this.*/
  /*       Any MPI calls in spare ranks with new_world */
  /*       trigger an abort.                           */
  /*****************************************************/

  ret = 1;
  while (ret) {
    ret = create_new_world();
    if (ret) {
      // just_repair_process();
    }
  }

  outstanding_request = __fenix_hash_table_new(512);
 
  if (spare_rank() != 1) {
    __fenix_g_num_inital_ranks = __fenix_get_world_size(*__fenix_g_new_world);
    if (__fenix_options.verbose == 0) {
     verbose_print("rank: %d, role: %d, number_initial_ranks: %d\n",
                  __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role,
                  __fenix_g_num_inital_ranks);   
    }

  } else {
    __fenix_g_num_inital_ranks = spare_ranks;

    if (__fenix_options.verbose == 0) {
       verbose_print("rank: %d, role: %d, number_initial_ranks: %d\n",
        __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role,
            __fenix_g_num_inital_ranks);   
    }
  }
  __fenix_g_num_survivor_ranks = 0;
  __fenix_g_num_recovered_ranks = 0;
  while (spare_rank() == 1) {
    int a;
    int myrank;
    MPI_Status mpi_status;
    ret = PMPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, *__fenix_g_world,
                    &mpi_status); // listen for a failure
    if (ret == MPI_SUCCESS) {
      if (__fenix_options.verbose == 0) {
        verbose_print("Finalize the program; rank: %d, role: %d\n",
                      __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role);
      }
      finalize_spare();
    } else {
      __fenix_g_repair_result = repair_ranks();
      if (__fenix_options.verbose == 0) {
        verbose_print("repair ranks; rank: %d, role: %d\n",
                      __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role);
      }
    }
    __fenix_g_role = FENIX_ROLE_RECOVERED_RANK;
  }

  return __fenix_g_role;
}


/**
 * @brief
 */
int create_new_world() {
  int ret;
  ret = PMPI_Comm_set_errhandler(*__fenix_g_world, MPI_ERRORS_RETURN);

  if (spare_rank() == 1) {
    int current_rank = __fenix_get_current_rank(*__fenix_g_world);

    /*************************************************************************/
    /** MPI_UNDEFINED makes the new communicator "undefined" at spare ranks **/
    /** This means no data is allocated at spare ranks                      **/
    /** Use of the new communicator triggers the program to abort .         **/
    /*************************************************************************/

    if (__fenix_options.verbose == 1) {
      verbose_print("rank: %d, role: %d\n", __fenix_get_current_rank(*__fenix_g_world),
                    __fenix_g_role);
    }

    ret = PMPI_Comm_split(*__fenix_g_world, MPI_UNDEFINED, current_rank,
                          __fenix_g_new_world);
    if (ret != MPI_SUCCESS) { debug_print("MPI_Comm_split: %d\n", ret); }

  } else {

    int current_rank = __fenix_get_current_rank(*__fenix_g_world);

    if (__fenix_options.verbose == 1) {
      verbose_print("rank: %d, role: %d\n", __fenix_get_current_rank(*__fenix_g_world),
                    __fenix_g_role);
    }

    ret = PMPI_Comm_split(*__fenix_g_world, 0, current_rank, __fenix_g_new_world);
    if (ret != MPI_SUCCESS) { debug_print("MPI_Comm_split: %d\n", ret); }
    MPI_Comm_set_errhandler(*__fenix_g_new_world, MPI_ERRORS_RETURN);

  }
  return ret;
}

#if 1
/**
 * @brief
 */
int repair_ranks() {

  /*********************************************************/
  /* Do not forget comm_free for broken communicators      */
  /*********************************************************/


  int ret;
  int survived_flag;
  int *survivor_world;
  int *fail_world;
  int current_rank;
  int survivor_world_size;
  int world_size;
  int fail_world_size;
  int rt_code = FENIX_SUCCESS;
  int repair_success = 0;
  int num_try = 0;
  int flag_g_world_freed = 0;
  MPI_Comm world_without_failures;

  if (__fenix_get_current_rank(*__fenix_g_world) == 0) {
    printf("Fenix: repairing communicators\n");
  }

  while (!repair_success) {
    repair_success = 1;
    ret = MPIF_Comm_shrink(*__fenix_g_world, &world_without_failures);
    /* if (ret != MPI_SUCCESS) { debug_print("MPI_Comm_shrink. repair_ranks\n"); } */
    if (ret != MPI_SUCCESS) {
      repair_success = 0;
      goto END_LOOP;
    }

    ret = MPI_Comm_set_errhandler(world_without_failures, MPI_ERRORS_RETURN);
    if (ret != MPI_SUCCESS) {
      repair_success = 0;
      MPI_Comm_free(&world_without_failures);
      goto END_LOOP;
    }

    /*********************************************************/
    /* Free up the stroage for active procee communicator */
    /*********************************************************/
    if (spare_rank() != 1) {
      PMPI_Comm_free(__fenix_g_new_world);
      if (__fenix_g_replace_comm_flag == 0) {
        PMPI_Comm_free(__fenix_g_user_world);
      }
    }
    /*********************************************************/
    /* Need closer look above                                */
    /*********************************************************/

    /* current_rank means the global MPI rank before failure */
    current_rank = __fenix_get_current_rank(*__fenix_g_world);
    survivor_world_size = __fenix_get_world_size(world_without_failures);
    world_size = __fenix_get_world_size(*__fenix_g_world);
    fail_world_size = world_size - survivor_world_size;

    if (__fenix_options.verbose == 2) {
      verbose_print(
              "current_rank: %d, role: %d, world_size: %d, fail_world_size: %d, survivor_world_size: %d\n",
              __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role, world_size,
              fail_world_size, survivor_world_size);
    }

    if (__fenix_g_spare_ranks < fail_world_size) {
      /* Not enough spare ranks */

      if (__fenix_options.verbose == 2) {
        verbose_print(
                "current_rank: %d, role: %d, spare_ranks: %d, fail_world_size: %d\n",
                __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role, __fenix_g_spare_ranks,
                fail_world_size);
      }

      if (__fenix_g_spawn_policy == 1) {
        debug_print("Spawn policy <%d>is not supported\n", __fenix_g_spawn_policy);
      } else {

        if (__fenix_g_spare_ranks != 0) {

          /***************************************/
          /* Fill the ranks in increasing order  */
          /***************************************/

          int active_ranks;

          rt_code = FENIX_WARNING_SPARE_RANKS_DEPLETED;

          survivor_world = (int *) s_malloc(survivor_world_size * sizeof(int));

          ret = PMPI_Allgather(&current_rank, 1, MPI_INT, survivor_world, 1, MPI_INT,
                               world_without_failures);

          if (__fenix_options.verbose == 2) {
            int index;
            for (index = 0; index < survivor_world_size; index++) {
              verbose_print("current_rank: %s, role: %d, survivor_world[%d]: %d\n",
                            __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role, index,
                            survivor_world[index]);
            }
          }

          /* if (ret != MPI_SUCCESS) { debug_print("MPI_Allgather. repair_ranks\n"); } */
          if (ret != MPI_SUCCESS) {
            repair_success = 0;
            if (ret == MPI_ERR_PROC_FAILED) {
              MPIF_Comm_revoke(world_without_failures);
            }
            MPI_Comm_free(&world_without_failures);
            free(survivor_world);
            goto END_LOOP;
          }

          survived_flag = 0;
          if (__fenix_g_role == FENIX_ROLE_SURVIVOR_RANK) {
            survived_flag = 1;
          }

          ret = PMPI_Allreduce(&survived_flag, &__fenix_g_num_survivor_ranks, 1,
                               MPI_INT, MPI_SUM, world_without_failures);

          /* if (ret != MPI_SUCCESS) { debug_print("MPI_Allreduce. repair_ranks\n"); } */
          if (ret != MPI_SUCCESS) {
            repair_success = 0;
            if (ret == MPI_ERR_PROC_FAILED) {
              MPIF_Comm_revoke(world_without_failures);
            }
            MPI_Comm_free(&world_without_failures);
            free(survivor_world);
            goto END_LOOP;
          }

          __fenix_g_num_inital_ranks = 0;

          /* recovered ranks must be the number of spare ranks */
          __fenix_g_num_recovered_ranks = fail_world_size;

          if (__fenix_options.verbose == 2) {
            verbose_print("current_rank: %d, role: %d, recovered_ranks: %d\n",
                          __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role,
                          __fenix_g_num_recovered_ranks);
          }

          fail_world = (int *) s_malloc(fail_world_size * sizeof(int));
          fail_world = get_fail_ranks(survivor_world, survivor_world_size,
                                      fail_world_size);

          if (__fenix_options.verbose == 2) {
            int index;
            for (index = 0; index < fail_world_size; index++) {
              verbose_print("fail_world[%d]: %d\n", index, fail_world[index]);
            }
          }

          free(survivor_world);

          active_ranks = world_size - __fenix_g_spare_ranks;

          if (__fenix_options.verbose == 2) {
            verbose_print("current_rank: %d, role: %d, active_ranks: %d\n",
                          __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role,
                          active_ranks);
          }

          /* Assign new rank for reordering */
          if (current_rank >= active_ranks) { // reorder ranks
            int rank_offset = ((world_size - 1) - current_rank);
            if (rank_offset < fail_world_size) {
              if (__fenix_options.verbose == 11) {
                verbose_print("reorder ranks; current_rank: %d -> new_rank: %d\n",
                              current_rank, fail_world[rank_offset]);
              }
              current_rank = fail_world[rank_offset];
            }
          }

          free(fail_world);

          /************************************/
          /* Update the number of spare ranks */
          /************************************/
          __fenix_g_spare_ranks = 0;

          /* debug_print("not enough spare ranks to repair rank failures. repair_ranks\n"); */
        }

        /****************************************************************/
        /* No rank reordering is required if no spare rank is available */
        /****************************************************************/

      }
    } else {


      int active_ranks;

      survivor_world = (int *) s_malloc(survivor_world_size * sizeof(int));

      ret = PMPI_Allgather(&current_rank, 1, MPI_INT, survivor_world, 1, MPI_INT,
                           world_without_failures);
      /* if (ret != MPI_SUCCESS) { debug_print("MPI_Allgather. repair_ranks\n"); } */
      if (ret != MPI_SUCCESS) {
        repair_success = 0;
        if (ret == MPI_ERR_PROC_FAILED) {
          MPIF_Comm_revoke(world_without_failures);
        }
        MPI_Comm_free(&world_without_failures);
        free(survivor_world);
        goto END_LOOP;
      }

      survived_flag = 0;
      if (__fenix_g_role == FENIX_ROLE_SURVIVOR_RANK) {
        survived_flag = 1;
      }

      ret = PMPI_Allreduce(&survived_flag, &__fenix_g_num_survivor_ranks, 1,
                           MPI_INT, MPI_SUM, world_without_failures);
      /*  if (ret != MPI_SUCCESS) { debug_print("MPI_Allreduce. repair_ranks\n"); } */
      if (ret != MPI_SUCCESS) {
        repair_success = 0;
        if (ret != MPI_ERR_PROC_FAILED) {
          MPIF_Comm_revoke(world_without_failures);
        }
        MPI_Comm_free(&world_without_failures);
        free(survivor_world);
        goto END_LOOP;
      }

      __fenix_g_num_inital_ranks = 0;
      __fenix_g_num_recovered_ranks = fail_world_size;

      fail_world = (int *) s_malloc(fail_world_size * sizeof(int));
      fail_world = get_fail_ranks(survivor_world, survivor_world_size, fail_world_size);
      free(survivor_world);

      if (__fenix_options.verbose == 2) {
        int index;
        for (index = 0; index < fail_world_size; index++) {
          verbose_print("fail_world[%d]: %d\n", index, fail_world[index]);
        }
      }

      active_ranks = world_size - __fenix_g_spare_ranks;

      if (__fenix_options.verbose == 2) {
        verbose_print("current_rank: %d, role: %d, active_ranks: %d\n",
                      __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role, active_ranks);
      }

      if (current_rank >= active_ranks) { // reorder ranks
        int rank_offset = ((world_size - 1) - current_rank);
        if (rank_offset < fail_world_size) {
          if (__fenix_options.verbose == 2) {
            verbose_print("reorder ranks; current_rank: %d -> new_rank: %d\n",
                          current_rank, fail_world[rank_offset]);
          }
          current_rank = fail_world[rank_offset];
        }
      }

      free(fail_world);

      /************************************/
      /* Update the number of spare ranks */
      /************************************/
      __fenix_g_spare_ranks = __fenix_g_spare_ranks - fail_world_size;
      if (__fenix_options.verbose == 2) {
        verbose_print("current_rank: %d, role: %d, spare_ranks: %d\n",
                      __fenix_get_current_rank(*__fenix_g_world), __fenix_g_role,
                      __fenix_g_spare_ranks);
      }
    }

    /*********************************************************/
    /* Done with the global communicator                     */
    /*********************************************************/

    if (!flag_g_world_freed) {
      ret = PMPI_Comm_free(__fenix_g_world);
      if (ret != MPI_SUCCESS) { flag_g_world_freed = 1; }
    }
    ret = PMPI_Comm_split(world_without_failures, 0, current_rank, __fenix_g_world);

    /* if (ret != MPI_SUCCESS) { debug_print("MPI_Comm_split. repair_ranks\n"); } */
    if (ret != MPI_SUCCESS) {
      repair_success = 0;
      if (ret != MPI_ERR_PROC_FAILED) {
        MPIF_Comm_revoke(world_without_failures);
      }
      MPI_Comm_free(&world_without_failures);
      goto END_LOOP;
    }
    ret = PMPI_Comm_free(&world_without_failures);

    /* As of 8/8/2016                            */
    /* Need special treatment for error handling */
    create_new_world();

    ret = PMPI_Barrier(*__fenix_g_world);
    /* if (ret != MPI_SUCCESS) { debug_print("MPI_Barrier. repair_ranks\n"); } */
    if (ret != MPI_SUCCESS) {
      repair_success = 0;
      if (ret != MPI_ERR_PROC_FAILED) {
        MPIF_Comm_revoke(*__fenix_g_world);
      }
    }

    END_LOOP:
    num_try++;

    /*******************************************************/
    /*** Not sure if we should include verbose statement ***/
    /*******************************************************/

/*
    if (__fenix_get_current_rank(*__fenix_g_world) == FENIX_ROOT) {
      LDEBUG("Fenix: communicators repaired\n");
    }
*/
  }
  return rt_code;
}

#else
int repair_ranks() {

  /*********************************************************/
  /* Do not forget comm_free for broken communicators      */
  /*********************************************************/

  int ret;
  int survived_flag;
  int *survivor_world;
  int *fail_world;
  int current_rank;
  int survivor_world_size;
  int world_size;
  int fail_world_size;
  int rt_code = FENIX_SUCCESS;
  MPI_Comm world_without_failures;

  if (__fenix_get_current_rank(*__fenix_g_world) == FENIX_ROOT) {
    printf("Fenix: repairing communicators\n");
  }

  ret = MPIF_Comm_shrink(*__fenix_g_world, &world_without_failures);
  if (ret != MPI_SUCCESS) { debug_print("MPI_Comm_shrink. repair_ranks\n"); }

  MPI_Comm_set_errhandler(world_without_failures, MPI_ERRORS_RETURN);

  
  /*********************************************************/
  /* Free up the stroage for active procee communicator */
  /*********************************************************/

  if( spare_rank() != 1 ) {
    MPI_Comm_free( __fenix_g_new_world );
    if( __fenix_g_replace_comm_flag == 0  ) {
      MPI_Comm_free( __fenix_g_user_world );
    }
  }

  /* current_rank means the global MPI rank before failure */
  current_rank = __fenix_get_current_rank(*__fenix_g_world);
  survivor_world_size = __fenix_get_world_size(world_without_failures);
  world_size = __fenix_get_world_size(*__fenix_g_world);
  fail_world_size = world_size - survivor_world_size;

  if (__fenix_g_spare_ranks < fail_world_size) {

    /* Not enough spare ranks */

    if (__fenix_g_spawn_policy == 1) {
      debug_print("Spawn policy is not supported. repair_ranks\n");
    } else {

      if( __fenix_g_spare_ranks != 0 ) {
        /***************************************/
        /* Fill the ranks in increasing order  */
        /***************************************/
        int active_ranks;

        rt_code = FENIX_WARNING_SPARE_RANKS_DEPLETED;

        survivor_world = (int *) s_malloc(survivor_world_size * sizeof(int));

        ret = PMPI_Allgather( &current_rank, 1, MPI_INT, survivor_world, 1, MPI_INT,
                              world_without_failures );

        if (ret != MPI_SUCCESS) { debug_print("MPI_Allgather. repair_ranks\n"); }

        survived_flag = 0;
        if (__fenix_g_role == FENIX_ROLE_SURVIVOR_RANK) {
          survived_flag = 1;
        }

        ret = PMPI_Allreduce(&survived_flag, &__fenix_g_num_survivor_ranks, 1,
                             MPI_INT, MPI_SUM, world_without_failures);
        if (ret != MPI_SUCCESS) { debug_print("MPI_Allreduce. repair_ranks\n"); }

        __fenix_g_num_inital_ranks = 0;
        
        /* recovered ranks must be the number of spara ranks */
        __fenix_g_num_recovered_ranks = fail_world_size;

        fail_world = (int *) s_malloc(fail_world_size * sizeof(int));
        fail_world = get_fail_ranks(survivor_world, survivor_world_size, fail_world_size);
        free(survivor_world);

        active_ranks = world_size - __fenix_g_spare_ranks;

        /* Assign new rank for reordering */
        if (current_rank >= active_ranks) { // reorder ranks
          int rank_offset = ((world_size - 1) - current_rank);
          if (rank_offset < fail_world_size) {
            current_rank = fail_world[rank_offset];
          }
        }

        free(fail_world);

        /************************************/
        /* Update the number of spare ranks */
        /************************************/
        __fenix_g_spare_ranks = 0;
         
     //   debug_print("not enough spare ranks to repair rank failures. repair_ranks\n");
      } 
      /****************************************************************/
      /* No rank reordering is required if no spare rank is available */
      /****************************************************************/
    }
  } else {
    int active_ranks;

    survivor_world = (int *) s_malloc(survivor_world_size * sizeof(int));

    ret = PMPI_Allgather(&current_rank, 1, MPI_INT, survivor_world, 1, MPI_INT,
                       world_without_failures);

    if (ret != MPI_SUCCESS) { debug_print("MPI_Allgather. repair_ranks\n"); }

    survived_flag = 0;
    if (__fenix_g_role == FENIX_ROLE_SURVIVOR_RANK) {
      survived_flag = 1;
    }

    ret = PMPI_Allreduce(&survived_flag, &__fenix_g_num_survivor_ranks, 1,
                         MPI_INT, MPI_SUM, world_without_failures);
    if (ret != MPI_SUCCESS) { debug_print("MPI_Allreduce. repair_ranks\n"); }
    __fenix_g_num_inital_ranks = 0;
    __fenix_g_num_recovered_ranks = fail_world_size;

    fail_world = (int *) s_malloc(fail_world_size * sizeof(int));
    fail_world = get_fail_ranks(survivor_world, survivor_world_size, fail_world_size);
    free(survivor_world);

    active_ranks = world_size - __fenix_g_spare_ranks;

    //printf("*** / *** (01) current_rank: %d; world_size: %d; spare_ranks: %d; survive: %d; fail: %d\n", __fenix_get_current_rank(*__fenix_g_world), world_size, __fenix_g_spare_ranks, survivor_world_size, fail_world_size);

    if (current_rank >= active_ranks) { // reorder ranks
      int rank_offset = ((world_size - 1) - current_rank);
      if (rank_offset < fail_world_size) {
        //printf("*** / *** (20) current_rank: %d; active_ranks: %d; rank_offset: %d\n", current_rank, active_ranks, rank_offset);
        current_rank = fail_world[rank_offset];
        //printf("*** / *** (30) current_rank: %d; fail_world[rank_offset]:%d\n", current_rank, fail_world[rank_offset]);
      }
    }

    free(fail_world);

    __fenix_g_spare_ranks = __fenix_g_spare_ranks - fail_world_size;
  }

  MPI_Comm_free( __fenix_g_world );

  ret = PMPI_Comm_split( world_without_failures, 0, current_rank, __fenix_g_world );

  if (ret != MPI_SUCCESS) { debug_print("MPI_Comm_split. repair_ranks\n"); }
  MPI_Comm_free( &world_without_failures );

  create_new_world();

  ret = PMPI_Barrier(*__fenix_g_world);
  if (ret != MPI_SUCCESS) { debug_print("MPI_Barrier. repair_ranks\n"); }

 return rt_code;
}
#endif

/**
 * @brief
 * @param survivor_world
 * @param survivor_world_size
 * @param fail_world_size
 * @param 
 */
int *get_fail_ranks(int *survivor_world, int survivor_world_size, int fail_world_size) {
  qsort(survivor_world, survivor_world_size, sizeof(int), __fenix_comparator);
  int failed_pos = 0;
  int *fail_ranks = calloc(fail_world_size, sizeof(int));
  int i;
  for (i = 0; i < survivor_world_size; i++) {
    if (__fenix_binary_search(survivor_world, survivor_world_size, i) != 1) {
      if (__fenix_options.verbose == 14) {
        verbose_print("fail_rank: %d, fail_ranks[%d]: %d\n", i, failed_pos,
                      fail_ranks[failed_pos++]);
      }
      fail_ranks[failed_pos++] = i;
    }
  }
  return fail_ranks;
}

/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */
int spare_rank() {
  int result = -1;
  int current_rank = __fenix_get_current_rank(*__fenix_g_world);
  int new_world_size = __fenix_get_world_size(*__fenix_g_world) - __fenix_g_spare_ranks;
  if (current_rank >= new_world_size) {
    if (__fenix_options.verbose == 6) {
      verbose_print("current_rank: %d, new_world_size: %d\n", current_rank, new_world_size);
    }
    result = 1;
  }
  return result;
}

/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */
int callback_register(void (*recover)(MPI_Comm, int, void *), void *callback_data) {
  int error_code = FENIX_SUCCESS;
  if (__fenix_g_fenix_init_flag) {
    fenix_callback_func *fp = s_malloc(sizeof(fenix_callback_func));
    fp->x = recover;
    fp->y = callback_data;
    __fenix_callback_push(&__fenix_g_callback_list, fp);
  } else {
    error_code = FENIX_ERROR_UNINITIALIZED;
  }
  return error_code;
}

/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */
void postinit(int *error) {

  if (__fenix_options.verbose == 9) {
    verbose_print("current_rank: %d, role: %d\n", __fenix_get_current_rank(*__fenix_g_new_world),
                  __fenix_g_role);
  }

  PMPI_Barrier(*__fenix_g_new_world);


  if (__fenix_g_replace_comm_flag == 0) {
    PMPI_Comm_dup(*__fenix_g_new_world, __fenix_g_user_world);
    PMPI_Comm_set_errhandler(*__fenix_g_user_world, MPI_ERRORS_RETURN);
  } else {
    PMPI_Comm_set_errhandler(*__fenix_g_new_world, MPI_ERRORS_RETURN);
  }

  if (__fenix_g_repair_result != 0) {
    *error = __fenix_g_repair_result;
  }
  __fenix_g_fenix_init_flag = 1;

#if 0
  if (__fenix_g_role != FENIX_ROLE_INITIAL_RANK) {
    init_data_recovery();
  }
#endif

  if (__fenix_g_role == FENIX_ROLE_SURVIVOR_RANK) {
    struct callback_list *current = __fenix_g_callback_list;
    while (current != NULL) {
      (current->callback.x)((MPI_Comm) * __fenix_g_new_world, (int) *error,
                            (void *) current->callback.y);
      current = current->next;
    }
  }
  if (__fenix_options.verbose == 9) {
    verbose_print("After barrier. current_rank: %d, role: %d\n", __fenix_get_current_rank(*__fenix_g_new_world),
                  __fenix_g_role);
  }
}

/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */
void finalize() {
  /* Last Barrier Statement */
  MPI_Barrier( *__fenix_g_new_world );
  if (__fenix_options.verbose == 10) {
    verbose_print("current_rank: %d, role: %d\n", __fenix_get_current_rank(*__fenix_g_new_world),
                  __fenix_g_role);
  }

  __fenix_g_fenix_init_flag = 0;
  if (__fenix_get_current_rank(*__fenix_g_world) == 0) {
    int spare_rank = __fenix_get_world_size(*__fenix_g_world) - 1;
    int a;
    int i;
    for (i = 0; i < __fenix_g_spare_ranks; i++) {
      int ret = PMPI_Send(&a, 1, MPI_INT, spare_rank, 1, *__fenix_g_world);
      if (ret != MPI_SUCCESS) { debug_print("MPI_Send: %d\n", ret); }
      if (__fenix_options.verbose == 10) {
        verbose_print("spare_rank: %d sending msg!\n", spare_rank);
      }
      spare_rank--;
    }
  }

  int ret = PMPI_Barrier(*__fenix_g_world);
  if (ret != MPI_SUCCESS) { debug_print("MPI_Barrier: %d\n", ret); } 

  MPI_Op_free(&__fenix_g_agree_op);
  MPI_Comm_set_errhandler(*__fenix_g_world, MPI_ERRORS_ARE_FATAL);
  MPI_Comm_free(__fenix_g_world);
  MPI_Comm_free(__fenix_g_new_world);
  free(__fenix_g_world);
  free(__fenix_g_new_world);

  /* Free the Hash Table */

}

/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */
void finalize_spare() {

  __fenix_g_fenix_init_flag = 0;
  int ret = PMPI_Barrier(*__fenix_g_world);
  if (ret != MPI_SUCCESS) { debug_print("MPI_Barrier: %d\n", ret); } 
 
  MPI_Op_free(&__fenix_g_agree_op);
  MPI_Comm_set_errhandler(*__fenix_g_world, MPI_ERRORS_ARE_FATAL);
  MPI_Comm_free(__fenix_g_world);

  /* This communicator is invalid for spare processes */
  /* MPI_Comm_free(__fenix_g_new_world); */ 
  free(__fenix_g_world);
  free(__fenix_g_new_world);

  /* Free the Hash Table */

  MPI_Finalize();
  exit(0);
}


/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */

void insert_request(MPI_Request *request) {
  if (__fenix_options.verbose == 16) {
    verbose_print("hash_table_put; request: %lu\n", (long) request);
  }
  __fenix_hash_table_put(outstanding_request, (long) request, request);
}


/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */

void remove_request(MPI_Request *request) {
  if (__fenix_options.verbose == 17) {
    verbose_print("hash_table_remove; request: %lu\n", (long) request);
  }
  __fenix_hash_table_remove(outstanding_request, (long) request);
}


/**
 * @brief
 * @param 
 * @param
 * @param 
 * @param 
 */

void test_MPI(int ret, const char *msg) {
  int ret_repair;
  int index;
  if (ret == MPI_SUCCESS || spare_rank() == 1) {
    return;
  }
  switch (ret) {
    case MPI_ERR_PROC_FAILED:
      MPIF_Comm_revoke(*__fenix_g_world);
      MPIF_Comm_revoke(*__fenix_g_new_world);

      if (__fenix_options.verbose == 20) {
        verbose_print(
                "MPI_ERR_PROC_FAILED; current_rank: %d, role: %d, msg: %s, MPIF_Comm_revoke: %d, MPIF_Comm_revoke: %s\n",
                msg, "world", "new_world");
      }

      if (__fenix_g_replace_comm_flag == 0) {
        MPIF_Comm_revoke(*__fenix_g_user_world);
      }

      for (index = 0; index < outstanding_request->size; index++) {
        if (outstanding_request->table[index].state == OCCUPIED) {
          MPI_Request *value = __fenix_hash_table_get(outstanding_request,
                                              outstanding_request->table[index].key);
          MPI_Status status;
          PMPI_Wait(value, &status);
        }
      }

      for (index = 0; index < outstanding_request->size; index++) {
        if (outstanding_request->table[index].state == OCCUPIED) {
          __fenix_hash_table_remove(outstanding_request, outstanding_request->table[index].key);
        }
      }

      __fenix_g_repair_result = repair_ranks();
      break;
    case MPI_ERR_REVOKED:

      if (__fenix_options.verbose == 20) {
        verbose_print("MPI_ERR_REVOKED; current_rank: %d, role: %d, msg: %s\n", msg);
      }

      for (index = 0; index < outstanding_request->size; index++) {
        if (outstanding_request->table[index].state == OCCUPIED) {
          MPI_Request *value = __fenix_hash_table_get(outstanding_request,
                                              outstanding_request->table[index].key);
          MPI_Status status;
          PMPI_Wait(value, &status);
        }
      }

      for (index = 0; index < outstanding_request->size; index++) {
        if (outstanding_request->table[index].state == OCCUPIED) {
          __fenix_hash_table_remove(outstanding_request, outstanding_request->table[index].key);
        }
      }

      __fenix_g_repair_result = repair_ranks();
      break;
    case MPI_ERR_INTERN:
      break;
    default:
      break;
#ifdef MPICH
    MPIF_Comm_revoke(*__fenix_g_world);
    MPIF_Comm_revoke(*__fenix_g_new_world);
    //MPIF_Comm_revoke(*__fenix_g_user_world);
    __fenix_g_repair_result = repair_ranks();
#endif
  }

  __fenix_g_role = FENIX_ROLE_SURVIVOR_RANK;
  longjmp(*__fenix_g_recover_environment, 1);
}