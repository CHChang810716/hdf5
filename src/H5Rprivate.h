/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * This file contains private information about the H5R module
 */
#ifndef _H5Rprivate_H
#define _H5Rprivate_H

/* Include package's public header */
#include "H5Rpublic.h"

/* Private headers needed by this file */
#include "H5Fprivate.h"         /* Files                                    */
#include "H5Gprivate.h"         /* Groups                                   */
#include "H5Oprivate.h"         /* Object headers                           */
#include "H5Sprivate.h"         /* Dataspaces                               */


/**************************/
/* Library Private Macros */
/**************************/


/****************************/
/* Library Private Typedefs */
/****************************/


/*****************************/
/* Library Private Variables */
/*****************************/


/******************************/
/* Library Private Prototypes */
/******************************/

/* To prevent including H5Sprivate.h that includes H5Rprivate.h */
struct H5S_t;

/* Private functions */
H5_DLL href_t H5R_create_object(H5G_loc_t *loc, const char *name);
H5_DLL href_t H5R_create_region(H5G_loc_t *loc, const char *name,
    struct H5S_t *space);
H5_DLL href_t H5R_create_attr(H5G_loc_t *loc, const char *name,
    const char *attr_name);
H5_DLL href_t H5R_create_ext_object(const char *filename, const char *pathname);
H5_DLL href_t H5R_create_ext_region(const char *filename, const char *pathname,
    struct H5S_t *space);
H5_DLL href_t H5R_create_ext_attr(const char *filename, const char *pathname,
    const char *attr_name);
H5_DLL herr_t H5R_destroy(href_t ref);

H5_DLL H5R_type_t H5R_get_type(href_t ref);

H5_DLL herr_t H5R_encode(href_t ref, unsigned char *buf, size_t *nalloc);
H5_DLL href_t H5R_decode(const unsigned char *buf, size_t *nbytes);

H5_DLL herr_t H5R_cast(href_t _ref, H5R_type_t ref_type);

#endif  /* _H5Rprivate_H */
