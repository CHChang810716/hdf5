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

#if !(defined H5O_FRIEND || defined H5O_MODULE)
#error "Do not include this file outside the H5O package!"
#endif

#ifndef _H5Opkg_H
#define _H5Opkg_H

/* Get package's private header */
#include "H5Oprivate.h"		/* Object headers		  	*/

/* Other private headers needed by this file */
#include "H5ACprivate.h"        /* Metadata cache                       */
#include "H5FLprivate.h"	/* Free Lists                           */

/* Object header macros */
#define H5O_NMESGS	8 		/*initial number of messages	     */
#define H5O_NCHUNKS	2		/*initial number of chunks	     */
#define H5O_MIN_SIZE	22		/* Min. obj header data size (must be big enough for a message prefix and a continuation message) */
#define H5O_MSG_TYPES   26              /* # of types of messages            */
#define H5O_MAX_CRT_ORDER_IDX 65535     /* Max. creation order index value   */

/* Versions of object header structure */

/* Initial version of the object header format */
#define H5O_VERSION_1		1

/* Revised version - leaves out reserved bytes and alignment padding, and adds
 *      magic number as prefix and checksum as suffix for all chunks.
 */
#define H5O_VERSION_2		2

/* The latest version of the format.  Look through the 'flush'
 *      and 'size' callback for places to change when updating this. */
#define H5O_VERSION_LATEST	H5O_VERSION_2

/* This is the initial size of the dynamically allocated list of object 
 * header continuation chunk flush dependency parents maintained by the 
 * object header proxy.  
 *
 * The current value of 1 presumes that the typical number of entries 
 * on this list is almost always either zero or 1.  Increase this value 
 * if appropriate.
 */
#define H5O_FD_PAR_LIST_BASE	1


/*
 * Align messages on 8-byte boundaries because we would like to copy the
 * object header chunks directly into memory and operate on them there, even
 * on 64-bit architectures.  This allows us to reduce the number of disk I/O
 * requests with a minimum amount of mem-to-mem copies.
 *
 * Note: We no longer attempt to do this. - QAK, 10/16/06
 */
#define H5O_ALIGN_OLD(X)	(8 * (((X) + 7) / 8))
#define H5O_ALIGN_VERS(V, X)						      \
    (((V) == H5O_VERSION_1) ?						      \
		H5O_ALIGN_OLD(X)					      \
        :								      \
		(X)							      \
    )
#define H5O_ALIGN_OH(O, X)						      \
     H5O_ALIGN_VERS((O)->version, X)
#define H5O_ALIGN_F(F, X)						      \
     H5O_ALIGN_VERS((H5F_USE_LATEST_FLAGS(F, H5F_LATEST_OBJ_HEADER) ? H5O_VERSION_LATEST : H5O_VERSION_1), X)

/* Size of checksum (on disk) */
#define H5O_SIZEOF_CHKSUM               4

/* ========= Object Creation properties ============ */
/* Default values for some of the object creation properties */
/* NOTE: The H5O_CRT_ATTR_MAX_COMPACT_DEF & H5O_CRT_ATTR_MIN_DENSE_DEF values
 *      are "built into" the file format, make certain existing files with
 *      default attribute phase change storage are handled correctly if they
 *      are changed.
 */
#define H5O_CRT_ATTR_MAX_COMPACT_DEF    8
#define H5O_CRT_ATTR_MIN_DENSE_DEF      6
#define H5O_CRT_OHDR_FLAGS_DEF          H5O_HDR_STORE_TIMES

/* Object header status flag definitions */
#define H5O_HDR_CHUNK0_1                0x00    /* Use 1-byte value for chunk #0 size */
#define H5O_HDR_CHUNK0_2                0x01    /* Use 2-byte value for chunk #0 size */
#define H5O_HDR_CHUNK0_4                0x02    /* Use 4-byte value for chunk #0 size */
#define H5O_HDR_CHUNK0_8                0x03    /* Use 8-byte value for chunk #0 size */

/*
 * Size of object header prefix.
 */
#define H5O_SIZEOF_HDR(O)						      \
    (((O)->version == H5O_VERSION_1)  					      \
        ?								      \
            H5O_ALIGN_OLD(1 +	/*version number	*/		      \
                1 +		/*reserved 		*/		      \
                2 +		/*number of messages	*/		      \
                4 +		/*reference count	*/		      \
                4)		/*chunk data size	*/		      \
        :								      \
            (H5_SIZEOF_MAGIC +	/*magic number  	*/		      \
                1 +		/*version number 	*/		      \
                1 +		/*flags		 	*/		      \
                (((O)->flags & H5O_HDR_STORE_TIMES) ? (			      \
                  4 +		/*access time		*/		      \
                  4 +		/*modification time	*/		      \
                  4 +		/*change time		*/		      \
                  4		/*birth time		*/		      \
                ) : 0) +						      \
                (((O)->flags & H5O_HDR_ATTR_STORE_PHASE_CHANGE) ? (	      \
                  2 +		/*max compact attributes */		      \
                  2		/*min dense attributes	*/		      \
                ) : 0) +						      \
                (1 << ((O)->flags & H5O_HDR_CHUNK0_SIZE)) + /*chunk 0 data size */ \
                H5O_SIZEOF_CHKSUM) /*checksum size	*/		      \
    )

/*
 * Size of object header message prefix
 */
#define H5O_SIZEOF_MSGHDR_VERS(V,C)					      \
    (((V) == H5O_VERSION_1)  						      \
        ?								      \
            H5O_ALIGN_OLD(2 +	/*message type		*/		      \
                2 +		/*sizeof message data	*/		      \
                1 +		/*flags              	*/		      \
                3)		/*reserved		*/		      \
        :								      \
            (1 +		/*message type		*/		      \
                2 + 		/*sizeof message data	*/		      \
                1 +		/*flags              	*/		      \
                ((C) ? (						      \
                  2		/*creation index     	*/		      \
                ) : 0))							      \
    )
#define H5O_SIZEOF_MSGHDR_OH(O)						      \
    H5O_SIZEOF_MSGHDR_VERS((O)->version, (O)->flags & H5O_HDR_ATTR_CRT_ORDER_TRACKED)
#define H5O_SIZEOF_MSGHDR_F(F, C)						      \
    H5O_SIZEOF_MSGHDR_VERS((H5F_USE_LATEST_FLAGS(F, H5F_LATEST_OBJ_HEADER) || H5F_STORE_MSG_CRT_IDX(F)) ? H5O_VERSION_LATEST : H5O_VERSION_1, (C))

/*
 * Size of chunk "header" for each chunk
 */
#define H5O_SIZEOF_CHKHDR_VERS(V)					      \
    (((V) == H5O_VERSION_1)  						      \
        ?								      \
            0 +		/*no magic #  */				      \
                0 	/*no checksum */				      \
        :								      \
            H5_SIZEOF_MAGIC + 		/*magic #  */			      \
                H5O_SIZEOF_CHKSUM 	/*checksum */			      \
    )
#define H5O_SIZEOF_CHKHDR_OH(O)						      \
    H5O_SIZEOF_CHKHDR_VERS((O)->version)

/*
 * Size of checksum for each chunk
 */
#define H5O_SIZEOF_CHKSUM_VERS(V)					      \
    (((V) == H5O_VERSION_1)  						      \
        ?								      \
            0 		/*no checksum */				      \
        :								      \
            H5O_SIZEOF_CHKSUM 		/*checksum */			      \
    )
#define H5O_SIZEOF_CHKSUM_OH(O)						      \
    H5O_SIZEOF_CHKSUM_VERS((O)->version)

/* Input/output flags for decode functions */
#define H5O_DECODEIO_NOCHANGE           0x01u   /* IN: do not modify values */
#define H5O_DECODEIO_DIRTY              0x02u   /* OUT: message has been changed */

/* Macro to incremend ndecode_dirtied (only if we are debugging) */
#ifndef NDEBUG
#define INCR_NDECODE_DIRTIED(OH) (OH)->ndecode_dirtied++;
#else /* NDEBUG */
#define INCR_NDECODE_DIRTIED(OH) ;
#endif /* NDEBUG */

/* Load native information for a message, if it's not already present */
/* (Only works for messages with decode callback) */
#define H5O_LOAD_NATIVE(F, DXPL, IOF, OH, MSG, ERR)                           \
    if(NULL == (MSG)->native) {                                               \
        const H5O_msg_class_t	*msg_type = (MSG)->type;                      \
        unsigned                ioflags = (IOF);                              \
                                                                              \
        /* Decode the message */                                              \
        HDassert(msg_type->decode);                                           \
        if(NULL == ((MSG)->native = (msg_type->decode)((F), (DXPL), (OH), (MSG)->flags, &ioflags, (MSG)->raw))) \
            HGOTO_ERROR(H5E_OHDR, H5E_CANTDECODE, ERR, "unable to decode message") \
                                                                              \
        /* Mark the message dirty if it was changed by decoding */            \
        if((ioflags & H5O_DECODEIO_DIRTY) && (H5F_get_intent((F)) & H5F_ACC_RDWR)) { \
            (MSG)->dirty = TRUE;                                              \
            /* Increment the count of messages dirtied by decoding, but */    \
            /* only ifndef NDEBUG */                                          \
            INCR_NDECODE_DIRTIED(OH)                                          \
        }                                                                     \
                                                                              \
        /* Set the message's "shared info", if it's shareable */	      \
        if((MSG)->flags & H5O_MSG_FLAG_SHAREABLE) {                           \
            HDassert(msg_type->share_flags & H5O_SHARE_IS_SHARABLE);          \
            H5O_UPDATE_SHARED((H5O_shared_t *)(MSG)->native, H5O_SHARE_TYPE_HERE, (F), msg_type->id, (MSG)->crt_idx, (OH)->chunk[0].addr) \
        } /* end if */                                                        \
                                                                              \
        /* Set the message's "creation index", if it has one */		      \
        if(msg_type->set_crt_index) {				      	      \
            /* Set the creation index for the message */		      \
            if((msg_type->set_crt_index)((MSG)->native, (MSG)->crt_idx) < 0)  \
                HGOTO_ERROR(H5E_OHDR, H5E_CANTSET, ERR, "unable to set creation index") \
        } /* end if */							      \
    } /* end if */

/* Flags for a message class's "sharability" */
#define H5O_SHARE_IS_SHARABLE   0x01
#define H5O_SHARE_IN_OHDR       0x02


/* The "message class" type */
struct H5O_msg_class_t {
    unsigned	id;				/*message type ID on disk   */
    const char	*name;				/*for debugging             */
    size_t	native_size;			/*size of native message    */
    unsigned    share_flags;			/* Message sharing settings */
    void	*(*decode)(H5F_t *, hid_t, H5O_t *, unsigned, unsigned *, const uint8_t *);
    herr_t	(*encode)(H5F_t *, hbool_t, uint8_t *, const void *);
    void	*(*copy)(const void *, void *);	/*copy native value         */
    size_t	(*raw_size)(const H5F_t *, hbool_t, const void *);/*sizeof encoded message	*/
    herr_t	(*reset)(void *);		/*free nested data structs  */
    herr_t	(*free)(void *);		/*free main data struct  */
    herr_t	(*del)(H5F_t *, hid_t, H5O_t *, void *);    /* Delete space in file referenced by this message */
    herr_t	(*link)(H5F_t *, hid_t, H5O_t *, void *);   /* Increment any links in file reference by this message */
    herr_t	(*set_share)(void*, const H5O_shared_t*);   /* Set shared information */
    htri_t	(*can_share)(const void *);	/* Is message allowed to be shared? */
    herr_t	(*pre_copy_file)(H5F_t *, const void *, hbool_t *, const H5O_copy_t *, void *); /*"pre copy" action when copying native value to file */
    void	*(*copy_file)(H5F_t *, void *, H5F_t *, hbool_t *, unsigned *, H5O_copy_t *, void *, hid_t); /*copy native value to file */
    herr_t	(*post_copy_file)(const H5O_loc_t *, const void *, H5O_loc_t *, void *, unsigned *, hid_t, H5O_copy_t *); /*"post copy" action when copying native value to file */
    herr_t      (*get_crt_index)(const void *, H5O_msg_crt_idx_t *);	/* Get message's creation index */
    herr_t      (*set_crt_index)(void *, H5O_msg_crt_idx_t);	/* Set message's creation index */
    herr_t	(*debug)(H5F_t*, hid_t, const void*, FILE*, int, int);
};

struct H5O_mesg_t {
    const H5O_msg_class_t	*type;	/*type of message		     */
    hbool_t		dirty;		/*raw out of date wrt native	     */
    hbool_t		locked;		/*message is locked into chunk	     */
    uint8_t		flags;		/*message flags			     */
    H5O_msg_crt_idx_t   crt_idx;        /*message creation index	     */
    unsigned		chunkno;	/*chunk number for this mesg	     */
    void		*native;	/*native format message		     */
    uint8_t		*raw;		/*ptr to raw data		     */
    size_t		raw_size;	/*size with alignment		     */
};

typedef struct H5O_chunk_t {
    haddr_t	addr;			/*chunk file address		     */
    size_t	size;			/*chunk size			     */
    size_t	gap;			/*space at end of chunk too small for null message */
    uint8_t	*image;			/*image of file			     */
} H5O_chunk_t;

struct H5O_t {
    H5AC_info_t cache_info; /* Information for metadata cache functions, _must_ be */
                            /* first field in structure */

    /* File-specific information (not stored) */
    size_t      sizeof_size;            /* Size of file sizes 		     */
    size_t      sizeof_addr;            /* Size of file addresses	     */
    hbool_t     swmr_write;             /* Whether we are doing SWMR writes  */

    /* Debugging information (not stored) */
#ifdef H5O_ENABLE_BAD_MESG_COUNT
    hbool_t     store_bad_mesg_count;   /* Flag to indicate that a bad message count should be stored */
                                        /* (This is to simulate a bug in earlier
                                         *      versions of the library)
                                         */
#endif /* H5O_ENABLE_BAD_MESG_COUNT */
#ifndef NDEBUG
    size_t      ndecode_dirtied;        /* Number of messages dirtied by decoding */
#endif /* NDEBUG */

    /* Chunk management information (not stored) */
    size_t      rc;                     /* Reference count of [continuation] chunks using this structure */
    size_t      chunk0_size;            /* Size of serialized first chunk    */
    hbool_t     mesgs_modified;         /* Whether any messages were modified when the object header was deserialized */
    hbool_t     prefix_modified;        /* Whether prefix was modified when the object header was deserialized */

    /* Object information (stored) */
    hbool_t     has_refcount_msg;       /* Whether the object has a ref. count message */
    unsigned	nlink;			/*link count			     */
    uint8_t	version;		/*version number		     */
    uint8_t	flags;			/*flags				     */

    /* Time information (stored, for versions > 1 & H5O_HDR_STORE_TIMES flag set) */
    time_t      atime;                  /*access time 			     */
    time_t      mtime;                  /*modification time 		     */
    time_t      ctime;                  /*change time 			     */
    time_t      btime;                  /*birth time 			     */

    /* Attribute information (stored, for versions > 1) */
    unsigned	max_compact;		/* Maximum # of compact attributes   */
    unsigned	min_dense;		/* Minimum # of "dense" attributes   */

    /* Message management (stored, encoded in chunks) */
    size_t	nmesgs;			/*number of messages		     */
    size_t	alloc_nmesgs;		/*number of message slots	     */
    H5O_mesg_t	*mesg;			/*array of messages		     */
    size_t      link_msgs_seen;         /* # of link messages seen when loading header */
    size_t      attr_msgs_seen;         /* # of attribute messages seen when loading header */

    /* Chunk management (not stored) */
    size_t	nchunks;		/*number of chunks		     */
    size_t	alloc_nchunks;		/*chunks allocated		     */
    H5O_chunk_t *chunk;			/*array of chunks		     */

    /* Object header proxy information (not stored) */
    haddr_t     proxy_addr;             /* Temporary address of object header proxy */
    hbool_t     proxy_present;          /* Whether the proxy is present in cache (and we have to track dependencies) */
};

/* Class for types of objects in file */
typedef struct H5O_obj_class_t {
    H5O_type_t	type;				/*object type on disk	     */
    const char	*name;				/*for debugging		     */
    void       *(*get_copy_file_udata)(void);	/*retrieve user data for 'copy file' operation */
    void	(*free_copy_file_udata)(void *); /*free user data for 'copy file' operation */
    htri_t	(*isa)(H5O_t *);		/*if a header matches an object class */
    hid_t	(*open)(const H5G_loc_t *, hid_t, hid_t, hbool_t );	/*open an object of this class */
    void	*(*create)(H5F_t *, void *, H5G_loc_t *, hid_t );	/*create an object of this class */
    H5O_loc_t	*(*get_oloc)(hid_t );		/*get the object header location for an object */
    herr_t      (*bh_info)(const H5O_loc_t *loc, hid_t dxpl_id, H5O_t *oh, H5_ih_info_t *bh_info); /*get the index & heap info for an object */
    herr_t      (*flush)(void *obj_ptr, hid_t dxpl_id); /*flush an opened object of this class */
} H5O_obj_class_t;

/* Node in skip list to map addresses from one file to another during object header copy */
typedef struct H5O_addr_map_t {
    H5_obj_t    src_obj_pos;            /* Location of source object */
    haddr_t     dst_addr;               /* Address of object in destination file */
    hbool_t     is_locked;              /* Indicate that the destination object is locked currently */
    hsize_t     inc_ref_count;          /* Number of deferred increments to reference count */
    const H5O_obj_class_t *obj_class;   /* Object class */
    void        *udata;                 /* Object class copy file udata */
} H5O_addr_map_t;

/* Stack of continuation messages to interpret */
typedef struct H5O_cont_msgs_t {
    size_t      nmsgs;                  /* Number of continuation messages found so far */
    size_t	alloc_nmsgs;		/* Continuation messages allocated */
    H5O_cont_t *msgs;			/* Array of continuation messages */
} H5O_cont_msgs_t;

/* Common callback information for loading object header prefix from disk */
typedef struct H5O_common_cache_ud_t {
    H5F_t *f;                           /* Pointer to file for object header/chunk */
    hid_t dxpl_id;                      /* DXPL for operation */
    unsigned file_intent;               /* Read/write intent for file */
    unsigned merged_null_msgs;          /* Number of null messages merged together */
    hbool_t mesgs_modified;             /* Whether any messages were modified when the object header was deserialized */
    H5O_cont_msgs_t *cont_msg_info;     /* Pointer to continuation messages to work on */
    haddr_t addr;                       /* Address of the prefix or chunk */
} H5O_common_cache_ud_t;

/* Callback information for loading object header prefix from disk */
typedef struct H5O_cache_ud_t {
    hbool_t made_attempt;               /* Whether the deserialize routine was already attempted */
    unsigned v1_pfx_nmesgs;             /* Number of messages from v1 prefix header */
    uint8_t     version;                /* Version number obtained in get_load_size callback.
					 * It will be used later in verify_chksum callback
					 */ 
    H5O_common_cache_ud_t common;       /* Common object header cache callback info */
} H5O_cache_ud_t;

/* Structure representing each chunk in the cache */
typedef struct H5O_chunk_proxy_t {
    H5AC_info_t cache_info;    /* Information for metadata cache functions, _must_ be */
                                /* first field in structure */

    H5F_t *f;                           /* Pointer to file for object header/chunk */
    H5O_t       *oh;                    /* Object header for this chunk */
    unsigned    chunkno;                /* Chunk number for this chunk */
    unsigned    cont_chunkno;           /* Chunk number for the chunk containing the continuation message that points to this chunk */

    /* Flush depencency parent information (not stored) 
     *
     * The following fields are used to store the base address and a pointer 
     * to the in core representation of the chunk proxy's flush dependency
     * parent -- if it exists.  If it does not exist, these fields will
     * contain HADDR_UNDEF and NULL respectively.
     *
     * If the file is opened in SWMR write mode, the flush dependency 
     * parent of the chunk proxy will be either its object header 
     * (if cont_chunkno == 0) or the chunk proxy indicated by the 
     * cont_chunkno field (if cont_chunkno > 0).
     *
     * Note that the flush dependency parent address is maintained purely
     * for sanity checking.  Once we are reasonably confident of the code,
     * it can be deleted or be maintained only in debug mode.
     */
    haddr_t fd_parent_addr;             /* Address of flush dependency parent
                                         * if any.  This field is initialized
                                         * to HADDR_UNDEF.  
                                         */
    void * fd_parent_ptr;               /* pointer to flush dependency parent
                                         * it it exists.  NULL otherwise.
                                         */
} H5O_chunk_proxy_t;

/* Callback information for loading object header chunk from disk */
typedef struct H5O_chk_cache_ud_t {
    hbool_t decoding;                   /* Whether the object header is being decoded */
    H5O_t *oh;                          /* Object header for this chunk */
    unsigned chunkno;                   /* Index of chunk being brought in (for re-loads) */
    size_t size;                        /* Size of chunk in the file */
    H5O_common_cache_ud_t common;       /* Common object header cache callback info */
} H5O_chk_cache_ud_t;

/* Metadata cache object header proxy type */
struct H5O_proxy_t {
    H5AC_info_t cache_info;             /* Information for H5AC cache functions, _must_ be */
                                        /* first field in structure */
    H5F_t *f;                           /* Pointer to file for object header/chunk */
    H5O_t *oh;                          /* Object header */

    /* Flush depencency parent information (not stored) 
     *
     * The following fields are used to store base addresses and pointers
     * to the in core representations of the object header proxy's flush 
     * dependency parents -- if they exist.
     *
     * At present, object header proxies may have two types of parents:
     *
     * 1) Exactly one object header.
     *
     * 2) Zero or more object header continuation chunks.
     *
     * The base address and pointer to the object header flush dependency 
     * parent are stored in the oh_fd_parent_addr and oh_fd_parent_ptr fields.
     * These fields are set to HADDR_UNDEF and NULL if there is no object 
     * header flush dependency parent.  Note that when defined, 
     * oh_fd_parent_ptr should point to the same object as oh.
     *
     * The number of object header continuation chunks (H5O_chunk_proxy_t) 
     * that are flush dependency parents of the object header proxy is stored
     * in chk_fd_parent_count.  
     *
     * If this field is greater than zero, chk_fd_parent_addrs must point to 
     * a dynamically allocated array of haddr_t of length chk_fd_parent_alloc, 
     * and chk_fd_parent_ptrs must point to a dynamically allocated array of 
     * void * of the same length.  These arrays are used to store the base 
     * addresses and pointers to the object header continuation chunk flush 
     * dependency parents of the object header proxy.  chk_fd_parent_alloc
     * must always be greater than or equal to chk_fd_parent_count.
     *
     * If chk_fd_parent_count is zero, chk_fd_parent_addrs and 
     * chk_fd_parent_ptrs must be NULL.
     *
     * Note that the flush dependency parent addresses are maintined 
     * purely for sanity checking.  Once we are confident of the code,
     * these fields and their supporting code can be either deleted 
     * on maintained only in debug builds.
     */
    haddr_t oh_fd_parent_addr;
    void * oh_fd_parent_ptr;

    unsigned chk_fd_parent_count;
    unsigned chk_fd_parent_alloc;
    haddr_t *chk_fd_parent_addrs;
    void **chk_fd_parent_ptrs;
};

/* Callback information for loading object header proxy */
typedef struct H5O_proxy_cache_ud_t {
    H5F_t *f;                           /* Pointer to file for object header/chunk */
    H5O_t *oh;                          /* Object header for this chunk */
} H5O_proxy_cache_ud_t;

/* H5O object header inherits cache-like properties from H5AC */
H5_DLLVAR const H5AC_class_t H5AC_OHDR[1];

/* H5O object header chunk inherits cache-like properties from H5AC */
H5_DLLVAR const H5AC_class_t H5AC_OHDR_CHK[1];

/* Header message ID to class mapping */
H5_DLLVAR const H5O_msg_class_t *const H5O_msg_class_g[H5O_MSG_TYPES];

/* Declare external the free list for H5O_t's */
H5FL_EXTERN(H5O_t);

/* Declare external the free list for H5O_mesg_t sequences */
H5FL_SEQ_EXTERN(H5O_mesg_t);

/* Declare external the free list for H5O_chunk_t sequences */
H5FL_SEQ_EXTERN(H5O_chunk_t);

/* Declare external the free list for chunk_image blocks */
H5FL_BLK_EXTERN(chunk_image);

/*
 * Object header messages
 */

/* Null Message. (0x0000) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_NULL[1];

/* Simple Dataspace Message. (0x0001) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_SDSPACE[1];

/* Link Information Message. (0x0002) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_LINFO[1];

/* Datatype Message. (0x0003) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_DTYPE[1];

/* Old Fill Value Message. (0x0004) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_FILL[1];

/* New Fill Value Message. (0x0005) */
/*
 * The new fill value message is fill value plus
 * space allocation time and fill value writing time and whether fill
 * value is defined.
 */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_FILL_NEW[1];

/* Link Message. (0x0006) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_LINK[1];

/* External File List Message. (0x0007) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_EFL[1];

/* Data Layout Message. (0x0008) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_LAYOUT[1];

#ifdef H5O_ENABLE_BOGUS
/* "Bogus valid" Message. (0x0009) */
/* "Bogus invalid" Message. (0x0019) */
/*
 * Used for debugging - should never be found in valid HDF5 file.
 */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_BOGUS_VALID[1];
H5_DLLVAR const H5O_msg_class_t H5O_MSG_BOGUS_INVALID[1];
#endif /* H5O_ENABLE_BOGUS */

/* Group Information Message. (0x000a) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_GINFO[1];

/* Filter pipeline message. (0x000b) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_PLINE[1];

/* Attribute Message. (0x000c) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_ATTR[1];

/* Object name message. (0x000d) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_NAME[1];

/* Modification Time Message. (0x000e) */
/*
 * The message is just a `time_t'.
 * (See also the "new" modification time message)
 */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_MTIME[1];

/* Shared Message information message (0x000f)
 * A message for the superblock extension, holding information about
 * the file-wide shared message "SOHM" table
 */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_SHMESG[1];

/* Object Header Continuation Message. (0x0010) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_CONT[1];

/* Symbol Table Message. (0x0011) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_STAB[1];

/* New Modification Time Message. (0x0012) */
/*
 * The message is just a `time_t'.
 */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_MTIME_NEW[1];

/* v1 B-tree 'K' value message (0x0013)
 * A message for the superblock extension, holding information about
 * the file-wide v1 B-tree 'K' values.
 */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_BTREEK[1];

/* Driver info message (0x0014)
 * A message for the superblock extension, holding information about
 * the file driver settings
 */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_DRVINFO[1];

/* Attribute Information Message. (0x0015) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_AINFO[1];

/* Reference Count Message. (0x0016) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_REFCOUNT[1];

/* Free-space Manager Info message. (0x0017) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_FSINFO[1];

/* Placeholder for unknown message. (0x0018) */
H5_DLLVAR const H5O_msg_class_t H5O_MSG_UNKNOWN[1];


/*
 * Object header "object" types
 */

/* Group Object. (H5O_TYPE_GROUP - 0) */
H5_DLLVAR const H5O_obj_class_t H5O_OBJ_GROUP[1];

/* Dataset Object. (H5O_TYPE_DATASET - 1) */
H5_DLLVAR const H5O_obj_class_t H5O_OBJ_DATASET[1];

/* Datatype Object. (H5O_TYPE_NAMED_DATATYPE - 2) */
H5_DLLVAR const H5O_obj_class_t H5O_OBJ_DATATYPE[1];


/* Package-local function prototypes */
H5_DLL herr_t H5O_msg_flush(H5F_t *f, H5O_t *oh, H5O_mesg_t *mesg);
H5_DLL herr_t H5O_flush_msgs(H5F_t *f, H5O_t *oh);
H5_DLL hid_t H5O_open_by_loc(const H5G_loc_t *obj_loc, hid_t lapl_id, hid_t dxpl_id, hbool_t app_ref);
H5_DLL herr_t H5O_delete_mesg(H5F_t *f, hid_t dxpl_id, H5O_t *open_oh, H5O_mesg_t *mesg);
H5_DLL const H5O_obj_class_t * H5O_obj_class(const H5O_loc_t *loc, hid_t dxpl_id);
H5_DLL int H5O_link_oh(H5F_t *f, int adjust, hid_t dxpl_id, H5O_t *oh, hbool_t *deleted);
H5_DLL herr_t H5O_inc_rc(H5O_t *oh);
H5_DLL herr_t H5O_dec_rc(H5O_t *oh);
H5_DLL herr_t H5O_free(H5O_t *oh);

/* Object header message routines */
H5_DLL herr_t H5O_msg_alloc(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    const H5O_msg_class_t *type, unsigned *mesg_flags, void *mesg,
    size_t *mesg_idx);
H5_DLL herr_t H5O_msg_append_real(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    const H5O_msg_class_t *type, unsigned mesg_flags, unsigned update_flags,
    void *mesg);
H5_DLL herr_t H5O_msg_write_real(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    const H5O_msg_class_t *type, unsigned mesg_flags, unsigned update_flags,
    void *mesg);
H5_DLL void *H5O_msg_free_real(const H5O_msg_class_t *type, void *mesg);
H5_DLL herr_t H5O_msg_free_mesg(H5O_mesg_t *mesg);
H5_DLL unsigned H5O_msg_count_real(const H5O_t *oh, const H5O_msg_class_t *type);
H5_DLL herr_t H5O_msg_remove_real(H5F_t *f, H5O_t *oh, const H5O_msg_class_t *type,
    int sequence, H5O_operator_t op, void *op_data, hbool_t adj_link, hid_t dxpl_id);
H5_DLL void *H5O_msg_copy_file(const H5O_msg_class_t *type, H5F_t *file_src,
    void *mesg_src, H5F_t *file_dst, hbool_t *recompute_size,
    unsigned *mesg_flags, H5O_copy_t *cpy_info, void *udata, hid_t dxpl_id);
H5_DLL herr_t H5O_msg_iterate_real(H5F_t *f, H5O_t *oh, const H5O_msg_class_t *type,
    const H5O_mesg_operator_t *op, void *op_data, hid_t dxpl_id);

/* Object header chunk routines */
H5_DLL herr_t H5O_chunk_add(H5F_t *f, hid_t dxpl_id, H5O_t *oh, unsigned idx,
    unsigned cont_chunkno);
H5_DLL H5O_chunk_proxy_t *H5O_chunk_protect(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    unsigned idx);
H5_DLL herr_t H5O_chunk_unprotect(H5F_t *f, hid_t dxpl_id,
    H5O_chunk_proxy_t *chk_proxy, hbool_t chk_dirtied);
H5_DLL herr_t H5O_chunk_update_idx(H5F_t *f, hid_t dxpl_id, H5O_t *oh, unsigned idx);
H5_DLL herr_t H5O_chunk_resize(H5O_t *oh, H5O_chunk_proxy_t *chk_proxy);
H5_DLL herr_t H5O_chunk_delete(H5F_t *f, hid_t dxpl_id, H5O_t *oh, unsigned idx);

/* Collect storage info for btree and heap */
H5_DLL herr_t H5O_attr_bh_info(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    H5_ih_info_t *bh_info);

/* Object header allocation routines */
H5_DLL herr_t H5O_alloc_msgs(H5O_t *oh, size_t min_alloc);
H5_DLL herr_t  H5O_alloc(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    const H5O_msg_class_t *type, const void *mesg, size_t *mesg_idx);
H5_DLL herr_t H5O_condense_header(H5F_t *f, H5O_t *oh, hid_t dxpl_id);
H5_DLL herr_t H5O_release_mesg(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    H5O_mesg_t *mesg, hbool_t adj_link);

/* Shared object operators */
H5_DLL void * H5O_shared_decode(H5F_t *f, hid_t dxpl_id, H5O_t *open_oh,
    unsigned *ioflags, const uint8_t *buf, const H5O_msg_class_t *type);
H5_DLL herr_t H5O_shared_encode(const H5F_t *f, uint8_t *buf/*out*/, const H5O_shared_t *sh_mesg);
H5_DLL size_t H5O_shared_size(const H5F_t *f, const H5O_shared_t *sh_mesg);
H5_DLL herr_t H5O_shared_delete(H5F_t *f, hid_t dxpl_id, H5O_t *open_oh,
    const H5O_msg_class_t *mesg_type, H5O_shared_t *sh_mesg);
H5_DLL herr_t H5O_shared_link(H5F_t *f, hid_t dxpl_id, H5O_t *open_oh,
    const H5O_msg_class_t *mesg_type, H5O_shared_t *sh_mesg);
H5_DLL herr_t H5O_shared_copy_file(H5F_t *file_src, H5F_t *file_dst,
    const H5O_msg_class_t *mesg_type, const void *_native_src, void *_native_dst,
    hbool_t *recompute_size, unsigned *mesg_flags, H5O_copy_t *cpy_info,
    void *udata, hid_t dxpl_id);
H5_DLL herr_t H5O_shared_post_copy_file (H5F_t *f,
    const H5O_msg_class_t *mesg_type, const H5O_shared_t *shared_src,
    H5O_shared_t *shared_dst, unsigned *mesg_flags, hid_t dxpl_id,
    H5O_copy_t *cpy_info);
H5_DLL herr_t H5O_shared_debug(const H5O_shared_t *mesg, FILE *stream,
    int indent, int fwidth);

/* Attribute message operators */
H5_DLL herr_t H5O_attr_reset(void *_mesg);
H5_DLL herr_t H5O_attr_delete(H5F_t *f, hid_t dxpl_id, H5O_t *open_oh, void *_mesg);
H5_DLL herr_t H5O_attr_link(H5F_t *f, hid_t dxpl_id, H5O_t *open_oh, void *_mesg);
H5_DLL herr_t H5O_attr_count_real(H5F_t *f, hid_t dxpl_id, H5O_t *oh,
    hsize_t *nattrs);

/* Object header proxy operators */
H5_DLL herr_t H5O__proxy_create(H5F_t *f, hid_t dxpl_id, H5O_t *oh);
H5_DLL H5O_proxy_t *H5O__proxy_pin(H5F_t *f, hid_t dxpl_id, H5O_t *oh);
H5_DLL herr_t H5O__proxy_unpin(H5O_proxy_t *proxy);
H5_DLL herr_t H5O__proxy_depend(H5F_t *f, hid_t dxpl_id, H5O_t *oh, void *parent);
H5_DLL herr_t H5O__proxy_undepend(H5F_t *f, hid_t dxpl_id, H5O_t *oh, void *parent);

/* Testing functions */
#ifdef H5O_TESTING
H5_DLL htri_t H5O_is_attr_empty_test(hid_t oid);
H5_DLL htri_t H5O_is_attr_dense_test(hid_t oid);
H5_DLL herr_t H5O_num_attrs_test(hid_t oid, hsize_t *nattrs);
H5_DLL herr_t H5O_attr_dense_info_test(hid_t oid, hsize_t *name_count, hsize_t *corder_count);
H5_DLL herr_t H5O_check_msg_marked_test(hid_t oid, hbool_t flag_val);
H5_DLL herr_t H5O_expunge_chunks_test(const H5O_loc_t *oloc, hid_t dxpl_id);
H5_DLL herr_t H5O_get_rc(const H5O_loc_t *oloc, hid_t dxpl_id, unsigned *rc);
#endif /* H5O_TESTING */

/* Object header debugging routines */
#ifdef H5O_DEBUG
H5_DLL herr_t H5O_assert(const H5O_t *oh);
#endif /* H5O_DEBUG */
H5_DLL herr_t H5O_debug_real(H5F_t *f, hid_t dxpl_id, H5O_t *oh, haddr_t addr, FILE *stream, int indent, int fwidth);

#endif /* _H5Opkg_H */

