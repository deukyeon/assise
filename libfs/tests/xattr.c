#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <unistd.h>
#include <assert.h>

/* int setxattr(const char *path, const char *name, */
/* 	     const void *value, size_t size, int flags); */
/* int lsetxattr(const char *path, const char *name, */
/* 	      const void *value, size_t size, int flags); */
/* int fsetxattr(int fd, const char *name, */
/* 	      const void *value, size_t size, int flags); */

int main ()
{
  const char *path = "/mlfs/xattr-test2\0";
  const char *name = "user.test.name\0";
  const char *name2 = "user.test.name2\0";
  const char *name3 = "user.test.name3\0";
  
  int fd = open (path, O_CREAT|O_WRONLY|O_TRUNC);
  if (fd < 0)
    {
      fprintf (stderr, "failed to open the test file\n");
      exit(-1);
    }
  
  close (fd);

  // Test setxattr
  char value[1000];
  memset(value, 0, 1000);
  int rc = setxattr (path, name, (const void *)value, 1000, 0);
  if (rc < 0)
    {
      fprintf (stderr, "setxattr: %s\n", strerror (errno));
      exit (-1);
    }
  
  char buf[1000] = {1};

  // Test getxattr with size 0 -> return the size of value without modifying the given buffer
  ssize_t s = getxattr (path, name, (void *)buf, 0);

  char test_buf[1000] = {1};
  assert (memcmp (test_buf, buf, 1000) == 0);
  assert (s == 1000);

  printf("Passed setxattr\n");

  // Test getxattr
  s = getxattr (path, name, (void *)buf, 1000);
  if (s < 0)
    {
      fprintf (stderr, "getxattr: %s\n", strerror (errno));
      exit (-1);
    }

  assert (memcmp (value, buf, s) == 0);

  printf("Passed getxattr\n");

  // Test removexattr
  rc = removexattr (path, name);
  if (rc < 0)
    {
      fprintf (stderr, "removexattr: %s\n", strerror (errno));
      exit (-1);
    }

  printf("Passed removexattr\n");

  // Test getxattr -> return a negative
  s = getxattr (path, name, (void *)buf, 1000);

  assert (s < 0);
  
  s = getxattr ("/mlfs/.xattr/invalid_path", name, (void *)buf, 1000);

  assert (s < 0);

  printf("Passed getxattr returns a negative number\n");

  setxattr(path, name, value, 1000, 0);
  setxattr(path, name2, value, 1000, 0);
  setxattr(path, name3, value, 1000, 0);

  char list[4096] = {1};
  ssize_t list_len = listxattr (path, list, 0);

  char test_list[4096] = {1};
  assert (memcmp (test_list, list, 4096) == 0);
  assert (list_len == strlen (name) + strlen (name2) + strlen (name3));

  list_len = listxattr (path, list, list_len);

  assert (list_len > 0);

  printf("Passed listxattr\n");

  return 0;
}
