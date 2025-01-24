
#ifndef _DM_API_H_
#define _DM_API_H_

#ifdef __cplusplus
extern "C"
{
#endif


extern int daserrno;

/**
 * \page dm_params dm_params
 * Use this function to query the DM about its effective (in-memory)
 * configuration parameters.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * struct dmParams* dm_params(int reconnect)</b>
 *
 * @param reconnect
 * TRUE or FALSE
 *
 * <b>Data Structures:</b>
 * \par
 * struct dmParams
 * \n
 *
 * @return struct dmParams *:A dynamically allocated dmParams structure
 * containing the configuration parameters of the queried DM. 
 * The caller is responsible for freeing all sub-structures
 * and string buffers in the structure as well as the structure itself.
 * \n Function was successful.
 *
 * @return NULL 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 *
 * @see \ref freeDmParams
 */
extern struct dmParams* dm_params(int);

/**
 * \page dm_admin dm_admin
 * Use this function to notify the dmd on the specified host to reconfigure
 * or shut down.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int dm_admin(bdataSubcommandOptionID subcmd, char* hostname)</b>
 *
 * @param subcmd
 * BDATA_ADMIN_RECONFIG or BDATA_ADMIN_SHUTDOWN.
 *
 * @param *hostname
 * The host name where the dmd lives.
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 */
extern int dm_admin(bdataSubcommandOptionID, char*);

/**
 * \page dm_clusters dm_clusters
 * Use this function to query the DM about its connections. Obtains
 * information about connected mbatchd daemons, with master DM host names,
 * their status, and the outgoing and incoming connections for remote DMs.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * struct queryClustersReply *dm_clusters(void)</b>
 *
 * <b>Parameters:</b>
 * \par
 * none
 * \n
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct queryClustersReply
 *
 * @return struct  queryClustersReply *:A dynamically allocated
 * queryClustersReply structure  containing the clusters information
 * from queried dmd. The caller is responsible for freeing all sub-structures
 * and string buffers in the structure as well as the  structure itself.
 * \n Function was successful.
 *
 * @return NULL
 * \n Function failed. 
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 */
extern struct queryClustersReply* dm_clusters();

/**
 * \page dm_cache dm_cache
 * This function is used for querying the DM for files or jobs information
 * in the staging area.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * struct dmCacheResult* dm_cache(struct dmCacheRequest *cacheQuery,
 *                                struct remoteDMs *remoteDMD)</b>
 *
 * @param *cacheQuery
 * cache query request structure
 *
 * @param *remoteDMD
 * The remote DMD to query, NULL for local.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct dmCacheResult
 * \n\ref struct dmCacheRequest
 * \n\ref struct remoteDMs
 *
 * @return struct dmCacheResult *:A dynamically allocated dmCacheResult
 * structure that contains the query results. The caller is responsible for
 * freeing all sub-structures and string buffers in the structure
 * as well as the structure itself.
 * \n Function was successful.
 *
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 */
extern struct dmCacheResult* dm_cache(struct dmCacheRequest*, struct remoteDMs*);

/**
 * \page dm_local dm_local
 * Use this function to query the local DM to obtain the DM candidate
 * host list, current DM master host, and the DM listening port.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int dm_local(char** hostList, char** masterHost, int* port)</b>
 *
 * @param **hostList
 * Returns a list of master and candidate DM hosts.
 *
 * @param **masterHost
 * Returns the current master DM host.
 *
 * @param *port 
 * Returns the listening port of the current master DM host.
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 */
extern int dm_local(char**, char**, int*);

/**
 * \page dm_stage_in dm_stage_in
 * Use this function to perform stage-in operations. If this function
 * is used by an application that is not running as an LSF job, the function
 * fails.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int dm_stage_in(struct dmStginRequest* stginReq, char **message)</b>
 *
 * @param *stginReq
 * stage-in request structure
 *
 * @param **message 
 * A string to give extra information about the error, this can be NULL
 * if you don't care
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct dmStginRequest
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error. 
 * The 'message' argument if initially not NULL; the caller is
 * responsible for freeing the memory (dynamically allocated in
 * the function).
 */
extern int dm_stage_in(struct dmStginRequest*, char **);

/**
 * \page dm_stage_out dm_stage_out
 * Use this function to perform stage-out operations. If this function
 * is used by an application that is not running as an LSF job, the function
 * fails.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int dm_stage_out(struct dmStgoutRequest* stgoutReq, char **message)</b>
 *
 * @param *stgoutReq [IN]
 * stage-out request structure
 *
 * @param **message
 * A string to provide more information about the error, this can be NULL
 * if it does not matter.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct dmStgoutRequest
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error. 
 * The 'message' argument if initially not NULL; the caller is
 * responsible for freeing the memory (dynamically allocated in
 * the function).
 */
extern int dm_stage_out(struct dmStgoutRequest*, char **);

/**
 * \page dm_list_tags dm_list_tags
 * Use this function to query the DM for a list of tags contained in
 * its staging area.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * struct dmTagInfo *dm_list_tags(char *user,
 *                               int *numTags,
 *                               struct remoteDMs *remoteDMD)</b>
 *
 * @param *user
 * The user name (can be "all"), this is optional.
 *
 * @param *numTags
 * The number of entries in the returned tags array.
 *
 * @param *remoteDMD
 * The remote cluster, this is optional.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct remoteDMs
 * \n\ref struct dmTagInfo
 *
 * @return struct dmTagInfo *:An array of tags info structures for tags that
 * are found in the DM's staging area. This memory is dynamically allocated
 * inside the library and it is the caller's responsibility to free that
 * memory
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 */
extern struct dmTagInfo* dm_list_tags(char *, int*, struct remoteDMs*);

/**
 * \page dm_delete_tag dm_delete_tag
 * Use this function to delete a tag in the DM's staging area. The tag
 * directory and all the contents inside it are deleted. If the 'user'
 * argument is NULL, DMD tries to delete only those tags that belong
 * to the calling user. If not NULL, deletes tags of the specified user.
 * This option can be specified only by DM administrators.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int dm_delete_tag(char *tagName,
 *                   char *user,
 *                   struct remoteDMs *remoteDMD)</b>
 *
 * @param *tagName
 * The tag name, this must be specified.
 *
 * @param *user
 * The user name, this is optional.
 *
 * @param *remoteDMD
 * The remote cluster, this is optional.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct remoteDMs
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 */
extern int dm_delete_tag(char *, char *, struct remoteDMs *);

/**
 * \page dm_chgrp dm_chgrp
 * This function is used to change the group of file or tag in the DM's
 * staging area to 'groupName'. If tagName is set, hostFile must be NULL,
 * or if hostFile is set, tagName must be NULL. If CACHE_ACCESS_CONTROL=Y is
 * not configured, this function fails.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int dm_chgrp(char *groupName,
 *              char *tagName,
 *              char *hostFile,
 *              struct remoteDMs *remoteDMD,
 *              int options,
 *              struct dmChPartialSuccess *out)</b>
 *
 * @param *groupName
 * The group name, must be specified.
 *
 * @param *tagName
 * The tag name.
 *
 * @param *hostFile
 * Specified data requirement. The format is "hostname:/full/file/path".
 *
 * @param options
 * Currently not used; set to 0.
 *
 * @param *out
 * Populated if it was a partial success, can set to NULL if this does
 * not matter.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct dmChPartialSuccess
 * \n\ref struct remoteDMs
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 * If daserrno is DAS_CHGRP_PARTIAL the 'out' object, if provided,
 * is set. 
 */
extern int dm_chgrp(char *, char *, char *, struct remoteDMs *,
		    int, struct dmChPartialSuccess *);

/**
 * \page dm_chmod dm_chmod
 * Use this function to change the file mode bits of the file or tag in the
 * DM's staging area to 'mode'. If tagName is set, hostFile must be NULL,
 * or if hostFile is set, tagName must be NULL. If CACHE_ACCESS_CONTROL=Y is
 * not configured, this function fails.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int dm_chmod(char *mode,
 *              char *tagName,
 *              char *hostFile,
 *              struct remoteDMs *remoteDMD,
 *              int options,
 *              struct dmChPartialSuccess *out)</b>
 *
 * @param *mode
 * Must be specified, 3 digit octal number.
 *
 * @param *tagName
 * The tag name.
 *
 * @param *hostFile
 * Specified data requirement, format is "hostname:/full/file/path".
 *
 * @param options
 * Currently not used; set to 0.
 *
 * @param *out
 * Populated if it was a partial success, can be set to NULL if this does
 * not matter.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct dmChPartialSuccess
 * \n\ref struct remoteDMs 
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return -1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 * If daserrno is DAS_CHMOD_PARTIAL the 'out' object, if provided,
 * is set. 
 */
extern int dm_chmod(char *, char *, char *, struct remoteDMs *,
		    int, struct dmChPartialSuccess *);

/**
 * \page getRegisteredDmdCluster getRegisteredDmdCluster
 * This is a helper function that is used when a 'struct remoteDMs' is needed
 * by some of the other APIs for communicating with a remote DM. The remote
 * DM must be known to the local DM for this function to succeed.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int getRegisteredDmdCluster(char *clusterName,
 *                             struct remoteDMs **remoteDMs)</b>
 *
 * @param *clusterName
 * Remote cluster name, the remote cluster that is the eventual
 * communication target.
 *
 * @param **remoteDMs   
 * A dynamically-allocated 'struct remoteDMs'.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct remoteDMs
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return < 0
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, daserrno is set to indicate the error.
 */
extern int getRegisteredDmdCluster(char *, struct remoteDMs **);

/**
 * \page das_strerror das_strerror
 * This function returns a pointer to a string that describes the error code
 * that is passed in. Do not have the caller free the returned pointer.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * char *das_strerror(int errcode)</b>
 *
 * @param errcode 
 * One of the values of 'enum dasErrCode'.
 *
 * @return char *:Return the appropriate error description string, or an
 * empty string if the error number is unknown.
 */
extern char *das_strerror(int);

/**
 * \page das_perror das_perror
 * Print an error message to stderr.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * void das_perror(int errcode, const char *msg)</b>
 *
 * @param errcode
 * One of the values of 'enum dasErrCode'.
 *
 * @param *msg
 * An optional message to be added to the error message.
 *
 * @return void:This function returns no value.
 */
extern void das_perror(int, const char *);

/**
 * \page das_init das_init
 * Used to initialize the DM library. Generally only needed before calling
 * \ref dm_params.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * int das_init(int options)</b>
 *
 * @param options
 * This is one or more of DAS_INIT_OPEN_LOGFILE,
 * DAS_INIT_WRITE_STDERR, DAS_INIT_CLIENT_SIDE,
 * DAS_INIT_LSFCONF_ONLY, and DAS_INIT_GET_PARAMS_FROM_DMD
 * 'or'-ed together. Most of the time set options to DAS_INIT_LSFCONF_ONLY.
 *
 * @return DAS_NO_ERROR
 * \n Function was successful.
 *
 * @return !DAS_NO_ERROR:One of the values of 'enum dasErrCode'.
 * \n Function failed.
 */
extern int das_init(int);

/**
 * \page das_reset das_reset
 * Close any connections to the DM.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * void das_reset(void)</b>
 *
 * <b>Parameters:</b>
 * \par
 * none
 * 
 * @return void:This function returns no value.
 */
extern void das_reset(void);

/**
 * \page freeDmParams freeDmParams
 * Use this function to free a dynamically-allocated dmParams structure.
 *
 * <b>\#include <lsf/dm.def.h>\n
 * #include <lsf/dm.api.h>
 *
 * void freeDmParams(struct dmParams **ppDasParams)</b>
 *
 * @param **ppDasParams
 * The pointer pointed to is set to NULL after the call.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref struct dmParams
 *
 * @return void:This function returns no value.
 */
extern void freeDmParams(struct dmParams **ppDasParams);


#ifdef __cplusplus
}
#endif

#endif /* _DM_API_H_ */

/**
 * \page daslib daslib
 * \brief Application Programming Interface (API) library functions for DM
 *
 * \note 
 * \par 
 * All DASLIB APIs require that the header file <lsf/dm.def.h> and <lsf/dm.api.h> be included. 
 * 
 * \b Files:
 * \par 
 * LSF_CONFDIR/lsf.datamanager.<clustername> 
 *
 * @see daslibapis
 */
