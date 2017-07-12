/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5. The full HDF5 copyright notice, including      *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************/
/* Module Setup */
/****************/

#include "H5PLmodule.h"          /* This source code file is part of the H5PL module */


/***********/
/* Headers */
/***********/
#include "H5private.h"      /* Generic Functions            */
#include "H5Eprivate.h"     /* Error handling               */
#include "H5MMprivate.h"    /* Memory management            */
#include "H5PLpkg.h"        /* Plugin                       */


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
 * Function:    H5PLset_loading_state
 *
 * Purpose:     Control the loading of dynamic plugin types.
 *
 *              This function will not allow plugin types if the pathname
 *              from the HDF5_PLUGIN_PRELOAD environment variable is set to
 *              the special "::" string.
 *
 *              plugin bit = 0, will prevent the use of that dynamic plugin type.
 *              plugin bit = 1, will allow the use of that dynamic plugin type.
 *
 *              H5PL_TYPE_FILTER changes just dynamic filters
 *              A H5PL_ALL_PLUGIN will enable all dynamic plugin types
 *              A zero value will disable all dynamic plugin types
 *
 * Return:      Success:    Non-negative
 *              Failture:   Negative
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLset_loading_state(unsigned int plugin_type)
{
    char *preload_path = NULL;
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE1("e", "Iu", plugin_type);

    /* change the bit value of the requested plugin type(s) */
    /* XXX: This is not a bitwise operation and clobbers instead of
     *      setting bits.
     */
    H5PL_plugin_g = plugin_type;

    /* check if special ENV variable is set and disable all plugin types.
     *
     * NOTE: The special symbol "::" means no plugin during data reading.
     *
     * This should be checked at library init, not here.
     */
    if (NULL != (preload_path = HDgetenv("HDF5_PLUGIN_PRELOAD")))
        if (!HDstrcmp(preload_path, H5PL_NO_PLUGIN))
            H5PL_plugin_g = 0;

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLset_loading_state() */


/*-------------------------------------------------------------------------
 * Function:    H5PLget_loading_state
 *
 * Purpose:     Query state of the loading of dynamic plugin types.
 *
 *              This function will return the state of the global flag.
 *
 *  Return:     Zero if all plugin types are disabled
 *              Negative if all plugin types are enabled
 *              Positive if one or more of the plugin types are enabled
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLget_loading_state(unsigned int *plugin_type)
{
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE1("e", "*Iu", plugin_type);

    /* XXX: This is a mess */
    if (plugin_type)
        *plugin_type = H5PL_plugin_g;

done:
    /* XXX: This is egregious abuse of the herr_t type. */
    FUNC_LEAVE_API(ret_value)
} /* end H5PLget_loading_state() */


/*-------------------------------------------------------------------------
 * Function:    H5PLappend
 *
 * Purpose:     Insert a plugin search path at the end of the list.
 *
 * Return:      Success:    Non-negative
 *              Failture:   Negative
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLappend(const char *search_path)
{
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE1("e", "*s", search_path);

    /* Check args */
    if (NULL == search_path)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot be NULL")
    if (0 == HDstrlen(search_path))
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot have length zero")

    /* Append the search path to the path table */
    if (H5PL__append_path(search_path) < 0)
        HGOTO_ERROR(H5E_PLUGIN, H5E_CANTAPPEND, FAIL, "unable to append search path")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLappend() */


/*-------------------------------------------------------------------------
 * Function:    H5PLprepend
 *
 * Purpose:     Insert a plugin search path at the beginning of the list.
 *
 * Return:      Success:    Non-negative
 *              Failture:   Negative
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLprepend(const char *search_path)
{
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE1("e", "*s", search_path);

    /* Check args */
    if (NULL == search_path)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot be NULL")
    if (0 == HDstrlen(search_path))
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot have length zero")

    /* Prepend the search path to the path table */
    if (H5PL__prepend_path(search_path) < 0)
        HGOTO_ERROR(H5E_PLUGIN, H5E_CANTINSERT, FAIL, "unable to prepend search path")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLprepend() */


/*-------------------------------------------------------------------------
 * Function:    H5PLreplace
 *
 * Purpose:     Replace the path at the specified index. The path at the
 *              index must exist.
 *
 * Return:      Non-negative or success.
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLreplace(const char *search_path, unsigned int index)
{
    unsigned    num_paths;              /* Current number of stored paths */
    herr_t      ret_value = SUCCEED;    /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE2("e", "*sIu", search_path, index);

    /* Check args */
    if (NULL == search_path)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot be NULL")
    if (0 == HDstrlen(search_path))
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot have length zero")

    /* Check index */
    num_paths = H5PL__get_num_paths();
    if (0 == num_paths)
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, FAIL, "path table is empty")
    else if (index >= num_paths)
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, FAIL, "index path out of bounds for table - can't be more than %u", (num_paths - 1))

    /* Insert the search path into the path table */
    if (H5PL__replace_path(search_path, index) < 0)
        HGOTO_ERROR(H5E_PLUGIN, H5E_CANTINSERT, FAIL, "unable to replace search path")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLreplace() */


/*-------------------------------------------------------------------------
 * Function:    H5PLinsert
 *
 * Purpose:     Insert a plugin search path at the specified index, moving
 *              other paths after the index.
 *
 * Return:      Success:    Non-negative
 *              Failture:   Negative
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLinsert(const char *search_path, unsigned int index)
{
    unsigned    num_paths;              /* Current number of stored paths */
    herr_t      ret_value = SUCCEED;    /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE2("e", "*sIu", search_path, index);

    /* Check args */
    if (NULL == search_path)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot be NULL")
    if (0 == HDstrlen(search_path))
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "plugin_path parameter cannot have length zero")

    /* Check index */
    num_paths = H5PL__get_num_paths();
    if ((0 != num_paths) && (index >= num_paths))
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, FAIL, "index path out of bounds for table - can't be more than %u", (num_paths - 1))

    /* Insert the search path into the path table */
    if (H5PL__insert_path(search_path, index) < 0)
        HGOTO_ERROR(H5E_PLUGIN, H5E_CANTINSERT, FAIL, "unable to insert search path")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLinsert() */


/*-------------------------------------------------------------------------
 * Function:    H5PLremove
 *
 * Purpose:     Remove the plugin path at the specifed index and compact
 *              the list.
 *
 * Return:      Success:    Non-negative
 *              Failture:   Negative
 *
 * Return:      Non-negative or success.
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLremove(unsigned int index)
{
    unsigned    num_paths;              /* Current number of stored paths */
    herr_t      ret_value = SUCCEED;    /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE1("e", "Iu", index);

    /* Check index */
    num_paths = H5PL__get_num_paths();
    if (0 == num_paths)
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, FAIL, "path table is empty")
    else if (index >= num_paths)
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, FAIL, "index path out of bounds for table - can't be more than %u", (num_paths - 1))

    /* Delete the search path from the path table */
    if (H5PL__remove_path(index) < 0)
        HGOTO_ERROR(H5E_PLUGIN, H5E_CANTDELETE, FAIL, "unable to remove search path")

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLremove() */


/*-------------------------------------------------------------------------
 * Function:    H5PLget
 *
 * Purpose:     Query the plugin path at a specified index.
 *
 *  If 'pathname' is non-NULL then up to 'size' bytes will be written into
 *  that buffer and the length of the pathname will be returned.
 *
 *  If 'pathname' is NULL, this function will simply return the number of
 *  characters required to store the pathname, ignoring 'pathname' and 'size'
 *
 *  If an error occurs then the buffer pointed to by `pathname'
 *  (NULL or non-NULL) will be unchanged and the function will return a
 *  negative value.
 *
 *  If a zero is returned for the name's length, then there is no pathname
 *  associated with the index and the 'pathname' buffer will be unchanged.
 *
 * Return:  Success:    The length of path
 *          Failure:    A negative value
 *
 *-------------------------------------------------------------------------
 */
ssize_t
H5PLget(unsigned int index, char *path_name, size_t size)
{
    unsigned    num_paths;              /* Current number of stored paths */
    const char *path = NULL;            /* path from table */
    size_t      path_len = 0;           /* Length of path */
    ssize_t     ret_value = 0;          /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE3("Zs", "Iu*sz", index, path_name, size);

    /* Check index */
    num_paths = H5PL__get_num_paths();
    if (0 == num_paths)
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, FAIL, "path table is empty")
    else if (index >= num_paths)
        HGOTO_ERROR(H5E_ARGS, H5E_BADRANGE, FAIL, "index path out of bounds for table - can't be more than %u", (num_paths - 1))

    /* Check if the search table is empty */
    if (H5PL__get_num_paths() == 0)
        HGOTO_ERROR(H5E_PLUGIN, H5E_NOSPACE, (-1), "plugin search path table is empty")

    /* Get the path at the specified index and its length */
    if (NULL == (path = H5PL__get_path(index)))
        HGOTO_ERROR(H5E_PLUGIN, H5E_BADVALUE, (-1), "no path stored at that index")
    path_len = HDstrlen(path);

    /* If the path_name is not NULL, copy the path to the buffer */
    if (path_name) {
        HDstrncpy(path_name, path, MIN((size_t)(path_len + 1), size));
        if ((size_t)path_len >= size)
            path_name[size - 1] = '\0';
    } /* end if */

    /* Set return value */
    ret_value = (ssize_t)path_len;

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLget() */


/*-------------------------------------------------------------------------
 * Function:    H5PLsize
 *
 * Purpose:     Get the number of stored plugin paths.
 *              XXX: This is a terrible name. Can it be changed?
 *
 * Return:      SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5PLsize(unsigned int *num_paths)
{
    herr_t ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_API(FAIL)
    H5TRACE1("e", "*Iu", num_paths);

    /* Check arguments */
    if (!num_paths)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "num_paths parameter cannot be NULL")

    /* Get the number of stored plugin paths */
    *num_paths = H5PL__get_num_paths();

done:
    FUNC_LEAVE_API(ret_value)
} /* end H5PLsize() */

