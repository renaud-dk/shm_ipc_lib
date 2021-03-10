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
/*----------------------------------------------------------- Project Headers */

#include "shm_ipc_lib.h"

/*----------------------------------------------------------------------------*/
/*------------------------------------------------------------------- Defines */

/* #define DEBUG */

/*----------------------------------------------------------------------------*/
/*------------------------------------------------------------ Data structure */

/*----------------------------------------------------------------------------*/
/*------------------------------------------------------- Functions prototype */

/* These functions are for library's internal use */
int sem_creat   (key_t sem_key);
int sem_destroy (key_t sem_key);
int sem_lock    (key_t sem_key);
int sem_unlock  (key_t sem_key);

/*----------------------------------------------------------------------------*/
/*--------------------------------------------------------------- Global data */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------- Functions */
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Name          : shm_creat                                                    *
* Description   : This function creates and map a shared memory to the current *
*                 process context. The function also create a semaphore to     *
*                 synchronize access to the shared memory created.             *
* Argument      : key   The key that will be used to create the shared memory. *
*                 size  Size of the shared memory to create in byte.           *
* Return code   : 0      On success.                                           *
*                 -1     On error errno is set.                                *
\*----------------------------------------------------------------------------*/
extern int shm_creat (unsigned int size, key_t key)
{
  int ret = 0;
  int shmid = 0;

  if( (ret = sem_creat(key + 1)) < 0 )
  {
    printf("<shm_create> Can not creat a semaphore for shm\n");
    printf("<shm_create> errno %d\n", errno);
    return -1;
  }
#ifdef DEBUG
  else
  {
    printf("<shm_create> Got semaphore\n");
  }
#endif

  if((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0)
  {
    printf("<shm_create> Can not create shared memory\n");
    printf("<shm_create> destroying semaphore\n");
    printf("<shm_create> errno %d\n", errno);
    sem_destroy(0);
    return -1;
  }
#ifdef DEBUG
  else
  {
    printf("<shm_create> OK share memory created\n");
  }
#endif

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Fucntion name : shm_read                                                     *
* Description   : This function read the contains of a shared memory.          *
*                 Shared memory is selected with the key passed in argument.   *
* Argument      : data Pointer to the buffer to copy the data.                 *
*                 size Size of data.                                           *
*                 key  The key that will be used to access the shared memory.  *
* Return code   : 0      On success and poiter is affected.                    *
*                 -1     On error errno is set.                                *
\*----------------------------------------------------------------------------*/
extern int shm_read (void *data, unsigned int size, key_t key)
{
  void *ptr;
  int shmid = 0;

  if((sem_lock(key+1)) < 0)
  {
    printf("<shm_read_conf> Can not take control over shared memory\n");
    return -1;
  }

  if((shmid = shmget(key, size, 0666)) < 0)  /* Request shared memory */
  {
    printf("<shm_read_conf> Can not get shared memory\n");
    printf("<shm_read_conf> destroying semaphore\n");
    printf("<shm_read_conf> errno %d\n", errno);
    sem_destroy(key + 1);
    return -1;
  }

#ifdef DEBUG
  printf("<shm_read> shmid %d\n", shmid);
#endif

  if((ptr = shmat(shmid, NULL, 0)) == NULL)  /* Map shared memory */
  {
    printf("<shm_read_conf> Can not map pointer to shared memory\n");
    printf("<shm_read_conf> destroying semaphore & shared memory\n");
    printf("<shm_read_conf> errno %d\n", errno);

    if((sem_unlock(key + 1)) < 0)
    {
      printf("<shm_read_conf> Can not release the semaphore\n");
    }
    return -1;
  }
#ifdef DEBUG
  else
  {
    printf("<shm_read_conf> OK share memory mapped\n");
  }
#endif

  memcpy(data, ptr, size);  /* Get data from shared memory */

  shmdt(ptr);               /* Release shared memory */

  if((sem_unlock(key + 1)) < 0)
  {
    printf("<shm_read_conf> Can not release the semaphore\n");
    return -1;
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Name          : shm_write                                                    *
* Description   : This function writes the contains of data poineter to a      *
*                 shared memory.                                               *
*                 Shared memory is selected with the key passed in argument.   *
* Argument      : data Pointer to the buffer to copy the data.                 *
*                 size Size of data.                                           *
*                 key  The key that will be used to access the shared memory.  *
* Return code   : 0       On success                                           *
*                 -1      On error errno is set                                *
\*----------------------------------------------------------------------------*/
extern int shm_write (void *data, unsigned int size, key_t key)
{
  void *ptr;
  int shmid = 0;

  if((sem_lock(key + 1)) < 0)
  {
    printf("<shm_write_conf> Can not take control over shared memory\n");
    return -1;
  }

  if((shmid = shmget(key, size, 0666)) < 0)  /* Request shared memory */
  {
    printf("<shm_write_conf> Can not create shared memory\n");
    printf("<shm_write_conf> destroying semaphore\n");
    printf("<shm_write_conf> errno %d\n", errno);
    sem_destroy(0);
    return -1;
  }

#ifdef DEBUG
  printf("<shm_write_conf> shm_id %d\n", shmid);
#endif

  if((ptr = shmat(shmid, NULL, 0)) == NULL)  /* Map shared memory */
  {
    printf("<shm_write_conf> Can not map pointer to shared memory\n");
    printf("<shm_write_conf> destroying semaphore & shared memory\n");
    printf("<shm_write_conf> errno %d\n", errno);

    if((sem_unlock(key + 1)) < 0)
    {
      printf("<shm_write_conf> Can not release the semaphore\n");
    }
    return -1;
  }
#ifdef DEBUG
  else
  {
    printf("<shm_write_conf> OK share memory mapped\n");
  }
#endif

  memcpy(ptr, data, size); /* Get data from shared memory */

  shmdt(ptr);              /* Release shared memory */

  if((sem_unlock(key + 1)) < 0)
  {
    printf("<shm_write_conf> Can not release the semaphore\n");
    return -1;
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Fucntion name : shm_destroy                                                  *
* Description   : This function destroys a shared memory and the associated    *
*                 sempahore.                                                   *
* Argument      : size Size of data.                                           *
*                 key  The key that will be used to access the shared memory.  *
* Return code   : 0      On success                                            *
*                 -1     On error errno is set                                 *
\*----------------------------------------------------------------------------*/
extern int shm_destroy (unsigned int size, key_t key)
{
  int err = 0;
  int shmid = 0;
#ifdef DEBUG
  struct shmid_ds buffer;
#endif

  if((sem_destroy(key + 1)) < 0)
  {
    printf("<shm_destroy> Semaphore is not destroied\n");
    err = -1;
  }

  if((shmid = shmget(key, size, 0666)) < 0)
  {
    printf("<shm_destroy> Can not get shared memory\n");
    printf("<shm_destroy> errno %d\n", errno);
    return -1;
  }
  else
  {
#ifdef DEBUG
    if((shmctl(shmid, IPC_STAT, &buffer)) < 0)
    {
      printf("<shm_destroy> Could not get shm stat\n");
    }
    else
    {
      printf("<shm_destroy> shm_nattch : 0x%04X\n",
                                             (unsigned int)(buffer.shm_nattch));
    }
#endif
    if((shmctl(shmid, IPC_RMID, 0) < 0))
    {
      printf("<shm_destroy> Shared memory is not destroied\n");
      err = -1;
    }
  }

  return (err);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Name          : sem_creat                                                    *
* Description   : This function creates semaphore to synchronize access to     *
*                 a shared memory. By default the semaphore is set to access   *
*                 to the shared memory.                                        *
* Argument      : sem_key The key that will be used to create the semaphore.   *
* Return code   : 0       On success                                           *
*                 -1      On error errno is set                                *
\*----------------------------------------------------------------------------*/
int sem_creat (key_t sem_key)
{
  int semid = 0;
  union semun
  {
    int val;
    struct semid_ds *buf;
    unsigned short * array;
  } argument;

  if((semid = semget(sem_key, 1, 0666 | IPC_CREAT)) < 0)
  {
    printf("<sem_creat> Can not get semaphore\n");
    printf("<sem_creat> errno %d\n", errno);
    return -1;
  }

  argument.val = 1;

  if((semctl(semid, 0, SETVAL, argument)) < 0)
  {
    printf("<sem_creat> Can not initialize semaphore to %d\n",argument.val);
    printf("<sem_creat> errno %d\n", errno);
    return -1;
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Fucntion name : sem_destroy                                                  *
* Description   : This function destroys a semaphore that access to the shared *
*                 memory.                                                      *
* Argument      : sem_key The key that will be used to destroy the semaphore.  *
* Return code   : 0      On success                                            *
*                 -1     On error errno is set                                 *
\*----------------------------------------------------------------------------*/
int sem_destroy (key_t sem_key)
{
  int semid = 0;

  if((semid = semget(sem_key, 1, 0666)) < 0)
  {
    printf("<sem_destroy> Can not get semaphore\n");
    printf("<sem_destroy> errno %d\n", errno);
    return -1;
  }

  if((semctl(semid, 0, IPC_RMID, 0)) < 0)
  {
    printf("<sem_destroy> Can not destroy semaphore\n");
    printf("<sem_destroy> errno %d\n", errno);
    return -1;
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Fucntion name : sem_lock                                                     *
* Description   : This function lock a semaphore to the shared memory.         *
* Argument      : sem_key The key of the semaphore to lock.                    *
* Return code   : 0      On success                                            *
*                 -1     On error errno is set                                 *
\*----------------------------------------------------------------------------*/
int sem_lock (key_t sem_key)
{
  struct sembuf op;
  int retval;
  int semid = 0;

  op.sem_num = 0;
  op.sem_op  = -1;
  op.sem_flg = 0;

  if((semid = semget(sem_key, 1, 0666)) < 0)
  {
    printf("<sem_lock> Can not get semaphore\n");
    printf("<sem_lock> errno %d\n", errno);
    return -1;
  }

  /* Try to get control of the semaphore */
  if((retval = semop(semid, &op, 1)) != 0)
  {
    printf("<sem_lock> Can not do operation -1 to semaphore\n");
    printf("<sem_lock> errno %d\n", errno);
    return -1;
  }
#ifdef DEBUG
  else
  {
    printf("<sem_lock> OK got semaphore.\n");
  }
#endif

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Fucntion name : sem_unlock                                                   *
* Description   : This function unlock a semaphore to the shared memory.       *
* Argument      : sem_key The key of the semaphore to unlock.                  *
* Return code   : 0      On success                                            *
*                 -1     On error errno is set                                 *
\*----------------------------------------------------------------------------*/
int sem_unlock (key_t sem_key)
{
  struct sembuf op;
  int retval;
  int semid = 0;

  op.sem_num = 0;
  op.sem_op  = 1;
  op.sem_flg = 0;

  if((semid = semget(sem_key, 1, 0666)) < 0)
  {
    printf("<sem_unlock> Can not release semaphore\n");
    printf("<sem_unlock> errno %d\n", errno);
    return -1;
  }

  /* Try to get control of the semaphore */
  if((retval = semop(semid, &op, 1)) != 0)
  {
    printf("<sem_unlock> Can not do operation +1 to semaphore\n");
    printf("<sem_unlock> errno %d\n", errno);
    return -1;
  }
#ifdef DEBUG
  else
  {
    printf("<sem_unlock> OK semaphore released.\n");
  }
#endif

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Name          : ipc_creat                                                    *
* Description   : This function creates an IPC based on the key passed in      *
*                 argument.                                                    *
* Argument      : ipc_key  The key of the PIC to create.                       *
* Return code   : 0      On success                                            *
*                 -1     On error & errno is set                               *
\*----------------------------------------------------------------------------*/
extern int ipc_creat (key_t ipc_key)
{
  int qid = 0;

  if((qid = msgget(ipc_key, IPC_CREAT | 0666)) == -1)
  {
    printf("<ipc_creat> Can not create IPC\n");
    return -1;
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Name          : read_message                                                 *
* Description   : This function do the low level read message over the IPC.    *
*                 This funtion do a non blocking read on the IPC.              *
* Argument      : ipc_key  The key to access to the IPC.                       *
*                 *qbuf    Buffer to pass the IPC low level call to store data *
*                          read from the IPC.                                  *
*                 type     Type of the message.                                *
* Return code   : 0    if no message                                           *
*                 1    if message                                              *
*                -1    In case of error & errno is set.                        *
\*----------------------------------------------------------------------------*/
extern int read_message(key_t ipc_key, struct mymsgbuf *qbuf, long type)
{
  int ret = 0;
  int qid = 0;

  if((qid = msgget(ipc_key, 0666 )) == -1)
  {
    printf("<read_message> Can not get IPC\n");
    return -1;
  }

  qbuf->mtype = type;
  ret = msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, IPC_NOWAIT);

  /* The absence of message is considered as an error, we must check errno */
  if( ret == -1 && errno == ENOMSG )
  {
    ret = 0;
  }
  else
  {
    /* Check if the call of msgrcv returned no error */
    if( ret == -1 && errno != 0 )
    {
      ret = -1;
    }
    else
    {
      ret = 1;  /* No error there is a valid message in qbuf */
    }
  }

  return (ret);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Name          : write_message                                                *
* Description   : This function do the low level send message over the IPC.    *
* Argument      : ipc_key  The key to access to the IPC.                       *
*                 type     Type of the message.                                *
*                 *text    A pointer to the message to send.                   *
* Return code   : 0      On success                                            *
*                 -1     On error & errno is set                               *
\*----------------------------------------------------------------------------*/
extern int write_message(key_t ipc_key, long type, char *text)
{
  int qid = 0;
  size_t length = 0;
  struct mymsgbuf qbuf;

  if((qid = msgget(ipc_key, 0666 )) == -1)
  {
    printf("<write_message> Can not get IPC\n");
    return -1;
  }

  qbuf.mtype = type;
  memset(qbuf.mtext, 0, sizeof(qbuf.mtext));
  strcpy(qbuf.mtext ,text);
  length = strlen(qbuf.mtext) + 1;

  if((msgsnd(qid, (struct msgbuf *)&qbuf, length, 0)) == -1)
  {
    printf("<write_message> Error on msgsend\n");
    return -1;
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*\
* Creation Date : 31-10-2007                                                   *
* Name          : ipc_destroy                                                  *
* Description   : This function destroy an IPC based on the key passed in      *
*                 argument.                                                    *
* Argument      : ipc_key  The key of the IPC to destroy.                      *
* Return code   : 0      On success                                            *
*                 -1     On error & errno is set                               *
\*----------------------------------------------------------------------------*/
extern int ipc_destroy (key_t ipc_key)
{
  int qid = 0;

  if((qid = msgget(ipc_key, 0666)) == -1)
  {
    printf("<ipc_destroy> Can not get IPC\n");
    return -1;
  }

  if(msgctl(qid, IPC_RMID, 0) < 0)
  {
    printf("<ipc_destroy> Can not remove IPC\n");
    return -1;
  }

  return 0;
}

/*---------------------------------------------- Doxygen documentation sectin */
/*!
 *  \file shm_ipc_lib.c
 *  \brief Shared memory & IPC managment library
 *  \author Renaud De Koninck
 *  \version 1.0
 *  \date 11 October 2007
 */




/*!
 *  \fn int shm_creat (unsigned int size, key_t key)
 *  This function creates and map a shared memory to the current process context.
 *  The function also create a semaphore to synchronize access to the shared
 *  memory created.
 *  \warning This function uses the shared memory key to create the associated 
 *  semaphore by incrementing the shared memory key.
 *  \param size Size of the shared memory to create in byte.
 *  \param key The key that will be used to create the shared memory.
 *  \return
 *	- 0 On success.
 *	- -1 On Faillure & errno contains system error.
 */

/*!
 *  \fn int shm_read (void *data, unsigned int size, key_t key)
 *  This function read the contains of a shared memory. Shared memory is
 *  selected with the key passed in argument.
 *  \param data Pointer to the buffer that will contains the copy of the shared
 *              memory.
 *  \param size Size of the shared memory to read in byte.
 *  \param key  The key that will be used to access the shared memory.
 *  \return
 *	- 0 On success.
 *	- -1 On Faillure & errno contains system error.
 */

/*!
 *  \fn int shm_write (void *data, unsigned int size, key_t key)
 *  This function writes the contains of data poineter to a shared memory.
 *  Shared memory is selected with the key passed in argument.
 *  \param data Pointer to the buffer that contains the data to write in the 
 *              shared memory.
 *  \param size Size of the shared memory to write in byte.
 *  \param key  The key that will be used to access the shared memory.
 *  \return
 *	- 0 On success.
 *	- -1 On Faillure & errno contains system error.
 */

/*!
 *  \fn int shm_destroy (unsigned int size, key_t key)
 *  This function destroys a shared memory and the associated sempahore.
 *  \param size Size of the shared memory to destroy in byte.
 *  \param key  The key that will be used to access the shared memory.
 *  \return
 *	- 0 On success.
 *	- -1 On Faillure & errno contains system error.
 */

/*!
 *  \fn int ipc_creat (key_t ipc_key)
 *  This function creates an IPC based on the key passed in argument.
 *  \param ipc_key The key of the PIC to create.
 *  \return
 *	- 0 On success.
 *	- -1 On Faillure & errno contains system error.
 */

/*!
 *  \fn int read_message (key_t ipc_key, struct mymsgbuf *qbuf, long type)
 *  This function do the low level read message over the IPC. This funtion does
 *  a non blocking read on the IPC.
 *  \param ipc_key  The key to access to the IPC.
 *  \param qbuf     Pointer to the buffer that will received the data from the
 *                  IPC.
 *  \param type     Type of the message to read.
 *  \return
 *	- 0 If no message in the IPC.
 *	- 1 If a valid message is in qbuf buffer.
 *	- -1 On Faillure & errno contains system error.
 */

/*!
 *  \fn int write_message (key_t ipc_key, long type, char *text)
 *  This function do the low level send message over the IPC.
 *  \param ipc_key  The key to access to the IPC.
 *  \param type     Type of the message to send.
 *  \param text     Pointer to the buffer that contains the data to write to 
 *                  the IPC.
 *  \return
 *	- 0 On success.
 *	- -1 On Faillure & errno contains system error.
 */

/*!
 *  \fn int ipc_destroy (key_t ipc_key)
 *  This function destroy an IPC based on the key passed in argument.
 *  \param ipc_key The key of the IPC to destroy.
 *  \return
 *	- 0 On success.
 *	- -1 On Faillure & errno contains system error.
 */
