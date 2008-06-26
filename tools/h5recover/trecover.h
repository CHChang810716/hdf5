/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have          *
 * access to either file, you may request a copy from help@hdfgroup.org.     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "hdf5.h"
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>

/*
#include <unistd.h>
 * Header file for the trecover test program.
 *
 * Creator: Albert Cheng, Jan 28, 2008.
 */

/* Macro definitions */
/* Crash modes */
#define SyncCrash	0
#define AsyncCrash	1
#define	CRASH	crasher(SyncCrash, 0)

/* Dataset properties */
#define DSContig	0x1		/* Contigous type */
#define DSChunked	0x2		/* Chunked type */
#define DSZip		0x4		/* Zlib compressed */
#define DSSZip		0x8		/* SZlib compressed */
#define DSAll		~0x0   		/* All datasets */
#define DSNone		0x0   		/* No datasets */

/* Dataset default dimensions. Intentional small for easier dumping of data. */
#define RANK   2
#define NX     10                    /* dataset dimensions */
#define NY     10
#define ChunkX 2		    /* Dataset chunk sizes */
#define ChunkY 2
#define H5FILE_NAME        "trecover.h5"
#define CTL_H5FILE_NAME    "CTL"H5FILE_NAME	/* control file name */
#define JNL_H5FILE_NAME    H5FILE_NAME".jnl"	/* journal file name */
#define DATASETNAME "IntArray" 
#define CHUNKDATASETNAME "IntArrayChunked" 
#define ZDATASETNAME "IntArrayZCompressed" 
#define SZDATASETNAME "IntArraySZCompressed" 

/* Data Structures */
typedef union CrasherParam_t {
    float tinterval;		/* time interval to schedule an Async Crash */
} CrasherParam_t;


/* Global variables */
extern CrasherParam_t	AsyncCrashParam;
extern int		CrashMode;
extern hid_t		datafile, ctl_file;    /* data and control file ids */
extern char *		dsetname;		/* dataset name */
extern int		PatchMode;		/* dataset name */

/* protocol definitions */
void crasher(int crash_mode, CrasherParam_t *crash_param);
void create_dataset(hid_t f, int dstype, int rank, hsize_t *dims, hsize_t *dimschunk);
int writedata(hid_t dataset, int begin, int end);
int extend_dataset(hid_t f, int begin, int end, int patch);
void wakeup(int signum);
void parser(int ac, char **av);	/* command option parser */
void init(void);		/* initialization */
void help(void);		/* initialization */
int create_files(const char *filename, const char *ctl_filename);
int journal_files(const char *filename, const char *ctl_filename, const char *jnl_filename, int patch);

int close_file(hid_t fid);
