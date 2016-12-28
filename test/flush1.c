/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
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

/*
 * Programmer:  Robb Matzke <matzke@llnl.gov>
 *              Friday, October 23, 1998
 *
 * Purpose:	This is the first half of a two-part test that makes sure
 *		that a file can be read after an application crashes as long
 *		as the file was flushed first.  We simulate a crash by
 *		calling _exit(EXIT_SUCCESS) since this doesn't flush HDF5 caches but
 *		still exits with success.
 */
#include "h5test.h"

const char *FILENAME[] = {
    "flush",
    "flush-swmr",
    "noflush",
    "noflush-swmr",
    "flush_extend",
    "flush_extend-swmr",
    "noflush_extend",
    "noflush_extend-swmr",
    NULL
};

static double	the_data[100][100];

/*-------------------------------------------------------------------------
 * Function:	create_file
 *
 * Purpose:	Creates files used in part 1 of the test
 *
 * Return:	Success:	0
 *
 *		Failure:	1
 *
 * Programmer:	Leon Arber
 *              Sept. 26, 2006
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static hid_t
create_file(char* name, hid_t fapl, hbool_t swmr)
{
    hid_t	file, dcpl, space, dset, groups, grp;
    hsize_t	ds_size[2] = {100, 100};
    hsize_t	ch_size[2] = {5, 5};
    unsigned    flags;
    size_t	i, j;

    flags = H5F_ACC_TRUNC | (swmr ? H5F_ACC_SWMR_WRITE : 0);

    if((file = H5Fcreate(name, flags, H5P_DEFAULT, fapl)) < 0) FAIL_STACK_ERROR

    /* Create a chunked dataset */
    if((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0) FAIL_STACK_ERROR
    if(H5Pset_chunk(dcpl, 2, ch_size) < 0) FAIL_STACK_ERROR
    if((space = H5Screate_simple(2, ds_size, NULL)) < 0) FAIL_STACK_ERROR
    if((dset = H5Dcreate2(file, "dset", H5T_NATIVE_FLOAT, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0) FAIL_STACK_ERROR

    /* Write some data */
    for(i = 0; i < ds_size[0]; i++)
        for(j = 0; j < (size_t)ds_size[1]; j++)
	    the_data[i][j] = (double)i / (double)(j + 1);
    if(H5Dwrite(dset, H5T_NATIVE_DOUBLE, space, space, H5P_DEFAULT, the_data) < 0) FAIL_STACK_ERROR

    /* Create some groups */
    if((groups = H5Gcreate2(file, "some_groups", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0) FAIL_STACK_ERROR
    for(i = 0; i < 100; i++) {
	sprintf(name, "grp%02u", (unsigned)i);
	if((grp = H5Gcreate2(groups, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0) FAIL_STACK_ERROR
	if(H5Gclose(grp) < 0) FAIL_STACK_ERROR
    } /* end for */

    return file;

error:
    HD_exit(EXIT_FAILURE);
}


/*-------------------------------------------------------------------------
 * Function:	extend_file
 *
 * Purpose:	Add a small dataset to the file.
 *
 * Return:	Success:	0
 *
 *		Failure:	1
 *
 * Programmer:	Leon Arber
 *              Oct. 4, 2006
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static hid_t
extend_file(hid_t file)
{
    hid_t	dcpl, space, dset;
    hsize_t	ds_size[2] = {100, 100};
    hsize_t	ch_size[2] = {5, 5};
    size_t	i, j;

    /* Create a chunked dataset */
    if((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0) goto error;
    if(H5Pset_chunk(dcpl, 2, ch_size) < 0) goto error;
    if((space = H5Screate_simple(2, ds_size, NULL)) < 0) goto error;
    if((dset = H5Dcreate2(file, "dset2", H5T_NATIVE_FLOAT, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
	goto error;

    /* Write some data */
    for(i = 0; i < ds_size[0]; i++)
	for(j = 0; j < (size_t)ds_size[1]; j++)
	    the_data[i][j] = (double)i / (double)(j + 1);
    if(H5Dwrite(dset, H5T_NATIVE_DOUBLE, space, space, H5P_DEFAULT, the_data) < 0) goto error;

    return file;

error:
    HD_exit(EXIT_FAILURE);
}

/*-------------------------------------------------------------------------
 * Function:	main
 *
 * Purpose:	Part 1 of a two-part H5Fflush() test.
 *
 * Return:	Success:	0
 *
 *		Failure:	1
 *
 * Programmer:	Robb Matzke
 *              Friday, October 23, 1998
 *
 * Modifications:
 * 		Leon Arber
 * 		Sept. 26, 2006, expand test to check for failure if H5Fflush is not called.
 * 		Oct. 4 2006, expand test to check for partial failure in case file is flushed, but then
 * 				new datasets are created after the flush.
 *
 *
 *-------------------------------------------------------------------------
 */
int
main(void)
{
    hid_t file, fapl;
    char	name[1024];
    unsigned swmr;

    h5_reset();
    fapl = h5_fileaccess();

    TESTING("H5Fflush (part1)");

    /* Loop over SWMR & non-SWMR opens for the file */
    for(swmr = 0; swmr <= 1; swmr++) {
        /* Create the file */
        h5_fixname(FILENAME[0 + swmr], fapl, name, sizeof name);
        file = create_file(name, fapl, swmr);
        /* Flush and exit without closing the library */
        if (H5Fflush(file, H5F_SCOPE_GLOBAL) < 0) goto error;

        /* Create the other file which will not be flushed */
        h5_fixname(FILENAME[2 + swmr], fapl, name, sizeof name);
        file = create_file(name, fapl, swmr);

        /* Create the file */
        h5_fixname(FILENAME[4 + swmr], fapl, name, sizeof name);
        file = create_file(name, fapl, swmr);
        /* Flush and exit without closing the library */
        if(H5Fflush(file, H5F_SCOPE_GLOBAL) < 0) goto error;
        /* Add a bit to the file and don't flush the new part */
        extend_file(file);
        /* Flush and exit without closing the library */
        if (H5Fflush(file, H5F_SCOPE_GLOBAL) < 0) goto error;

        /* Create the file */
        h5_fixname(FILENAME[6 + swmr], fapl, name, sizeof name);
        file = create_file(name, fapl, swmr);
        /* Flush and exit without closing the library */
        if(H5Fflush(file, H5F_SCOPE_GLOBAL) < 0) goto error;
        /* Add a bit to the file and don't flush the new part */
        extend_file(file);
    } /* end for */

    PASSED();
    fflush(stdout);
    fflush(stderr);

    HD_exit(EXIT_SUCCESS);

error:
    HD_exit(EXIT_FAILURE);
    return 1;
}

