#ifndef __XATTR_H__
#define __XATTR_H__

#include "shared.h"

#ifdef __cplusplus
extern "C" {
#endif
  
#define XATTR_CREATE_OR_REPLACE 0
#define XATTR_CREATE 1
#define XATTR_REPLACE 2

/**
 * Set an attribute
 * param[in] inode 
 * param[in] name
 * param[in] value
 * param[in] value length
 * param[in] flag
 * param[out] Return 0 if succeed, otherwise, return -1
 */
int setxattri(struct inode *, const char *, const void *, size_t, int);


/**
 * Get an attribute
 * param[in] inode 
 * param[in] name
 * param[in] value
 * param[in] value buffer length
 * param[out] the size of value
 */
ssize_t getxattri(struct inode *, const char *, void *, size_t);

/**
 * Remove an attribute
 * param[in] inode 
 * param[in] name
 * param[out] Return 0 if succeed, otherwise, return -1
 */
int removexattri(struct inode *, const char *);


/**
 * List an attribute (name:value)
 * param[in] inode 
 * param[in] list
 * param[in] list buffer size
 * param[out] the size of list
 */
ssize_t listxattri(struct inode *, char *, size_t);

#ifdef __cplusplus
}
#endif

#endif
