/* $RCSfile: das.h,v $Revision: 1.1.2.32 $Date: 2014/03/30 21:00:28 $
 */
#ifndef _DM_DEF_H_
#define _DM_DEF_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <sys/socket.h>
#include <lsbatch.h>

/* API data structures */

/**
 * \brief used by dm_cache(), dm_list_tags(), dm_delete_tag(), dm_chgrp(), dm_chmod(), getRegisteredDmdCluster() APIs
 */
struct remoteDMs {
    char *clustername;  /**< lsf cluster name */
    char *servers;      /**< space separated list of host names where dmd runs */
    int port;           /**< port dmd is listening on */
};


/**
 * \brief dm_params() API data structure
 */
struct dmParams {
    char *admins;    /**< DM admins */
    char *hosts;     /**< DM hosts */
    int   port;      /**< DM listening port */
    char *stagingArea; /**< staging area path */
    int   cacheInputGracePeriod; /**< cache input grace period */ 
    int   cacheOutputGracePeriod; /**< cache output grace period */
    char *fileTransferCmd; /**< file transfer command */
    char *cachePermissions; /**< cache permissions */
    int   remoteDmdHeartbeatInterval; /**< remote dmd heartbeat interval */
    int   remoteCacheRefreshInterval; /**< remote cache refresh interval */
    int   remoteCacheSize; /**< remote cache size */
    int   localNJobs; /**< local number of jobs */
    int   localNFiles; /**< local number of files */
    int   localNTags; /**< local number of tags */
    int   queryNThreads; /**< number of query threads */
    int   inputFileProcessingNThreads; /**< number of input file process threads */
    int   outputFileProcessingNThreads; /**< number of output file process threads */
    int   recoveryNThreads; /**< number of recovery threads */
    int   recvTimeout; /**< receive timeout */
    int   connTimeout; /**< connection timeout */
    int   nRemoteDMs; /**< number of remote DMs */
    struct remoteDMs *remoteDMList; /**< info from RemoteDataManagers section */
    int   timeDmd; /**< dmd timing  */
    char *logDir; /**< log dir */
    char *logMask; /**< log mask */
    int cacheAccessibleFiles; /**< cache accessible files flag */
    int cacheAccessControl; /**< cache access control flag  */
    int permissionMask; /**< permission mask */
    int permissionCheckInterval; /**< check interval for permissions */
    char *sshCmd; /**< ssh command */
    char *scpCmd; /**< scp command */
    int lockFileCheckInterval; /**< check interval for lock file */  
    int cacheRefreshInterval; /**< cache refresh interval in minutes */
    int disableBsubEnh; /**< disable submission enhancement flag */
    char *noSshChkHosts; 
    int accessibleFilesCheck; /**< check accessible files and stage area on the same NFS point flag */
};

/**
 * \brief cluster type used by dm_clusters()
 */
typedef enum {
    OUTGOING_CLUSTER, /**< 0 outgoing */
    INCOMING_CLUSTER  /**< 1 incoming */
} dmClusterType;

/**
 * \brief cluster status used by dm_clusters()
 */
typedef enum {
    CLUSTER_NOTCONNECTED, /**< 0 not connected */
    CLUSTER_CONNECTED  /**< 1 connected */
} dmClusterStatus;

/* dm_clusters() API data structures */

/**
 * \brief dm_clusters() API data structure
 */
struct dmdRegisteredCluster {
    char *clusterName; /**< cluster name */
    char *masterName; /**< master host */
    int   status; /**< status, connected or not connected */
};

/**
 * \brief dm_clusters() API data structure
 */
struct dmdRegisteredRemoteCluster {
    dmClusterType type; /**< type, outgoing or incoming */
    char *clusterName; /**< cluster name */
    int index_master_host; /**< index of master host */
    int num_hosts; /**< number of hosts */
    char **hosts; /**< host list */
    dmClusterStatus status; /**< cluster status */
    int port; /**< port number */
};

/* dm_cache() API data structures */
/**
 * \brief cache query mode
 */
enum dmCacheQueryMode {
    DM_CACHE_QUERY_UNKNOWN_MODE,  /**< 0 unknown */
    DM_CACHE_QUERY_FILE_MODE,     /**< 1 file */
    DM_CACHE_QUERY_JOB_MODE,      /**< 2 job */
    DM_CACHE_QUERY_TAG_MODE       /**< 3 tag */
};

/**
 * \brief cache query option
 */
enum dmCacheQueryOption {
    DM_CACHE_QUERY_UNKNOWN_OPTION, /**< 0 unknown */
    DM_CACHE_QUERY_DETAILS         /**< 1 details */
};

/**
 * \brief cache query status
 */
enum dmFileStatus {
    DM_FILE_NONE,           /**< 0 none */
    DM_FILE_NEW,            /**< 1 new */
    DM_FILE_STAGING,        /**< 2 staging */
    DM_FILE_TRANSFERRED,    /**< 3 transferred */
    DM_FILE_ERROR,          /**< 4 error */
    DM_FILE_UNKNOWN,        /**< 5 unknown */
    DM_FILE_LINKED,         /**< 6 linked */
    DM_FILE_NO_TRANSFER     /**< 7 no copy, internal */
};

/**
 * \brief cache job query
 */
struct dmCacheJobQuery {
    char *jobID; /**< job id */
    char *clusterName; /**< cluster name */
};

/**
 * \brief cache file query
 */
struct dmCacheFileQuery {
    char *srcPath; /**< full source path */
    char *srcHost; /**< source host name */
    char *hashKey; /**< hash key */
};

/**
 * \brief used by dm_cache() API to specify a request
 */
struct dmCacheRequest {
    char* username; /**< The user name. */
    enum dmCacheQueryMode mode; /**< Cache query mode */
    int options;
    
    struct dmCacheFileQuery *file; /**< Cache file query */
    struct dmCacheJobQuery  *job;  /**< Cache job query */
    char *userGroup; /**< User group name */
};

/**
 * \brief used by dm_list_tags() to provide results
 */
struct dmTagInfo {
    char *tagName;   /**< The tag name. */
    char *tagOwner; /**< The owner of the tag */
    time_t lastDownloadTime;  /**< Time of last download. */
    time_t lastUploadTime;  /**< Time of last upload. */
    int chgrpErrno;  /**< The chgrp error number. */
    char *groupName; /**< The group of the tag */
};

/**
 * \brief reference job
 */
struct dmRefJob {
    char *jobID; /**< job id */
    char *clusterName; /**< cluster name */
};

/**
 * \brief used by dm_cache() API to provide job info
 */
struct dmJobInfo {
    char *jobID;                    /**< Single job or job array element */
    char *clusterName;              /**< cluster name */
    int   numCachedFiles;           /**< Number of cached files required by job */
    struct dmFileInfo *cachedFiles; /**< Array of cached files required by job */
};

/**
 * \brief file type
 */
typedef enum {
    DM_UNKNOWN_FILE_TYPE,   /**< 0 unknown */
    DM_STAGE_IN,            /**< 1 stage-in */
    DM_STAGE_OUT            /**< 2 stage-out */
} dmFileType;

/**
 * \brief used by dm_cache() API to provide file info
 */
struct dmFileInfo {
    dmFileType type;              /**< Input (stage-in) or output (stage-out) file */
    char *permission;             /**< The owner of this file in the cache. One of either: all | user:user_name | group:group_name */
    char  *filePath;              /**< source path of input file */
    char  *fileHost;              /**< source host of input file */
    char  *destPath;              /**< destination path of output file */
    char  *destHost;              /**< destination host of output file */
    char  *locationInCache;       /**< Host name and full path of file in cache */
    char  *hashKey;               /**< Hash key of cached file */
    LS_LONG_INT size;             /**< Size of file */
    int    status;                /**< Status of file in cache */
    time_t lastModifiedTime;      /**< Last modification time of file in cache */
    int    gracePeriodRemain;     /**< Seconds remain in grace period of file */
    int    numRefJobs;            /**< Number of jobs that require the file */
    struct dmRefJob *refJobs;     /**< Array of jobs that require the file */
    char  *xferJobID;             /**< Transfer job ID for the file */
    char  *xferClusterName;       /**< Transfer job cluster name for the file */
    time_t xferFinishTime;        /**< Transfer job finish time */
    char  *reason;                /**< Reason if the file in ERROR status */
};

/* the following used by both struct dmCacheResult and struct dmCacheRequest
 */
#define DM_CACHE_OPTIONS_DMD_RECOVERING    (0x0001) /* not for dmCacheRequest */
#define DM_CACHE_RESULT_DMD_RECOVERING     DM_CACHE_OPTION_DMD_RECOVERING
#define DM_CACHE_OPTIONS_SHOW_NTRANSFER    (0x0002)

/**
 * \brief used by dm_cache() API to return results
 */
struct dmCacheResult {
    int mode;                     /**< File or job info */
    int options;                  /**< options field */
    char *stageAreaHost;          /**< The staging area host */

    int numEntries;               /**< Number of entries in the result. */
    struct {
       struct dmFileInfo *files;  /**< Array of matching cached files. */
       struct dmJobInfo  *jobs;   /**< Array of matching jobs. */
    } queryMode;
};

/* used buy dm_chgrp internally */
struct dmChgrpInfo {
    char *groupName;                      /* must be specified */
#define DM_CHGRP_INFO_TARGET_NONE       0 /* not really an option */
#define DM_CHGRP_INFO_TARGET_TAG        1
#define DM_CHGRP_INFO_TARGET_HOSTFILE   2
    int targetType;
    char *targetName;
#define DM_CHGRP_INFO_OPTION_NONE                      0x0
#define DM_CHGRP_INFO_OPTION_IGNORE_TAG_UPLOAD_TIME    0x01
    int options;
};

/* used by dm_chmod() internally */
struct dmChmodInfo {
    char *mode;                           /* must be specified */
#define DM_CHMOD_INFO_TARGET_NONE       0 /* not really an option */
#define DM_CHMOD_INFO_TARGET_TAG        1
#define DM_CHMOD_INFO_TARGET_HOSTFILE   2
    int targetType;
    char *targetName;
    int options;
};



/* Stage-in/-out options */
#define DM_STAGE_LINK       0x00000001 /* stage-in & out option */
#define DM_STAGE_IN_ALL     0x00000002 /* stage-in option       */
#define DM_STAGE_IN_FILE    0x00000004 /* stage-in option       */
#define DM_STAGE_TAG        0x00000008 /* stage-in & out option */
#define DM_STAGE_OUT_DEST   0x00000010 /* stage-out option      */


/* dm_stage_in() API data structures */
struct dmStageinSourceFile {
    char *srcPath;   /* Source file path */
    char *srcHost;   /* Optional host name where the source file or
                      * directory is located.  If srcHost is not
                      * specified, all matching srcPath with any
                      * host will be staged in. */
};

/* The main request data structure */
/**
 * \brief main request data structure for dm_stage_in() API
 */
struct dmStginRequest {
    int options;
    char *destPath;  /**< Optional destination path name - can be a file path or folder path (for ALL or TAG mode). */
    union {
        struct dmStageinSourceFile srcFile; /**< source file information */
        struct dmTagInfo           tag; /**< tag information */
    } stginMode;
};


/* dm_stage_out() API data structures */
/**
 * \brief stage-out destination 
 */
struct dmStageoutDest {
    char *path;   /**< Final destination path to which the file will be transferred out. This field is optional. */
    char *host;   /**< Host name where the source file is located. This field is optional. */
};

/* The main request data structure */
/**
 * \brief main request data structure for dm_stage_out() API
 */
struct dmStgoutRequest {
    int  options;
    char *srcPath;  /**< A relative (to job's CWD) or absolute source path name. This field is required. */
    union {
        struct dmStageoutDest dest;  /**< destination information */
        struct dmTagInfo      tag;   /**< tag information */
    } stageoutMode;
};

/* Stage-out reply structure */

/* used internally */
struct dmStgoutReply {

#define STGOUT_REPLY_OK         0
#define STGOUT_REPLY_ERROR      1
#define STGOUT_REPLY_IN_CACHE   2
#define STGOUT_REPLY_TRY_AGAIN  3

    int reply;          /* contents of reply */
    int reqId;          /* requirement ID */
    char *pathInCache;  /* location of file in cache */
    int errorCode;      /* from enum dasErrCode */
};

/**
 * \brief used by dm_chgrp(), dm_chmod() APIs
 */
struct dmChPartialSuccess {
    int nSuccess; /**< Number of successful chgrp or chmod operations. */
    int nTotal; /**< Total number of attempted chgrp or chmod operations. */
};

/* dm_admin data structures and constants */
typedef enum {
     BDATA_ADMIN_RECONFIG        /* 0 */
    ,BDATA_ADMIN_SHUTDOWN        /* 1 */
} bdataSubcommandOptionID;

/* The corresponding error strings are defined
 * in lib.das.c/das_strerror().
 */
/**
 * \brief Possible values for daserrno.
 */
enum dasErrCode {
    DAS_NO_ERROR,              /**< 0 No errors */
    DAS_XDR_ERROR,             /**< 1 XDR encode/decode error */
    DAS_CHAN_ERROR,            /**< 2 Error in channel library */
    DAS_SOCKET_ERROR,          /**< 3 TCP/IP socket error */
    DAS_SERVER_UNREACH,        /**< 4 The dmd is unreachable */
    DAS_CONF_ERROR,            /**< 5 Error processing LSF data manager configuration */
    DAS_EINVAL,                /**< 6 Invalid argument  */
    DAS_MALLOC_FAILED,         /**< 7 malloc/calloc failed */
    DAS_MAX_CONN_EXCEEDED,     /**< 8 handles table is full */
    DAS_REQUIRED_CONF_MISSING, /**< 9 mandatory conf missing */
    DAS_RUNNING_ON_WRONG_HOST, /**< 10 dmd running on non-configured host */
    DAS_DMD_INTERNAL_ERROR,    /**< 11 dmd internal error, catch all */
    DAS_DMD_STAGING_AREA,      /**< 12 dmd can't access the staging area */
    DAS_PARAMS_ERROR,          /**< 13 Error geting in-memory parameters from DMD */
    DAS_CHECKCONF_FAILED,      /**< 14 DMD configuration check failed */
    DAS_NOT_IN_JOB,            /**< 15 Stage-in/out not executed inside an LSF job environment */
    DAS_NO_PERM_CLI,           /**< 16 User permission denied.  */
    DAS_NO_PERM_SRC_FILE,      /**< 17 No permission to access the source file */
    DAS_NO_PERM_TAG,           /**< 18 No permission to clean or write to a tag */
    DAS_TAG_NOT_FOUND,         /**< 19 Tag not found in staging area (for stage-in or clean) */
    DAS_FILE_NOTLINK,          /**< 20 You cannot use -link because the staging area is not locally mounted on the execution host. */
    DAS_FILE_NOTACCESS,        /**< 21 File not accessible */
    DAS_JOB_METADATA_NOTACCESS,/**< 22 Job metadata file cannot be found in staging area or it's not accessible */
    DAS_SRC_FILE_UNKNOWN,      /**< 23 Specified source file not found in job data requirement */
    DAS_RECONFIG_ERROR,        /**< 24 Error in processing DMD reconfiguration */
    DAS_SHUTDOWN_ERROR,        /**< 25 Error in processing DMD shutdown */
    DAS_FILE_SIGN_ERROR,       /**< 26 Error in generating a file signature */
    DAS_SHUTDOWN_IN_PROG,      /**< 27 DMD shutdown is in progress */
    DAS_API_ERROR,             /**< 28 Generic DAS API error */
    DAS_JOB_NOT_FOUND,         /**< 29 Queried job not found */
    DAS_FILE_NOT_FOUND,        /**< 30 Queried file not found */
    DAS_CLUSTER_NOT_FOUND,     /**< 31 Queried cluster not found */
    DAS_EAUTH_SERVER_ERROR,    /**< 32 General EAUTH SERVER error on DMD*/
    DAS_EAUTH_CLIENT_ERROR,    /**< 33 General EAUTH CLIENT error in DAS Environment, i.e. bdata calling eauth -c */
    DAS_DIRECTORY_ERROR,       /**< 34 Failed to create or access a directory */
    DAS_JOB_META_FILE_ERROR,   /**< 35 Job metadata file syntax or format error */
    DAS_RECOVERY_IO_ERROR,     /**< 36 I/O operation failed during recovery */
    DAS_RECOVERY_PATH_LIMIT,   /**< 37 Cache path too long */
    DAS_STGAREA_NOTACCESS,     /**< 38 Staging area not accessible */
    DAS_NO_ADMIN,              /**< 39 no admin */
    DAS_GET_DATA_REQ_FAILED,   /**< 40 Failed to get job's data requirement */
    DAS_REQ_NOT_FOUND,         /**< 41 Job's data requirement not found */
    DAS_GET_FILE_DATA_FAILED,  /**< 42 Failed to get file data information */
    DAS_UNDEF_CLUSTER,         /**< 43 Unable to determine the cluster name */
    DAS_REQ_FILES_NOT_FOUND,   /**< 44 Files in the job's data requirement are not found in the Data Manager. */
    DAS_DMD_MAS_HOST_UNKNOWN,  /**< 45 Unable to determine the DMD master host. */
    DAS_GET_CLUSTERS_FAILED,   /**< 46 Failed to get clusters info in DMD. */
    DAS_GET_REMOTE_CLUSTERS_FAILED, /**< 47 Failed to get remote clusters info from DMD */
    
    DAS_INVALID_CLI_OPTION,    /**< 48 Invalid DAS CLI command or option. */
    DAS_INV_OPT_FORMAT,        /**< 49 Invalid DAS CLI command or option. */
    DAS_UNKNOWN_CMD_HOST,      /**< 50 Unable to determine the command execution host. */
    DAS_ENV_INIT_FAILED,       /**< 51 Failed to initialize the DAS environment. */
    DAS_STAGE_XFER_CMD_FAILED, /**< 52 The transfer command for staging in or out a file failed. */
    DAS_CMD_USER_UNKNOWN,      /**< 53 Unable to determine the user name who executes the command. */
    DAS_DELETE_DIR_FAILED,     /**< 54 Failed to delete a directory or its contents due  to a system error. */
    DAS_INVALID_FILE_PATH,     /**< 55 Invalid file path or path format is specified. */
    DAS_INVALID_JOB_ID,        /**< 56 Invalid job id */
    DAS_SRC_PATH_NOT_FILE,     /**< 57 Source path is not a regular file. */
    DAS_DST_HOST_UNKNOWN,      /**< 58 Destination host for stage-out is unknown. */
    DAS_DST_PATH_UNKNOWN,      /**< 59 Destination path for stage-out is unknown. */
    DAS_CLUSTER_UNKNOWN, /**< 60 Specified cluster is unknown */
    DAS_SHUTDOWN_WHEN_LIM_RUNNING, /**< 61 Cannot shut down dmd when local lim is running. */
    DAS_DMD_NOTMASTER_ERROR,   /**< 62 LSF data manager is unreachable. Try later. */
    DAS_BAD_TAG_NAME,          /**< 63 Bad tag name specified  */

    DAS_USAGE,                 /**< 64 Incorrect sub command. */
    DAS_SUB_CACHE,             /**< 65 bdata cache sub command parameter error. */
    DAS_SUB_TAGS,              /**< 66 bdata tags sub command parameter error. */
    DAS_SUB_SHOWCONF,          /**< 67 bdata showconf sub command parameter error. */
    DAS_SUB_CONNECTIONS,       /**< 68 bdata connections sub command parameter error. */
    DAS_SUB_ADMIN,             /**< 69 bdata admin sub command parameter error. */
    DAS_SUB_RELATIVEPATH,      /**< 70 bdata cache file has relative path. */
    DAS_TAG_NOTACCESS,         /**< 71 The tag name directory cannot be found in staging area or it's not accessible */
    DAS_SHUTDOWN_FROM_NON_DMD_HOST, /**< 72 Cannot shut down dmd from host not defined in LSF_DATA_HOSTS. */
    DAS_JOB_FORWARDED,         /**< 73 Job forwarded to a remote cluster. Run bjobs or bhist to see current job information. */
    DAS_TAGS_CLEAN_UALL_ERROR, /**< 74 bdata tags clean option <-u all> is not valid. */
    DAS_DST_EMPTY_STRING,      /**< 75 the -dst option cannot be an empty string
*/
    DAS_USER_GROUP_NOT_EXIST,  /**< 76 user group does not exist */
    DAS_USER_NOT_IN_USER_GROUP, /**< 77 user doesn't belong to the user group */
    DAS_SUB_CHGRP,             /**< 78 bdata chgrp sub command error */
    DAS_SUB_CHMOD,             /**< 79 bdata chmod sub command error */
    DAS_ACCESS_CONTROL_DISABLED, /**< 80 feature not available */
    DAS_CHGRP_PARTIAL,         /**< 81 chgrp was only partially successful */
    DAS_CHGRP_FAIL,            /**< 82 chgrp failed */
    DAS_TAG_NOT_A_DIR,         /**< 83 the tag name does not specify a directory */
    DAS_TAG_LINK,              /**< 84 the tag directory is a link */
    DAS_USER_NOT_EXIST,        /**< 85 user does not exist */
    DAS_INVALID_MODE,          /**< 86 bad mode argument */
    DAS_CHMOD_PARTIAL,         /**< 87 chmod was only partially successful */
    DAS_CHMOD_FAIL,            /**< 88 chmod failed */
    DAS_CHMOD_PARTIAL_MASK,    /**< 89 chmod could only change some of the permissions because of permission mask */
    DAS_CHMOD_NONE_MASK,       /**< 90 chmod could not change any of the permissions because of permission mask is 000 */
    DAS_FOLDER_NOT_ALLOWED,    /**< 91 requested an operation on a folder which is not allowed */
    DAS_NUM_ERRORS             /**< number of error codes */
};


/* das_init() options
 * If DAS_INIT_OPEN_LOGFILE is set the function ls_openlog_version()
 * is called, otherwise it is not.  If DAS_INIT_OPEN_LOGFILE is not
 * used then setting DAS_INIT_WRITE_STDERR has no effect.
 */
#define DAS_INIT_NONE           (0x0)
#define DAS_INIT_OPEN_LOGFILE   (0x01)
#define DAS_INIT_WRITE_STDERR   (0x02)
#define DAS_INIT_CLIENT_SIDE    (0x04) /* DAS init is done on client-side
                                          such as CLI/API. */
#define DAS_INIT_LSFCONF_ONLY   (0x08) /* Only read the lsf.conf params. */
#define DAS_INIT_GET_PARAMS_FROM_DMD    (0x10)


#ifndef _QUERY_CLUSTERS_REPLY_
#define _QUERY_CLUSTERS_REPLY_
/**
 * \brief used by dm_clusters() API
 */
struct queryClustersReply {
    int numClusters; /**< number of local clusters */
    int remoteNumClusters; /**< number of remote clusters */
    struct dmdRegisteredCluster *clustersInfo; /**< local cluster information */
    struct dmdRegisteredRemoteCluster *remoteClustersInfo; /**< remote cluster information */
};
#endif


#ifdef __cplusplus
}
#endif

#endif /* _DM_DEF_H_ */
