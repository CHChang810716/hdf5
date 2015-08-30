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
 * Purpose:     Reference routines.
 */

/****************/
/* Module Setup */
/****************/

#include "H5Rmodule.h"      /* This source code file is part of the H5R module */

/***********/
/* Headers */
/***********/
#include "H5private.h"          /* Generic Functions                        */
#include "H5ACprivate.h"        /* Metadata cache                           */
#include "H5CXprivate.h"        /* API Contexts                             */
#include "H5Eprivate.h"         /* Error handling                           */
#include "H5Gprivate.h"         /* Groups                                   */
#include "H5Iprivate.h"         /* IDs                                      */
#include "H5Rpkg.h"             /* References                               */


/****************/
/* Local Macros */
/****************/


/******************/
/* Local Typedefs */
/******************/


/********************/
/* Local Prototypes */
/********************/


/*********************/
/* Package Variables */
/*********************/


/*****************************/
/* Library Private Variables */
/*****************************/


/*******************/
/* Local Variables */
/*******************/


/*-------------------------------------------------------------------------
 * Function:    H5Rcreate_object
 *
 * Purpose: Creates an object reference. The LOC_ID and NAME are used to locate
 * the object pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rcreate_object(hid_t loc_id, const char *name)
{
    H5G_loc_t loc; /* File location */
    href_t    ret_value = NULL; /* Return value */

    FUNC_ENTER_API(NULL)
    H5TRACE2("r", "i*s", loc_id, name);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "not a location")
    if(!name || !*name)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no name given")

    if(NULL == (ret_value = H5R_create_object(&loc, name)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCREATE, NULL, "unable to create object reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rcreate_object() */


/*-------------------------------------------------------------------------
 * Function:    H5Rcreate_region
 *
 * Purpose: Creates a region reference. The LOC_ID and NAME are used to locate
 * the object pointed to and the SPACE_ID is used to choose the region pointed
 * to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rcreate_region(hid_t loc_id, const char *name, hid_t space_id)
{
    H5G_loc_t loc; /* File location */
    struct H5S_t *space = NULL;  /* Pointer to dataspace containing region */
    href_t     ret_value = NULL; /* Return value */

    FUNC_ENTER_API(NULL)
    H5TRACE3("r", "i*si", loc_id, name, space_id);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "not a location")
    if(!name || !*name)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no name given")
    if(space_id == H5I_BADID)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "reference region dataspace id must be valid")
    if(space_id != H5I_BADID && (NULL == (space = (struct H5S_t *)H5I_object_verify(space_id, H5I_DATASPACE))))
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "not a dataspace")

    if(NULL == (ret_value = H5R_create_region(&loc, name, space)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCREATE, NULL, "unable to create region reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rcreate_region() */


/*-------------------------------------------------------------------------
 * Function:    H5Rcreate_attr
 *
 * Purpose: Creates an attribute reference. The LOC_ID, NAME and ATTR_NAME are
 * used to locate the attribute pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rcreate_attr(hid_t loc_id, const char *name, const char *attr_name)
{
    H5G_loc_t loc; /* File location */
    href_t    ret_value = NULL; /* Return value */

    FUNC_ENTER_API(NULL)
    H5TRACE3("r", "i*s*s", loc_id, name, attr_name);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "not a location")
    if(!name || !*name)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no name given")
    if(!attr_name || !*attr_name)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no attribute name given")

    if(NULL == (ret_value = H5R_create_attr(&loc, name, attr_name)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCREATE, NULL, "unable to create atribute reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rcreate_attr() */


/*-------------------------------------------------------------------------
 * Function:    H5Rcreate_ext_object
 *
 * Purpose: Creates an external object reference. The FILENAME and PATHNAME
 * are used to locate the object pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rcreate_ext_object(const char *filename, const char *pathname)
{
    href_t    ret_value = NULL; /* Return value */

    FUNC_ENTER_API(NULL)
    H5TRACE2("r", "*s*s", filename, pathname);

    /* Check args */
    if(!filename || !*filename)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no filename given")
    if(!pathname || !*pathname)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no pathname given")

    if(NULL == (ret_value = H5R_create_ext_object(filename, pathname)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCREATE, NULL, "unable to create object reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rcreate_ext_object() */


/*-------------------------------------------------------------------------
 * Function:    H5Rcreate_ext_region
 *
 * Purpose: Creates an external region reference. The FILENAME and PATHNAME
 * are used to locate the object pointed to and the SPACE_ID is used to choose
 * the region pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rcreate_ext_region(const char *filename, const char *pathname, hid_t space_id)
{
    struct H5S_t *space = NULL;  /* Pointer to dataspace containing region */
    href_t     ret_value = NULL; /* Return value */

    FUNC_ENTER_API(NULL)
    H5TRACE3("r", "*s*si", filename, pathname, space_id);

    /* Check args */
    if(!filename || !*filename)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no filename given")
    if(!pathname || !*pathname)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no pathname given")
    if(space_id == H5I_BADID)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "reference region dataspace id must be valid")
    if(space_id != H5I_BADID && (NULL == (space = (struct H5S_t *)H5I_object_verify(space_id, H5I_DATASPACE))))
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, NULL, "not a dataspace")

    if(NULL == (ret_value = H5R_create_ext_region(filename, pathname, space)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCREATE, NULL, "unable to create region reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rcreate_ext_region() */


/*-------------------------------------------------------------------------
 * Function:    H5Rcreate_ext_attr
 *
 * Purpose: Creates an external attribute reference. The FILENAME, PATHNAME
 * and ATTR_NAME are used to locate the attribute pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rcreate_ext_attr(const char *filename, const char *pathname, const char *attr_name)
{
    href_t    ret_value = NULL; /* Return value */

    FUNC_ENTER_API(NULL)
    H5TRACE3("r", "*s*s*s", filename, pathname, attr_name);

    /* Check args */
    if(!filename || !*filename)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no filename given")
    if(!pathname || !*pathname)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no pathname given")
    if(!attr_name || !*attr_name)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "no attribute name given")

    if(NULL == (ret_value = H5R_create_ext_attr(filename, pathname, attr_name)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCREATE, NULL, "unable to create atribute reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rcreate_ext_attr() */


/*-------------------------------------------------------------------------
 * Function:    H5Rdestroy
 *
 * Purpose: Destroy reference and free resources allocated during H5Rcreate.
 *
 * Return:  Non-negative on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5Rdestroy(href_t ref)
{
    herr_t    ret_value = FAIL; /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE1("e", "r", ref);

    /* Check args */
    if(NULL == ref)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "invalid reference pointer")

    if(FAIL == (ret_value = H5R_destroy(ref)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTDELETE, FAIL, "unable to destroy reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rdestroy() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_type
 *
 * Purpose: Given a reference to some object, return the type of that reference.
 *
 * Return:  Reference type/H5R_BADTYPE on failure
 *
 *-------------------------------------------------------------------------
 */
H5R_type_t
H5Rget_type(href_t ref)
{
    H5R_type_t ret_value;

    FUNC_ENTER_API(H5R_BADTYPE)
    H5TRACE1("Rt", "r", ref);

    /* Check args */
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, H5R_BADTYPE, "invalid reference pointer")

    /* Create reference */
    ret_value = H5R_get_type(ref);
    if((ret_value <= H5R_BADTYPE) || (ret_value >= H5R_MAXTYPE))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, H5R_BADTYPE, "invalid reference type")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_type() */


/*-------------------------------------------------------------------------
 * Function:    H5Requal
 *
 * Purpose: Compare two references
 *
 * Return:  TRUE if equal, FALSE if unequal, FAIL if error
 *
 *-------------------------------------------------------------------------
 */
htri_t
H5Requal(href_t ref1, href_t ref2)
{
    htri_t ret_value;

    FUNC_ENTER_API(FAIL)
    H5TRACE2("t", "rr", ref1, ref2);

    /* Check args */
    if(!ref1 || !ref2)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "invalid reference pointer")

    /* Create reference */
    if (FAIL == (ret_value = H5R__equal(ref1, ref2)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCOMPARE, FAIL, "cannot compare references")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Requal() */


/*-------------------------------------------------------------------------
 * Function:    H5Rcopy
 *
 * Purpose: Copy a reference
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rcopy(href_t ref)
{
    href_t ret_value;

    FUNC_ENTER_API(NULL)
    H5TRACE1("r", "r", ref);

    /* Check args */
    if(!ref)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "invalid reference pointer")

    /* Create reference */
    if (NULL == (ret_value = H5R__copy(ref)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCOPY, NULL, "cannot copy reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rcopy() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_object
 *
 * Purpose: Given a reference to some object, open that object and return an
 * ID for that object.
 *
 * Return:  Valid ID on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
hid_t
H5Rget_object(hid_t obj_id, hid_t oapl_id, href_t _ref)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    H5G_loc_t loc;      /* Group location */
    hid_t ret_value = H5I_INVALID_HID;  /* Return value */

    FUNC_ENTER_API(H5I_INVALID_HID)
    H5TRACE3("i", "iir", obj_id, oapl_id, _ref);

    /* Check args */
    if(H5G_loc(obj_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, H5I_INVALID_HID, "not a location")
    if(oapl_id < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, H5I_INVALID_HID, "not a property list")
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, H5I_INVALID_HID, "invalid reference pointer")
    if(ref->ref_type <= H5R_BADTYPE || ref->ref_type >= H5R_MAXTYPE)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, H5I_INVALID_HID, "invalid reference type")

    /* Verify access property list and set up collective metadata if appropriate */
    if(H5CX_set_apl(&oapl_id, H5P_CLS_DACC, obj_id, FALSE) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTSET, H5I_INVALID_HID, "can't set access property list info")

    /* Get_object */
    if((ret_value = H5R__get_object(loc.oloc->file, oapl_id, ref, TRUE)) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, H5I_INVALID_HID, "unable to get_object object")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_object() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_region2
 *
 * Purpose: Given a reference to some object, creates a copy of the dataset
 * pointed to's dataspace and defines a selection in the copy which is the
 * region pointed to.
 *
 * Return:  Valid ID on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
hid_t
H5Rget_region2(hid_t loc_id, href_t _ref)
{
    H5G_loc_t loc;          /* Object's group location */
    struct H5S_t *space = NULL;  /* Pointer to dataspace containing region */
    struct href *ref = (struct href *) _ref; /* Reference */
    hid_t ret_value = H5I_INVALID_HID;  /* Return value */

    FUNC_ENTER_API(H5I_INVALID_HID)
    H5TRACE2("i", "ir", loc_id, _ref);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, H5I_INVALID_HID, "not a location")
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, H5I_INVALID_HID, "invalid reference pointer")
    if(ref->ref_type != H5R_REGION && ref->ref_type != H5R_EXT_REGION)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, H5I_INVALID_HID, "invalid reference type")

    /* Get the dataspace with the correct region selected */
    if(NULL == (space = H5R__get_region(loc.oloc->file, ref)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, H5I_INVALID_HID, "unable to create dataspace")

    /* Atomize */
    if((ret_value = H5I_register (H5I_DATASPACE, space, TRUE)) < 0)
        HGOTO_ERROR(H5E_ATOM, H5E_CANTREGISTER, H5I_INVALID_HID, "unable to register dataspace atom")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_region2() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_attr
 *
 * Purpose: Given a reference to some attribute, open that attribute and
 * return an ID for that attribute.
 *
 * Return:  Valid ID on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
hid_t
H5Rget_attr(hid_t loc_id, href_t _ref)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    H5G_loc_t loc;      /* Group location */
    struct H5A_t *attr;        /* Attribute */
    hid_t ret_value = H5I_INVALID_HID;  /* Return value */

    FUNC_ENTER_API(H5I_INVALID_HID)
    H5TRACE2("i", "ir", loc_id, _ref);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, H5I_INVALID_HID, "not a location")
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, H5I_INVALID_HID, "invalid reference pointer")
    if(ref->ref_type != H5R_ATTR && ref->ref_type != H5R_EXT_ATTR)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, H5I_INVALID_HID, "invalid reference type")

    /* Get the attribute */
    if((attr = H5R__get_attr(loc.oloc->file, ref)) == NULL)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, H5I_INVALID_HID, "unable to open attribute")

    /* Atomize */
    if((ret_value = H5I_register (H5I_ATTR, attr, TRUE)) < 0)
        HGOTO_ERROR(H5E_ATOM, H5E_CANTREGISTER, H5I_INVALID_HID, "unable to register attribute atom")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_attr() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_obj_type3
 *
 * Purpose: Given a reference to some object, this function returns the type
 * of object pointed to.
 *
 * Return:  Non-negative on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5Rget_obj_type3(hid_t loc_id, href_t _ref, H5O_type_t *obj_type)
{
    H5G_loc_t loc;              /* Object location */
    struct href *ref = (struct href *) _ref; /* Reference */
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE3("e", "ir*Ot", loc_id, _ref, obj_type);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a location")
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "invalid reference pointer")
    if(ref->ref_type <= H5R_BADTYPE || ref->ref_type >= H5R_MAXTYPE)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "invalid reference type")

    /* Get the object information */
    if(H5R__get_obj_type(loc.oloc->file, ref, obj_type) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTINIT, FAIL, "unable to determine object type")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_obj_type3() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_obj_name
 *
 * Purpose: Given a reference to some object, determine a path to the object
 * referenced in the file.
 *
 * Return:  Non-negative length of the path on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
ssize_t
H5Rget_obj_name(hid_t loc_id, href_t _ref, char *name, size_t size)
{
    H5G_loc_t loc;      /* Group location */
    H5F_t *file;        /* File object */
    struct href *ref = (struct href *) _ref; /* Reference */
    ssize_t ret_value;  /* Return value */

    FUNC_ENTER_API((-1))
    H5TRACE4("Zs", "ir*sz", loc_id, _ref, name, size);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, (-1), "not a location")
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, (-1), "invalid reference pointer")
    if(ref->ref_type <= H5R_BADTYPE || ref->ref_type >= H5R_MAXTYPE)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, (-1), "invalid reference type")

    /* Get the file pointer from the entry */
    file = loc.oloc->file;

    /* Get name */
    if((ret_value = H5R__get_obj_name(file, H5P_DEFAULT, ref, name, size)) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, (-1), "unable to determine object path")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_obj_name() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_attr_name
 *
 * Purpose: Given a reference to some attribute, determine its name.
 *
 * Return:  Non-negative length of the path on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
ssize_t
H5Rget_attr_name(hid_t loc_id, href_t _ref, char *name, size_t size)
{
    H5G_loc_t loc;      /* Group location */
    H5F_t *file;        /* File object */
    struct href *ref = (struct href *) _ref; /* Reference */
    ssize_t ret_value;  /* Return value */

    FUNC_ENTER_API((-1))
    H5TRACE4("Zs", "ir*sz", loc_id, _ref, name, size);

    /* Check args */
    if(H5G_loc(loc_id, &loc) < 0)
        HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, (-1), "not a location")
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, (-1), "invalid reference pointer")
    if((ref->ref_type != H5R_ATTR) && (ref->ref_type != H5R_EXT_ATTR))
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, (-1), "invalid reference type")

    /* Get the file pointer from the entry */
    file = loc.oloc->file;

    /* Get name */
    if((ret_value = H5R__get_attr_name(file, ref, name, size)) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, (-1), "unable to determine object path")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_attr_name() */


/*-------------------------------------------------------------------------
 * Function:    H5Rget_file_name
 *
 * Purpose: Given a reference to some object, determine a file name of the
 * object located into.
 *
 * Return:  Non-negative length of the path on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
ssize_t
H5Rget_file_name(href_t _ref, char *name, size_t size)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    ssize_t ret_value;  /* Return value */

    FUNC_ENTER_API((-1))
    H5TRACE3("Zs", "r*sz", _ref, name, size);

    /* Check args */
    if(ref == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, (-1), "invalid reference pointer")
    if(ref->ref_type <= H5R_BADTYPE || ref->ref_type >= H5R_MAXTYPE)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, (-1), "invalid reference type")

    /* Get name */
    if((ret_value = H5R__get_file_name(ref, name, size)) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, (-1), "unable to retrieve file name")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rget_file_name() */


/*-------------------------------------------------------------------------
 * Function:    H5Rencode
 *
 * Purpose: Given a reference, serialize it into a buffer.
 *
 * Return:  Non-negative on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5Rencode(href_t ref, void *buf, size_t *nalloc)
{
    herr_t ret_value = SUCCEED;

    FUNC_ENTER_API(FAIL)
    H5TRACE3("e", "r*x*z", ref, buf, nalloc);

    /* Check argument and retrieve object */
    if(nalloc == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "NULL pointer for buffer size")

    /* Go encode the datatype */
    if(H5R_encode(ref, (unsigned char *)buf, nalloc) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTENCODE, FAIL, "can't encode reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rencode() */


/*-------------------------------------------------------------------------
 * Function:    H5Rdecode
 *
 * Purpose: Deserialize a reference.
 *
 * Return:  Success:    Reference
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5Rdecode(const void *buf)
{
    href_t ret_value; /* Return value */

    FUNC_ENTER_API(NULL)
    H5TRACE1("r", "*x", buf);

    /* Check args */
    if(buf == NULL)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "empty buffer")

    /* Create datatype by decoding buffer */
    if(NULL == (ret_value = H5R_decode((const unsigned char *)buf, NULL)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTDECODE, NULL, "can't decode reference")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5Rdecode() */

