/* This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/
/*---------------------------------------------------------- Standard Headers */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------- Project Headers */

/*----------------------------------------------------------------------------*/
/*------------------------------------------------------------------- Defines */

#define MAX_SEND_SIZE 512 /* Length max of a write to an IPC */

/*----------------------------------------------------------------------------*/
/*------------------------------------------------------------ Data structure */

/* Data structure for read IPC */
struct mymsgbuf
{
  long mtype;
  char mtext[MAX_SEND_SIZE];
};

/*----------------------------------------------------------------------------*/
/*------------------------------------------------------ Functions prototypes */

extern int shm_creat     (unsigned int size, key_t key);
extern int shm_read      (void *data, unsigned int size, key_t key);
extern int shm_write     (void *data, unsigned int size, key_t key);
extern int shm_destroy   (unsigned int size, key_t key);
extern int ipc_creat     (key_t ipc_key);
extern int read_message  (key_t ipc_key, struct mymsgbuf *qbuf, long type);
extern int write_message (key_t ipc_key, long type, char *text);
extern int ipc_destroy   (key_t ipc_key);
