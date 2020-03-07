/*
  ============================================================================
  Name        : TreasureCaveDb.c
  Author      : Alberto Geniola
  Version     : 0.1A
  Copyright   :
  Description : A simple file-mapped DB library to use with TreasureSpot service.
  ============================================================================
*/

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dbhelper.h"
#include "errno.h"

/**
 * Validates the string passed as parameter. A string is valid if:
 * -> Is shorter than the MAXIMUM USER INPUT
 * -> Does not contain any invalid char, i.e. contains only Number, letters (both
 *    upper and lower cases) and points.
 **/
int is_str_valid(char* str)
{
  int i =0;
  while(i<(MAX_USER_LEN-1))
    {
      if (str[i]=='\0')
        break;
      if (!(str[i]>20 && str[i]<127))
        return 0;
      i++;
    }

  if (str[i]=='\0')
    return 1;
  else
    return 0;
}

/**
 * Reads an integer from the file descriptor. Play ATTENTION: the file descriptor must be valid
 * and opened with read permissions.
 * This function will return 1 if all goes ok, otherwise 0.
 *
 **/
int read_integer(int* dest, int fd)
{
  int count=0;
  while(count<sizeof(int))
    {
      int byteRead=read(fd,((char*)dest)+count,sizeof(int)-count);
      if (byteRead<1)
        {
          //perror("Error during int reading. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteRead;
    }

  if(count==sizeof(int))
    return 1;
  else
    return 0;
}

/**
 * Reads an u integer from the file descriptor. Play ATTENTION: the file descriptor must be valid
 * and opened with read permissions.
 * This function will return 1 if all goes ok, otherwise 0.
 *
 **/
int read_uinteger(unsigned int* dest, int fd)
{
  int count=0;
  while(count<sizeof(unsigned int))
    {
      int byteRead=read(fd,((char*)dest)+count,sizeof(unsigned int)-count);
      if (byteRead<1)
        {
          //perror("Error during int reading. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteRead;
    }

  if(count==sizeof(unsigned int))
    return 1;
  else
    return 0;
}

/**
 * Writes an integer to the file descriptor. Play ATTENTION: the file descriptor must be valid
 * and opened with write permissions.
 * This function will return 1 if all goes ok, otherwise 0.
 *
 **/
int write_integer(int source, int fd)
{
  int count=0;
  while(count<sizeof(int))
    {
      int byteWritten=write(fd,((char*)&source)+count,sizeof(int)-count);
      if (byteWritten<1)
        {
          perror("Error during int writing. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteWritten;
    }

  if(count==sizeof(int))
    return 1;
  else
    return 0;
}

/**
 * Writes a u integer to the file descriptor. Play ATTENTION: the file descriptor must be valid
 * and opened with write permissions.
 * This function will return 1 if all goes ok, otherwise 0.
 *
 **/
int write_uinteger(unsigned int source, int fd)
{
  int count=0;
  while(count<sizeof(unsigned int))
    {
      int byteWritten=write(fd,((char*)&source)+count,sizeof(unsigned int)-count);
      if (byteWritten<1)
        {
          perror("Error during int writing. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteWritten;
    }

  if(count==sizeof(unsigned int))
    return 1;
  else
    return 0;
}

/**
 * Reads a double from the file descriptor. Play ATTENTION: the file descriptor must be valid
 * and opened with read permissions.
 * This function will return 1 if all goes ok, otherwise 0.
 *
 **/
int read_double(double* dest, int fd)
{
  int count=0;
  while(count<sizeof(double))
    {
      int byteRead=read(fd,((char*)dest)+count,sizeof(double)-count);
      if (byteRead<1)
        {
          //perror("Error during int reading. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteRead;
    }

  if(count==sizeof(double))
    return 1;
  else
    return 0;
}

/**
 * Writes a double to the file descriptor. Play ATTENTION: the file descriptor must be valid
 * and opened with write permissions.
 * This function will return 1 if all goes ok, otherwise 0.
 *
 **/
int write_double(double source, int fd)
{
  int count=0;
  while(count<sizeof(double))
    {
      int byteWritten=write(fd,((char*)&source)+count,sizeof(double)-count);
      if (byteWritten<1)
        {
          perror("Error during int writing. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteWritten;
    }

  if(count==sizeof(double))
    return 1;
  else
    return 0;
}

/**
 * Reads a string from the file descriptor long "len" bytes (chars).
 * Pay ATTENTION: the file descriptor must be valid and opened with
 * read permissions.
 * This function will return 1 if all goes ok, otherwise 0 is returned.
 *
 **/
int read_string(char* dest, int len, int fd)
{
  int count=0;
  while(count<len)
    {
      int byteRead=read(fd,(dest+count),len-count);
      if (byteRead<1)
        {
          //perror("Error during int reading. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteRead;
    }

  if(count==len)
    {
      dest[count]='\0';
      return 1;
    }
  else
    return 0;
}

/**
 * Writes a string long "len" bytes (chars) to the file descriptor.
 * Pay ATTENTION: the file descriptor must be valid and opened with
 * write permissions.
 * This function will return 1 if all goes ok, otherwise 0 is returned.
 *
 **/
int write_string(char* source, int len, int fd)
{
  int count=0;
  while(count<len)
    {
      int byteWritten=write(fd,source+count,len-count);
      if (byteWritten<1)
        {
          perror("Error during string writing. The user.dat file is corrupted.");
          return 0;
        }
      count+=byteWritten;
    }

  if(count==len)
    return 1;
  else
    return 0;
}

int get_diamonds(diamond **ptr, int* ndiamonds)
{
  int fd;
  struct flock lock;
  int maxId=0;
  int scanningDiaId=0;
  int scanningDescrLength=0;
  int scanningUserId=0;
  double scanningPrice,scanningSize;
  char scanningDescr[MAX_USER_LEN];
  int userDiaCount=0;
  unsigned int t;

  // If it is correct
  // Open the USER file in READ and WRITE,
  fd = open(DIAMONDS_DB_PATH,O_RDWR|O_CREAT,S_IRWXU);
  if (fd<0)
    {
      // Error occurred
      return DB_OPEN_ERROR;
    }

  // Lock for writing
  memset (&lock, 0, sizeof(lock));
  lock.l_type = F_WRLCK;
  fcntl (fd, F_SETLKW, &lock);

  // Scan each row, it is needed to increment the ID
  do {
    // Read the Timestamp (integer)
    if (!read_uinteger(&t,fd))
      {
        // This has failed, because there are no more lines. So break
        break;
      }
    // Read the id (integer)
    if (!read_integer(&scanningDiaId,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);
        // DB file corrupted
        return DB_ERROR;
      }
    // Read the dimension of the description
    if (!read_integer(&scanningDescrLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);
        // DB file corrupted
        return DB_ERROR;
      }

    // Read the user id
    if (!read_integer(&scanningUserId,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);
        // DB file corrupted
        return DB_ERROR;
      }

    // Read the price
    if (!read_double(&scanningPrice,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);
        // DB file corrupted
        return DB_ERROR;
      }

    // Read the size
    if (!read_double(&scanningSize,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);
        // DB file corrupted
        return DB_ERROR;
      }

    // Read the description
    if (!read_string(scanningDescr,scanningDescrLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);
        // DB file corrupted
        return DB_ERROR;
      }
    *ptr = realloc(*ptr, ++(*ndiamonds) * sizeof(diamond));
    (*ptr)[(*ndiamonds) - 1].insert = NULL;
    (*ptr)[(*ndiamonds) - 1].descriptionLen = scanningDescrLength;
    strncpy((*ptr)[(*ndiamonds) - 1].descr, scanningDescr, strnlen(scanningDescr, scanningDescrLength));
    (*ptr)[(*ndiamonds) - 1].size = scanningSize;
    (*ptr)[(*ndiamonds) - 1].price = scanningPrice;
    (*ptr)[*ndiamonds - 1].userId = scanningUserId;
    (*ptr)[*ndiamonds - 1].id = scanningDiaId;
    (*ptr)[*ndiamonds - 1].timestamp = t;
  }
  while (1);
  return RESULT_OK;
}

/**
 * This function will insert an user into the database. If the user.dat file doesn't exist
 * a new one will be created. An integer >0 is returned which is the user_id, otherwise a negative
 * number matching the DB_RESULT codes will be returned.
 * **/
int add_user(char* username, char* password)
{
  int fd;
  struct flock lock;
  char scanningUser[MAX_USER_LEN];
  char scanningPass[MAX_USER_LEN];
  int scanningUserLength;
  int scanningPassLength;
  int scanningUserId=0;
  int maxId=0;
  unsigned int t;
  int userCount = 0;

  // Check validity of username and password
  if (!is_str_valid(username) || ! is_str_valid(password))
    return STRING_WRONG_FORMAT;


  // If they are correct
  // Open the USER file in READ and WRITE,
  fd = open(USER_DB_PATH,O_RDWR|O_CREAT,S_IRWXU);
  if (fd<0)
    {
      // Error occurred
      perror("Error opening DB file.");
      return DB_OPEN_ERROR;
    }

  // Lock for writing
  memset (&lock, 0, sizeof(lock));
  lock.l_type = F_WRLCK;
  fcntl (fd, F_SETLKW, &lock);

  // Scan each row: if there is any equal username, unlock, close and return error
  do {

    // Check maximum users limit
    if (userCount > MAX_USER_LIMIT)
      {
        // Nice try!
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        perror("Too many users. Limit reached.");
        return DB_ERROR_TOO_MANY_USERS;
      }

    // Read the Timestamp
    //FIXME: read_unsigned_integer
    if (!read_uinteger(&t,fd))
      {
        // This has failed, because there are no more lines. So break
        break;
      }


    // Read the User Id
    if (!read_integer(&scanningUserId,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read userid");
        return DB_ERROR;

      }

    // Read the username length
    if (!read_integer(&scanningUserLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read username len");
        return DB_ERROR;
      }

    // Read the password length
    if (!read_integer(&scanningPassLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read pass len");
        return DB_ERROR;
      }

    // Read the username
    if (!read_string(scanningUser,scanningUserLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read the first string (username).");
        return DB_ERROR;
      }

    // Read the password
    if (!read_string(scanningPass,scanningPassLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read the first string (pass).");
        return DB_ERROR;
      }

    // Find the maximum user id
    if (scanningUserId>maxId)
      maxId=scanningUserId;

    //fprintf(stderr,"\nAddUser:\nCurrentId:%d\nUserLen:%d\nPassLen:%d\nUser:%s\nPass:%s\n\n",scanningUserId,scanningUserLength,scanningPassLength,scanningUser,scanningPass);

    userCount++;

  }
  while(1);

  // If no row contains the same username, get the last id, and insert the new user
  // Get current time and write it
  t = time(NULL);
  // Write the timestamp
  if(!write_integer(t,fd))
    {perror("0");}
  // Write the user id
  if(!write_integer(maxId+1,fd))
    {perror("1");}
  // Write the length of username
  if(!write_integer(strnlen(username,MAX_USER_LEN),fd))
    {perror("2");}
  // Write the length of password
  if(!write_integer(strnlen(password,MAX_USER_LEN),fd))
    {perror("3");}
  // Write the username
  if(!write_string(username, strnlen(username,MAX_USER_LEN),fd))
    {perror("4");}
  // Write the password
  if(!write_string(password, strnlen(password,MAX_USER_LEN),fd))
    {perror("5");}
  // Then unlock the file, close the file.
  lock.l_type = F_UNLCK;
  fcntl (fd, F_SETLKW, &lock);
  close(fd);

  //fprintf(stderr,"\n-> Added User %s with Id %d.",username,(maxId+1));

  return maxId+1;
}

/**
 * This function will scan the user.dat file and search for a row with the giver USER-PASS pair.
 * If no one is found, DB_NO_RESULT is returned, meaning the user has typed wrong credentials.
 * If the login is successful, the user's id is returned (>=0).
 * **/
int check_cred(int uid, char* password, char** username)
{
  int fd;
  struct flock lock;
  char scanningUser[MAX_USER_LEN];
  char scanningPass[MAX_USER_LEN];
  int scanningUserLength;
  int scanningPassLength;
  int scanningUserId;
  unsigned int t;

  *username = NULL;
  // Check validity of username and password
  if (!is_str_valid(password))
    return STRING_WRONG_FORMAT;

  // If they are correct
  // Open the USER file in READ and WRITE,
  fd = open(USER_DB_PATH,O_RDWR|O_CREAT,S_IRWXU);
  if (fd<0)
    {
      // Error occurred
      perror("Error opening DB file.");
      return DB_OPEN_ERROR;
    }

  // Lock for reading
  memset (&lock, 0, sizeof(lock));
  lock.l_type = F_RDLCK;
  fcntl (fd, F_SETLKW, &lock);

  // Scan each row: if there is any equal username, unlock, close and return error
  do {


    // Read timestamp
    if (!read_uinteger(&t,fd))
      {
        // This has failed, because there are no more lines. So break
        break;
      }

    // Read the User Id
    if (!read_integer(&scanningUserId,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read user id");
        return DB_ERROR;
      }

    // Read the username length
    if (!read_integer(&scanningUserLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read username len");
        return DB_ERROR;
      }

    // Read the password length
    if (!read_integer(&scanningPassLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read password len");
        return DB_ERROR;
      }

    // Read the username
    if (!read_string(scanningUser,scanningUserLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read the first string (username).");
        return DB_ERROR;
      }

    // Read the password
    if (!read_string(scanningPass,scanningPassLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read the first string (password).");
        return DB_ERROR;
      }

    // Check if credentials are ok
    if((scanningUserId == uid) && strncmp(password,scanningPass,MAX_USER_LEN)==0)
      {
        // Login OK
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);
        if ((*username = (char*) malloc((strlen(scanningUser) + 1) * sizeof(char))) == NULL) {
          return DB_ERROR;
        }
        sprintf(*username, "%s", scanningUser);
        return RESULT_OK;
      }
  }
  while(1);

  // If you reach this statement, you haven't found the user-pass pair.
  // So unlock the file, close the file and return the error
  lock.l_type = F_UNLCK;
  fcntl (fd, F_SETLKW, &lock);
  close(fd);

  perror("Username ID and password invalid.");
  return DB_NO_RESULT;
}

/**
 * This function will list up to MAX_DIAMONDS_PER_USER for a given userId.
 * The caller must pass a pointer buf, in which will be saved the pointer
 * to the array of read diamonds (allocated on the HEAP).
 * The function will return the number of read diamonds, or a negative number
 * if something has gone wrong. You can check the error CODE by using DB_RESULT
 * enum type.
 * Also, the caller must free that buf when he's done with using that data.
 **/
int list_diamonds(int user_id, diamond** buf)
{
  int fd;
  struct flock lock;
  int scanningDescrLength;
  // Temp buffer for diamonds reading
  diamond tmp[MAX_DIAMONDS_PER_USER];
  int diamondCount=0;

  // If they are correct
  // Open the USER file in READ and WRITE,
  fd = open(DIAMONDS_DB_PATH,O_RDWR|O_CREAT,S_IRWXU);
  if (fd<0)
    {
      // Error occurred
      perror("Error opening DB file.");
      return DB_OPEN_ERROR;
    }

  // Lock for reading
  memset (&lock, 0, sizeof(lock));
  lock.l_type = F_RDLCK;
  fcntl (fd, F_SETLKW, &lock);

  do {
    // Read the Timstamp
    if (!read_integer(&tmp[diamondCount].timestamp,fd))
      {
        // This has failed, because there are no more lines. So break
        break;
      }

    // Read the id
    if (!read_integer(&tmp[diamondCount].id,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The diamonds.dat file contains an invalid row: I was unable to read id integer");
        return DB_ERROR;
      }

    // Read the description length
    if (!read_integer(&scanningDescrLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The diamonds.dat file contains an invalid row: I was unable to read desc len");
        return DB_ERROR;
      }
    // Read the userId
    if (!read_integer(&tmp[diamondCount].userId,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The diamonds.dat file contains an invalid row: I was unable to read userid");
        return DB_ERROR;
      }

    // If the user ID is the same of the userid passed as parameter, write to the struct. Otherwise SEEK the file and continue without incrementing the counter
    if (tmp[diamondCount].userId!=user_id)
      {
        // I have to seek of sizeof(double)*2 + sizeof(char)*descrLen
        lseek(fd,sizeof(double)*2 + sizeof(char)*scanningDescrLength,SEEK_CUR);
        continue;
      }

    // Read the price
    if (!read_double(&tmp[diamondCount].price,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the size
    if (!read_double(&tmp[diamondCount].size,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the description
    tmp[diamondCount].descr[0]='\0';
    if (!read_string(tmp[diamondCount].descr,scanningDescrLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        perror("The diamonds.dat file contains an invalid row: I was unable to read the diamonds description.");
        return DB_ERROR;
      }

    ++diamondCount;
  }
  while(diamondCount<MAX_DIAMONDS_PER_USER);

  lock.l_type = F_UNLCK;
  fcntl (fd, F_SETLKW, &lock);
  close(fd);

  // Now alloc enough memory and return a pointer to the caller
  *buf = NULL;
  if (diamondCount > 0) {
    if ((*buf = (diamond*)malloc(sizeof(diamond)*diamondCount)) == NULL) {
      return DB_ERROR;
    }
    memcpy(*buf,tmp,sizeof(diamond)*diamondCount);
  }
  return diamondCount;
}

int insert_diamond(int user_id ,double price, double size, char* description)
{
  int fd;
  struct flock lock;
  int maxId=0;
  int scanningDiaId=0;
  int scanningDescrLength=0;
  int scanningUserId=0;
  double scanningPrice,scanningSize;
  char scanningDescr[MAX_USER_LEN];
  int userDiaCount=0;
  unsigned int t;

  // Check validity of description
  if (!is_str_valid(description))
    return STRING_WRONG_FORMAT;

  // If it is correct
  // Open the USER file in READ and WRITE,
  fd = open(DIAMONDS_DB_PATH,O_RDWR|O_CREAT,S_IRWXU);
  if (fd<0)
    {
      // Error occurred
      return DB_OPEN_ERROR;
    }

  // Lock for writing
  memset (&lock, 0, sizeof(lock));
  lock.l_type = F_WRLCK;
  fcntl (fd, F_SETLKW, &lock);

  // Scan each row, it is needed to increment the ID
  do {
    // Read the Timestamp (integer)
    if (!read_uinteger(&t,fd))
      {
        // This has failed, because there are no more lines. So break
        break;
      }
    // Read the id (integer)
    if (!read_integer(&scanningDiaId,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }
    // Read the dimension of the description
    if (!read_integer(&scanningDescrLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the user id
    if (!read_integer(&scanningUserId,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the price
    if (!read_double(&scanningPrice,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the size
    if (!read_double(&scanningSize,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the description
    if (!read_string(scanningDescr,scanningDescrLength,fd))
      {
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        // DB file corrupted
        return DB_ERROR;
      }

    // Find the maximum user id
    if (scanningDiaId>maxId)
      maxId=scanningDiaId;

    // Check if the user already has reached maximum amount of allowed diamonds
    if (user_id==scanningUserId)
      ++userDiaCount;

    // Check there areno equals rows
    if ((user_id==scanningUserId) && (scanningSize==size) && (scanningPrice==price) && (strncmp(scanningDescr,description,MAX_USER_LEN)==0))
      {
        // This is a duplicate!
        // Unlock and close the file
        lock.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock);
        close(fd);

        return DB_ERROR_DIAMOND_EXISTS;
      }
  }
  while(userDiaCount<MAX_DIAMONDS_PER_USER);

  // Write only if you are not at the maximum capacity
  if (userDiaCount>=MAX_DIAMONDS_PER_USER)
    {
      lock.l_type = F_UNLCK;
      fcntl (fd, F_SETLKW, &lock);
      close(fd);

      return DB_ERROR_TOO_MANY_DIAMONDS;
    }
  // Get the timestamp
  t = time(NULL);
  // Write the timestamp
  if(!write_integer(t,fd))
    {perror("0");}
  if(!write_integer(maxId+1,fd))
    {perror("1");}
  // Write the length of description
  if(!write_integer(strnlen(description,MAX_USER_LEN),fd))
    {perror("2");}
  // Write the user_id
  if(!write_integer(user_id,fd))
    {perror("3");}
  // Write the price
  if(!write_double(price,fd))
    {perror("4");}
  // Write the size
  if(!write_double(size, fd))
    {perror("5");}
  // Write the description
  if(!write_string(description,strnlen(description,MAX_USER_LEN), fd))
    {perror("5");}

  // Then unlock the file, close the file.
  lock.l_type = F_UNLCK;
  fcntl (fd, F_SETLKW, &lock);
  close(fd);

  return RESULT_OK;
}

int db_clean()
{
  int user_fd;
  struct flock user_lock;
  int user_fd_bak;
  int dia_fd;
  int dia_fd_bak;
  struct flock diamond_lock;
  char buff[sizeof(char)*10240]; // 10Kb buffer
  USER tmp_user;
  diamond tmp_diamond;
  int saved_users[MAX_USER_LIMIT];
  int good_user_count=0;
  int diamondCount=0;
  unsigned int now;
  int i=0;
  int good=0;
  long dia_file_len=0;
  long user_file_len=0;
  size_t byte_read=0;
  size_t byte_written=0;

  now=time(NULL);
  fprintf(stderr,"\n ***** Cleaning routines started at %u. ***** \n",now);

  // Open both user and diamond files
  // Acquire locks on both of them
  user_fd = open(USER_DB_PATH,O_RDWR|O_CREAT,S_IRWXU);
  if (user_fd<0)
    {
      // Error occurred
      return DB_OPEN_ERROR;
    }

  dia_fd = open(DIAMONDS_DB_PATH,O_RDWR|O_CREAT,S_IRWXU);
  if (dia_fd<0)
    {
      // Error occurred
      return DB_OPEN_ERROR;
    }

  // Copy both the file
  dia_fd_bak = open(DIAMONDS_DB_PATH_BAK,O_RDWR|O_CREAT,S_IRWXU);
  if (dia_fd_bak<0)
    {
      // Error occurred
      return DB_OPEN_ERROR;
    }

  user_fd_bak = open(USER_DB_PATH_BAK,O_RDWR|O_CREAT,S_IRWXU);
  if (user_fd_bak<0)
    {
      // Error occurred
      perror("Error opening USER BAK DB file.");
      return DB_OPEN_ERROR;
    }

  memset (&user_lock, 0, sizeof(user_lock));
  user_lock.l_type = F_WRLCK;
  memset (&diamond_lock, 0, sizeof(diamond_lock));
  diamond_lock.l_type = F_WRLCK;

  // Get the lock on both of them
  fcntl (user_fd, F_SETLKW, &user_lock);
  fcntl (dia_fd, F_SETLKW, &diamond_lock);

  /*----- COPING USER DB FILE-----*/
  //fprintf(stderr,"Copying %s -> %s\n", USER_DB_PATH,USER_DB_PATH_BAK);
  while ((byte_read=read(user_fd,buff,sizeof(buff)))>0)
    {
      byte_written=0;
      //fprintf(stderr,"->Read buffer of %u bytes\n", byte_read);
      while(byte_written<byte_read)
        {
          i=write(user_fd_bak,buff+byte_written,byte_read-byte_written);
          if (i<1)
            {
              perror("I was unable to copty USER.DAT file");
              user_lock.l_type = F_UNLCK;
              diamond_lock.l_type = F_UNLCK;
              fcntl (user_fd, F_SETLKW, &user_lock);
              fcntl (dia_fd, F_SETLKW, &diamond_lock);
              close(user_fd_bak);
              close(dia_fd_bak);
              close(user_fd);
              close(dia_fd);
              unlink(USER_DB_PATH_BAK);
              unlink(DIAMONDS_DB_PATH_BAK);
              return DB_ERROR;

            }
          byte_written+=i;
        }
    }

  /*----- COPING DIAMOND DB FILE-----*/
  //fprintf(stderr,"Copying %s -> %s\n", DIAMONDS_DB_PATH,DIAMONDS_DB_PATH_BAK);
  while ((byte_read=read(dia_fd,buff,sizeof(buff)))>0)
    {
      byte_written=0;
      //fprintf(stderr,"->Read buffer of %u bytes\n", byte_read);
      while(byte_written<byte_read)
        {
          i+=write(dia_fd_bak,buff+byte_written,byte_read-byte_written);
          if (i<1)
            {
              user_lock.l_type = F_UNLCK;
              diamond_lock.l_type = F_UNLCK;
              fcntl (user_fd, F_SETLKW, &user_lock);
              fcntl (dia_fd, F_SETLKW, &diamond_lock);
              close(user_fd_bak);
              close(dia_fd_bak);
              close(user_fd);
              close(dia_fd);
              unlink(USER_DB_PATH_BAK);
              unlink(DIAMONDS_DB_PATH_BAK);
              return DB_ERROR;
            }
          byte_written+=i;
          //fprintf(stderr,"\n-- Written %u bytes\n", byte_written);
        }
    }
  //fprintf(stderr,"Copied %u bytes\n", byte_written);

  // Reset FD pointer
  lseek(user_fd,0,SEEK_SET);
  lseek(user_fd_bak,0,SEEK_SET);
  lseek(dia_fd,0,SEEK_SET);
  lseek(dia_fd_bak,0,SEEK_SET);

  // >>>>>>>>>>>>>>>>>>>> Users cleaning <<<<<<<<<<<<<<<<<<<<
  good_user_count=0;
  // For each line you read in users bak file, if the timestamp is older than the limit, do not copy the record in the new file. Remember the good users.
  do {
    // Read timestamp
    if (!read_uinteger(&tmp_user.ts,user_fd_bak))
      {
        // This has failed, because there are no more lines. So break
        break;
      }

    // Read the User Id
    if (!read_integer(&tmp_user.user_id,user_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read user id");
        return DB_ERROR;
      }

    // Read the username length
    if (!read_integer(&tmp_user.uname_len,user_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read username len");
        return DB_ERROR;
      }

    // Read the password length
    if (!read_integer(&tmp_user.pass_len,user_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read password len");
        return DB_ERROR;
      }

    // Read the username
    if (!read_string(tmp_user.username,tmp_user.uname_len,user_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read the first string (username).");
        return DB_ERROR;
      }

    // Read the password
    if (!read_string(tmp_user.pass,tmp_user.pass_len,user_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        perror("The user.dat file contains an invalid row: I was unable to read the first string (password).");
        return DB_ERROR;
      }

    // Check if timestamp is older than the limit
    // If yes, write it to the user_fd file directly
    if((now-tmp_user.ts)<DATA_VALIDITY_INTERVAL)
      {
        //fprintf(stderr,"\n--- Found 1 good record to preserve:\nTIMESTAMP:%u\nUSERID: %d\nLEN USER: %d\nLEN PASS: %d\nUsername: %s \n Password: %s",tmp_user.ts,tmp_user.user_id,tmp_user.uname_len,tmp_user.pass_len,tmp_user.username, tmp_user.pass);

        // Write the timestamp
        if(!write_integer(tmp_user.ts,user_fd))
          {perror("0");}
        // Write the user id
        if(!write_integer(tmp_user.user_id,user_fd))
          {perror("1");}
        // Write the length of username
        if(!write_integer(tmp_user.uname_len,user_fd))
          {perror("2");}
        // Write the length of password
        if(!write_integer(tmp_user.pass_len,user_fd))
          {perror("3");}
        // Write the username
        if(!write_string(tmp_user.username,tmp_user.uname_len,user_fd))
          {perror("4");}
        // Write the password
        if(!write_string(tmp_user.pass, tmp_user.pass_len,user_fd))
          {perror("5");}
        user_file_len+=sizeof(int)*4+tmp_user.uname_len*sizeof(char)+tmp_user.pass_len*sizeof(char);

        saved_users[good_user_count]=tmp_user.user_id;
        ++good_user_count;
      }
  }
  while(1);

  //fprintf(stderr,"\nSaved %d records to USER.DAT.",good_user_count);

  // >>>>>>>>>>>>>>>>>>>> cleaning <<<<<<<<<<<<<<<<<<<<
  // Same policy for diamonds: delete all the diamonds matching a trashed user
  // Scan the diamond file, save only diamonds matching good user ids, skip the others
  diamondCount=0;
  do {
    // Read the Timsetamp
    if (!read_integer(&tmp_diamond.timestamp,dia_fd_bak))
      {
        // This has failed, because there are no more lines. So break
        break;
      }

    // Read the id
    if (!read_integer(&tmp_diamond.id,dia_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the description length
    if (!read_integer(&tmp_diamond.descriptionLen,dia_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the userId
    if (!read_integer(&tmp_diamond.userId,dia_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        return DB_ERROR;
      }

    // If the user ID is the same of the userid is marked as valid, then procced with reading other stuff. Otherwise,
    // simply seek ahead without incrementing the counter
    good=0;
    for (i=0;i<good_user_count;i++)
      {
        if (saved_users[i]==tmp_diamond.userId)
          {
            // Found! it is good and must be kept
            good=1;
          }
      }
    if (good==0)
      {
        // I have to seek of sizeof(double)*2 + sizeof(char)*descrLen
        lseek(dia_fd_bak,sizeof(double)*2 + sizeof(char)*tmp_diamond.descriptionLen,SEEK_CUR);
        continue;
      }

    // Read the price
    if (!read_double(&tmp_diamond.price,dia_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        // DB file corrupted
        return DB_ERROR;
      }

    // Read the size
    if (!read_double(&tmp_diamond.size,dia_fd_bak))
      {
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd); // DB file corrupted
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        return DB_ERROR;
      }

    // Read the description
    tmp_diamond.descr[0]='\0';
    if (!read_string(tmp_diamond.descr,tmp_diamond.descriptionLen,dia_fd_bak))
      {
        // DB file corrupted
        // Unlock and close the file
        user_lock.l_type = F_UNLCK;
        diamond_lock.l_type = F_UNLCK;
        fcntl (user_fd, F_SETLKW, &user_lock);
        fcntl (dia_fd, F_SETLKW, &diamond_lock);
        close(user_fd_bak);
        close(dia_fd_bak);
        close(user_fd);
        close(dia_fd);
        unlink(USER_DB_PATH_BAK);
        unlink(DIAMONDS_DB_PATH_BAK);

        return DB_ERROR;
      }

    // Write to the diamond db
    // Write the timestamp
    if(!write_integer(tmp_diamond.timestamp,dia_fd))
      {perror("0");}
    // Write the id
    if(!write_integer(tmp_diamond.id,dia_fd))
      {perror("1");}
    // Write the length of description
    if(!write_integer(tmp_diamond.descriptionLen,dia_fd))
      {perror("2");}
    // Write the user_id
    if(!write_integer(tmp_diamond.userId,dia_fd))
      {perror("3");}
    // Write the price
    if(!write_double(tmp_diamond.price,dia_fd))
      {perror("4");}
    // Write the size
    if(!write_double(tmp_diamond.size, dia_fd))
      {perror("5");}
    // Write the description
    if(!write_string(tmp_diamond.descr,tmp_diamond.descriptionLen, dia_fd))
      {perror("5");}
    dia_file_len+=sizeof(int)*4+sizeof(double)*2+sizeof(char)*tmp_diamond.descriptionLen;

    ++diamondCount;
  }
  while(1);

  ftruncate(dia_fd,dia_file_len);
  ftruncate(user_fd,user_file_len);

  // Release both the locks
  // close both the files
  user_lock.l_type = F_UNLCK;
  diamond_lock.l_type = F_UNLCK;
  fcntl (user_fd, F_SETLKW, &user_lock);
  fcntl (dia_fd, F_SETLKW, &diamond_lock);
  close(user_fd_bak);
  close(dia_fd_bak);
  close(user_fd);
  close(dia_fd);

  i = unlink(USER_DB_PATH_BAK);
  if (i!=0)
    perror("It was impossible to delete the USER.DAT.BAK file");


  i = unlink(DIAMONDS_DB_PATH_BAK);
  if (i!=0)
    perror("It was impossible to delete the USER.DAT.BAK file");

  now = time(NULL);
  fprintf(stderr,"\n ***** Cleaning routines ended at %u. ***** \n",now);

  return RESULT_OK;

}
