#include "xattr.h"

#include "fs.h"

#include <dirent.h>

// TODO: The current implementation of xattr uses a file to persist attributes

#define XATTR_MAX_NAME_SIZE 2048  ///< The maximum size of attribute name
#define XATTR_MAX_VALUE_SIZE 2048 ///< The maximum size of attribute value

#define XATTR_ROOTDIR "/mlfs/.xattr"

static inline void get_xattr_path (char *buf, int inum, const char *name)
{
  if (name)
    sprintf (buf, "%s/%d/%s\0", XATTR_ROOTDIR, inum, name);
  else
    sprintf (buf, "%s/%d\0", XATTR_ROOTDIR, inum);
}

static inline int create_rootdir ()
{
  /*
  int rootfd = open (XATTR_ROOTDIR, O_DIRECTORY, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (rootfd < 0)
      return -1;

  close (rootfd);
  return 0;
  */
  mkdir (XATTR_ROOTDIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  return 0;
}

// TODO: handle a variety of errors
int setxattri(struct inode *ip, const char *name, const void *value,
	      size_t size, int flags) {
  int i;

  if (create_rootdir () == -1)
    return -1;
  
  ilock(ip);
  
  char xattr_path[1024];
  get_xattr_path (xattr_path, ip->inum, NULL);
  mlfs_debug ("xattr path: %s\n", xattr_path);

  int status = mkdir (xattr_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (status < 0 && errno != EEXIST)
    {
      iunlock (ip);
      return -1;
    }
  
  get_xattr_path (xattr_path, ip->inum, name);
  mlfs_debug ("xattr path: %s\n", xattr_path);

  int fd;
  if (flags == XATTR_REPLACE)
    {
      fd = open(xattr_path, O_WRONLY|O_TRUNC);
    }
  else
    {
      fd = open(xattr_path, O_CREAT|O_WRONLY|O_TRUNC);
    }
  
  if (fd < 0)
    {
      errno = ENODATA;
      iunlock (ip);
      return -1;
    }

  if (flags == XATTR_CREATE)
    {
      errno = EEXIST;
      close (fd);
      iunlock (ip);
      return -1;
    }
  
  write (fd, value, size);

  close (fd);
  iunlock(ip);
  return 0;
}

// TODO: Handle various errors
ssize_t getxattri(struct inode *ip, const char *name, void *value, size_t size)
{
  if (size > XATTR_MAX_VALUE_SIZE)
    {
      errno = ERANGE;
      return -1;
    }
  
  if (create_rootdir () == -1)
    return -1;

  ilock(ip);

  char xattr_path[1024];
  get_xattr_path (xattr_path, ip->inum, NULL);

  struct stat st;
  if (stat(xattr_path, &st) != 0 || !S_ISDIR(st.st_mode))
    {
      errno = ENODATA;
      iunlock (ip);
      return -1;
    }
  
  get_xattr_path (xattr_path, ip->inum, name);

  int fd = open (xattr_path, O_RDONLY);
  if (fd < 0)
    {
      errno = ENODATA;
      iunlock (ip);
      return -1;
    }

  ssize_t value_len = -1;

  if (size == 0)
    {
      char buf[XATTR_MAX_VALUE_SIZE];
      value_len = read (fd, buf, XATTR_MAX_VALUE_SIZE);
      goto out;
    }

  value_len = read (fd, value, size);

 out:
  close (fd);
  iunlock (ip);
  return value_len;
}

ssize_t _getxattri (struct inode *ip, const char *name, void *value, size_t size)
{
  if (size > XATTR_MAX_VALUE_SIZE)
    {
      errno = ERANGE;
      return -1;
    }
  
  char xattr_path[1024];
  get_xattr_path (xattr_path, ip->inum, name);

  int fd = open (xattr_path, O_RDONLY);
  if (fd < 0)
    {
      errno = ENODATA;
      return -1;
    }

  ssize_t value_len = -1;

  if (size == 0)
    {
      char buf[XATTR_MAX_VALUE_SIZE];
      value_len = read (fd, buf, XATTR_MAX_VALUE_SIZE);
      goto out;
    }

  value_len = read (fd, value, size);

 out:
  close (fd);
  return value_len;
}

int removexattri(struct inode *ip, const char *name)
{
  if (create_rootdir () == -1)
    return -1;
  
  ilock(ip);

  char xattr_path[1024];
  get_xattr_path (xattr_path, ip->inum, name);

  struct stat st;
  int ret = stat (xattr_path, &st);
  if (ret < 0)
    {
      errno = ENODATA;
      iunlock(ip);
      return -1;
    }

  ret = unlink (xattr_path);
  if (ret < 0)
    {
      iunlock (ip);
      return -1;
    }

  iunlock (ip);
  return 0;
}

ssize_t listxattri(struct inode *ip, char *list, size_t size)
{
  if (create_rootdir () == -1)
    return -1;
  
  ilock(ip);

  DIR *d;
  struct dirent *dir;

  char xattr_path[1024];
  get_xattr_path (xattr_path, ip->inum, NULL);

  ssize_t list_size = 0;


  d = opendir (xattr_path);
  if (d)
    {
      while ((dir = readdir (d)) != NULL)
	{
	  if (memcmp (dir->d_name, ".", 1) == 0)
	    continue;
	  
	  list_size += strlen(dir->d_name);
	}

      closedir (d);
    }

  if (size == 0)
    {
      iunlock (ip);
      return list_size;
    }

  if (list_size > size)
    {
      errno = ERANGE;
      iunlock (ip);
      return -1;
    }

  list_size = 0;
  
  d = opendir (xattr_path);
  if (d)
    {
      while ((dir = readdir (d)) != NULL)
	{
	  // FIXME: If the name starts with '.', it ignores the attribute
	  if (memcmp (dir->d_name, ".", 1) == 0)
	    continue;
	  
	  memmove (list + list_size, dir->d_name, strlen(dir->d_name));
	  list_size += strlen(dir->d_name);
	}

      closedir (d);
    }

  iunlock(ip);
  return list_size;
}

