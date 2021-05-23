#ifndef _TYPES_H_
#define _TYPES_H_

#include <sys/time.h>
#include "ds/uthash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned char	uchar;
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long int uint64_t;
typedef uint64_t		addr_t;
typedef uint64_t		offset_t;
typedef struct timeval	mlfs_time_t; // 16 bytes
typedef UT_hash_handle	mlfs_hash_t;

typedef uint32_t mlfs_lblk_t;
typedef uint64_t mlfs_fsblk_t;

#define LINUX_DT_UNKNOWN  0
#define LINUX_DT_FIFO     1
#define LINUX_DT_CHR      2
#define LINUX_DT_DIR      4
#define LINUX_DT_BLK      6
#define LINUX_DT_REG      8
#define LINUX_DT_LNK      10
#define LINUX_DT_SOCK     12
#define LINUX_DT_WHT      14

struct linux_dirent64 {
	uint64_t            d_ino;      /* Inode number */
	uint64_t            d_off;      /* Offset to next linux_dirent */
	unsigned short int  d_reclen;   /* Length of this linux_dirent */
	unsigned char       d_type;
	char                d_name[256];   /* File name (null-terminated) */
};

struct linux_dirent {
	unsigned long       d_ino;      /* Inode number */
	unsigned long       d_off;      /* Offset to next linux_dirent */
	unsigned short int  d_reclen;   /* Length of this linux_dirent */
	char                d_name[246];   /* File name (null-terminated) */
};

struct linux_dirent_tail {
	char                pad;
	unsigned char       d_type;
};

  #ifndef _GNU_SOURCE
struct statx_timestamp {
	int64_t tv_sec;    /* Seconds since the Epoch (UNIX time) */
	uint32_t tv_nsec;   /* Nanoseconds since tv_sec */
};

struct statx {
	uint32_t stx_mask;        /* Mask of bits indicating
							  filled fields */
	uint32_t stx_blksize;     /* Block size for filesystem I/O */
	uint64_t stx_attributes;  /* Extra file attribute indicators */
	uint32_t stx_nlink;       /* Number of hard links */
	uint32_t stx_uid;         /* User ID of owner */
	uint32_t stx_gid;         /* Group ID of owner */
	uint16_t stx_mode;        /* File type and mode */
	uint64_t stx_ino;         /* Inode number */
	uint64_t stx_size;        /* Total size in bytes */
	uint64_t stx_blocks;      /* Number of 512B blocks allocated */
	uint64_t stx_attributes_mask;
	/* Mask to show what's supported
	   in stx_attributes */

	/* The following fields are file timestamps */
	struct statx_timestamp stx_atime;  /* Last access */
	struct statx_timestamp stx_btime;  /* Creation */
	struct statx_timestamp stx_ctime;  /* Last status change */
	struct statx_timestamp stx_mtime;  /* Last modification */

	/* If this file represents a device, then the next two
	   fields contain the ID of the device */
	uint32_t stx_rdev_major;  /* Major ID */
	uint32_t stx_rdev_minor;  /* Minor ID */

	/* The next two fields contain the ID of the device
	   containing the filesystem where the file resides */
	uint32_t stx_dev_major;   /* Major ID */
	uint32_t stx_dev_minor;   /* Minor ID */
};
  #endif

#ifdef __cplusplus
}
#endif

#endif
