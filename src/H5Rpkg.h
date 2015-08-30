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

/* Purpose:     This file contains declarations which are visible
 *              only within the H5R package. Source files outside the
 *              H5R package should include H5Rprivate.h instead.
 */
#if !(defined H5R_FRIEND || defined H5R_MODULE)
#error "Do not include this file outside the H5R package!"
#endif

#ifndef _H5Rpkg_H
#define _H5Rpkg_H

/* Get package's private header */
#include "H5Rprivate.h"

/* Other private headers needed by this file */
#include "H5Fprivate.h"         /* Files                                    */
#include "H5Gprivate.h"         /* Groups                                   */
#include "H5Oprivate.h"         /* Object headers                           */
#include "H5Sprivate.h"         /* Dataspaces                               */


/**************************/
/* Package Private Macros */
/**************************/

#define H5R_INITIALIZER { H5R_BADTYPE, {0, NULL} }

/****************************/
/* Package Private Typedefs */
/****************************/

/* Internal data structures */
struct href {
    H5R_type_t ref_type;
    union {
        struct {
            size_t buf_size;/* Size of serialized reference */
            void *buf;      /* Pointer to serialized reference */
        } serial;
        haddr_t addr;
    } ref;
};

/*****************************/
/* Package Private Variables */
/*****************************/


/******************************/
/* Package Private Prototypes */
/******************************/

/* General functions */
H5_DLL htri_t H5R__equal(href_t _ref1, href_t _ref2);
H5_DLL href_t H5R__copy(href_t _ref);
H5_DLL ssize_t H5R__get_file_name(href_t ref, char *name, size_t size);
H5_DLL herr_t H5R__get_obj_type(H5F_t *file, href_t ref, H5O_type_t *obj_type);
H5_DLL hid_t H5R__get_object(H5F_t *file, hid_t dapl_id, href_t ref,
    hbool_t app_ref);
H5_DLL struct H5S_t *H5R__get_region(H5F_t *file, href_t ref);
H5_DLL struct H5A_t *H5R__get_attr(H5F_t *file, href_t ref);
H5_DLL ssize_t H5R__get_obj_name(H5F_t *file, hid_t lapl_id, href_t ref,
    char *name, size_t size);
H5_DLL ssize_t H5R__get_attr_name(H5F_t *file, href_t ref, char *name,
    size_t size);

#endif /* _H5Rpkg_H */

