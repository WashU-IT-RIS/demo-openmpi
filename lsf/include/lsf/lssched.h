/*
 ****************************************************************************
 *
 * Load Sharing Facility
 *
 * Header file for external scheduler plugins
 *
 ****************************************************************************
 */
#ifndef _LSSCHED_H_
#define _LSSCHED_H_

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#if defined(WIN32)
#include <string.h>
#else
#include <strings.h>
#endif
#include "lsf.h"

#ifndef      FALSE
#define      FALSE      0
#endif
#ifndef      TRUE
#define      TRUE       1
#endif

#if !defined(WIN32)
extern int sched_version(void *);
extern int sched_init(void *);
extern int sched_pre_proc(void *);
extern int sched_match_limit(void *);
extern int sched_order_alloc(void *);
extern int sched_post_proc(void *);
extern int sched_finalize(void *);
#else
__declspec(dllexport) int sched_version(void *);
__declspec(dllexport) int sched_init(void *);
__declspec(dllexport) int sched_pre_proc(void *);
__declspec(dllexport) int sched_match_limit(void *);
__declspec(dllexport) int sched_order_alloc(void *);
__declspec(dllexport) int sched_post_proc(void *);
__declspec(dllexport) int sched_finalize(void *);
#endif


/***************************************************************************
 *
 * Opaque internal data structures
 *
 ***************************************************************************
 */

typedef void INT_Reason;         /* pending reason(s) for a job */
typedef void INT_RsrcReq;        /* resource requirement structure */
typedef void INT_Alloc;          /* resource allocation */
typedef void INT_AllocLimitList; /* allocation limit list */
typedef void INT_Job;            /* a job */
typedef void INT_JobList;        /* a list of jobs */
typedef void INT_JobBlock;       /* a job element from a list */
typedef void INT_Host;           /* a host */
typedef void INT_CandGroupList;  /* a list of candidate host groups */
typedef void INT_ChunkRef;       /* job chunk reference */


/*
 * Data structure for how slots are distributed over a set of hosts for a job.
 *
 * For a parallel job, this structure gives information about how many slots
 * have been given to the job on each candidate host, and how many slots are
 * available to be given to the job on the host.
 *
 * SEE ALSO
 *
 * extsched_cand_getavailslot
 * candHost
 */
struct hostSlot {
    struct candHost * candHost;	/* information for which candidate host */
    char *name;			/* name of candidate host */
    int  nslotAvail;		/* available slots on the host */
    int  nslotAlloc;		/* allocated slots on the host */
    int  ncpus;			/* number of cpu on the host */
};

/*
 * Data structure representing a candidate host for a job.
 *
 * SEE ALSO
 *
 * candHostGroup
 * hostSlot
 * extsched_reason_set
 * extsched_cand_getavailslot
 */
struct candHost {
    int  hostIdx;		/* index to the internal host table */
    INT_Host *hostPtr;		/* pointer to internal host data    */
};

/*
 * Data structure representing a group of candidate hosts for a job.
 * 
 * SEE ALSO
 *
 * candHost
 * extsched_cand_getnextgroup
 * extsched_cand_removehost
 * extsched_alloc_gethostslot
 */
struct candHostGroup {
    struct candHostGroup *forw; /* forward pointer to other cand groups for
				 * the job */
    struct candHostGroup *back; /* backward pointer to other cand groups for
				 * the job */
    int    maxOfMembers;	/* total number of hosts */
    int    numOfMembers;	/* current number of candidate hosts for
				 * the group */
    struct candHost *candHost;	/* array of candidate host info */
};

/* 
 *-----------------------------------------------------------------------
 *
 * extsched_cand_getnextgroup
 *
 * PARAMETERS
 * candGroupList [IN] :  current pointer to group of candidate hosts.
 *
 * DESCRIPTION
 *
 * Iterate to the next group of candidate hosts for a job. 
 * 
 * RETURN
 *
 * pointer to a candHostGroup structure
 *
 * SEE ALSO
 * 
 * candHostGroup
 *-----------------------------------------------------------------------
 */
extern struct candHostGroup *extsched_cand_getnextgroup(
    INT_CandGroupList *candGroupList);

/* These names are the basic ones used in LSF to represent the load
 * and the static resources of a host. These names are always present.
 * These names appear in the resName member of the resources data
 * structure. All other resources defined in the cluster, via lsf.shared
 * or via the license scheduler, appear in the data structure as well,
 * but since they have custom definition they cannot appear in this symbol
 * table. The names in the table have the _ postfix to not conflict with
 * other definitions in the scheduler.
 */

#define cpuf_     "cpuf"
#define ncpus_    "ncpus" 
#define ndisks_   "ndisks" 
#define maxmem_   "maxmem"
#define maxswp_   "maxswp"
#define maxtmp_   "maxtmp"
#define r15s_     "r15s" 
#define r1m_      "r1m" 
#define r15m_     "r15m" 
#define ut_       "ut" 
#define pg_       "pg" 
#define io_       "io" 
#define ls_       "ls" 
#define it_       "it" 
#define tmp_      "tmp" 
#define swp_      "swp" 
#define mem_      "mem" 
#define slot_     "slots" 
#define task_     "-task-" 

/* This data structure wraps the resources available on each host
 */
struct hostResources {
    char               *hostName;
    char               *cluster;
    int                nres;
    struct resources   *res;
};

/* This is the type of the resource as configured in LSF base
 */
enum restype {
    NONE,
    STATIC_RESOURCE,
    DYNAMIC_RESOURCE,
    SHARED_RESOURCE
};

/* This is the C type of the resource value, bool (basically int), 
 * 0 terminated char * or float.
 */
enum ctype {
    BOOL_VAL,
    FLOAT_VAL,
    STRING_VAL
};

/* This is the representation of each individual resource.
 * A resource is basically a pair (name, value) representing a
 * named quantity.
 */
struct resources {
    char        *resName;
    int         resType;
    int         cType;
    union {
	char     *sval;
	int      bval;
	float    fval;
    } val;
};

/*
 *-----------------------------------------------------------------------
 *
 * extsched_host_resources
 *
 * PARAMETERS
 *
 * host   [IN] : point to internal host structure
 *
 * DESCTIPTION
 *
 * This API takes in input a void pointer to a scheduler internal
 * representation of a host and returns the resources on the host.
 * The host is for example the void * pointer of the candHost data
 * structure.
 * The API returns a pointer to a statically allocated memory
 * which is freed at the next fuction invocation. The caller must
 * copy the data if they have to be kept across function invocations.
 *
 * RETURN
 *
 * pointer to hostResource structure
 *
 *-----------------------------------------------------------------------
 */
extern struct hostResources *extsched_host_resources(INT_Host *host);

/*
 *-----------------------------------------------------------------------
 * extsched_getResType
 *
 * PARAMETERS
 *
 * r [IN] : pointer to the resource structure 
 *
 * DESCRIPTION
 *
 * A little helper routine returning the string from the enum restype.
 * Don't free the returned string (it points to a static buffer).
 *
 * RETURN
 * 
 * string ("STATIC_RESOURCE", "DYNAMIC_RESOURCE", "SHARED_RESOURCE", or "")
 *
 *-----------------------------------------------------------------------
 */
extern char *extsched_getResType(const struct resources *r);

/* Data structure representing a job inside the plugin
 */
struct jobInfo {
    int    jobId;
    int    jobIndex; /* will be set to indexRangeStart[0] or 0 */
    int    indexRangeCnt; /* 0 is set for non-array single job */
    int    *indexRangeStart;
    int    *indexRangeEnd;
    int    *indexRangeStep;
    char   *cluster;
    char   *user;
    char   *queue;
    char   *resKey;
    int    status;          /* job states */
    int    pendReason;
    int    submitTime;
    char   *jobGroup;
    char   *serviceClass;
    char   *project;
    char   *enforcedGroup;
    char   *schedHostType;
    char   *schedHostModel;
    int    userPriority;   
    int    incPriority;    
    int    cpuLimit;       
    int    runLimit;       
};

/* The scheduler uses different data structures for the job
 * in different phases of the scheduling.
 * The API lsb_get_jobinfo() should be use in ALLOC or NOTIFY
 * phases, the API has to be told in which phase is being invoked
 * in order to derefrence the opaque job address correctly.
 */
enum phase {
    PHASE_ALLOC,
    PHASE_NOTIFY
};


/* 
 *-----------------------------------------------------------------------
 * extsched_reason_set
 *
 * PARAMS
 * 
 * reasonPtr - [IN/OUT] the pointer to internal object that collects
 * 	       pending reasons for the job.
 * candHost - [IN] host that cannot be used by job
 * reasonId - [IN] the ID of the reason why host cannot be used.
 *
 * DESCRIPTION
 *
 * Provide the reason why a host is not considered usable for a job.
 *
 * reasonId must be either a pre-defined reason number, 
 * or a customized reason number between 20001 and 25000.
 * LSF reserves pending reason numbers from 1 - 20000.  Please refer
 * lsbatch.h for reason ID definitions.
 *
 * RETURNS
 *
 * 0 if successfully set reason, <0 if failed.
 *
 *-----------------------------------------------------------------------
 */
extern int extsched_reason_set(INT_Reason *reasonPtr, 
			       struct candHost *candHost, 
			       int reasonId);

/* 
 *-----------------------------------------------------------------------
 * extsched_cand_removehost
 *
 * PARAMS
 * 
 * group - [IN/OUT] group of hosts to be modified
 * index - [IN] index of the candidate host in the group to be removed.
 *
 * DESCRIPTION
 *
 * Remove a candidate host from the group.
 *
 * Remaining hosts in the group are shifted up to the current index.
 * Thus, do not increment the current index if iterating through the 
 * candidate hosts in this group, 
 *
 * RETURN
 *
 * none
 *-----------------------------------------------------------------------
 */
extern void extsched_cand_removehost(struct candHostGroup *group, int index);


/* 
 *-----------------------------------------------------------------------
 * extsched_cand_getavailslot
 *
 * PARAMS
 * 
 * candHost - (IN) host to get slot availability from.
 * 
 * DESCRIPTION
 *
 * Get number of available slots on a candidate host. 
 * This function can also be used to get candidate host name, which is 
 * retruned in hostSlot.
 *
 * RETURNS
 *
 * Available slots on host.
 *
 *-----------------------------------------------------------------------
 */
extern struct hostSlot *extsched_cand_getavailslot(struct candHost *candHost);

/* 
 * Signature for functions that handle newly submitted resource requirement.
 *
 * Use these functions for newly submitted jobs to 
 * - get the user-specified resource requirement messages
 * - attach handler-specific data to peer jobs.
 *
 * PARAMS
 * 
 * resreq - (IN) Resource requirement object for newly submitted job.
 *          This is an internal object representing resource requirements for
 *          peer jobs.  
 *
 * RETURNS
 *
 * 0 if successfully processed newly submitted resource requirements.
 * <0 if failed to process.
 *
 * SEE ALSO
 * 
 * RsrcReqHandlerType
 * RsrcReqHandler_FreeFn
 * extsched_resreq_registerhandler
 * extsched_resreq_getextresreq
 */
typedef int (* RsrcReqHandler_NewFn) (INT_RsrcReq *resreq);

/* 
 * Signature for functions that handles freeing of handler-specific
 * data when no more peer jobs refer to a resource requirement.
 *
 * PARAMS
 * 
 * handlerData-(IN/OUT) Handler-specific data that was created and attached to 
 *	    a resource requirement in above New function. Scheduler 
 *	    framework invokes this callback when the data is no longer
 *          usable.
 *
 * SEE ALSO
 * 
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * extsched_resreq_registerhandler
 */
typedef void (* RsrcReqHandler_FreeFn)(void *handlerData);

/* 
 * Signature for functions that check candidate hosts against resource
 * requirements for peer jobs.
 *
 * PARAMS
 * 
 *  handlerData - (IN) Handler-specific data attached to resource requirements
 *  	          for peer jobs.
 *  candGroupList - (IN/OUT) list of candidate host groups.
 *  reasonPtr - (IN/OUT) pointer to internal object that collects pending
 *  	        reasons.
 *
 * SEE ALSO
 * 
 * extsched_cand_getnextgroup
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * extsched_resreq_registerhandler
 */
typedef int (* RsrcReqHandler_MatchFn)(void *handlerData,
				       INT_CandGroupList *candGroupList,
				       INT_Reason *reasonPtr);

/* 
 * Signature for functions that sort candidate hosts by preferences for peer
 * jobs.
 *
 * These "sort" functions are called after matching is finished.
 *
 * PARAMS
 * 
 *  handlerData - (IN) Handler-specific data attached to resource requirements
 *  	          for peer jobs.
 *  candGroupList - (IN/OUT) list of candidate host groups.
 *  reasonPtr - (IN/OUT) pointer to internal object that collects
 *              pending reasons.
 *
 * SEE ALSO
 * 
 * extsched_cand_getnextgroup
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * extsched_resreq_registerhandler
 */
typedef int (*RsrcReqHandler_SortFn)(void *handlerData,
				     INT_CandGroupList *candGroupList,
				     INT_Reason *reasonPtr);

/* These define are imported from the framework header files.
 */
#define NOTIFY_IN_SCHED_SESSION     0x001   /* inside scheduling session
					     */
#define NOTIFY_OUT_SCHED_SESSION    0x002   /* outside scheduling session
					     */
#define NOTIFY_ALLOC                0x004   /* allocation notification 
					     */
#define NOTIFY_DEALLOC              0x008   /* deallocation notification 
					     */
#define NOTIFY_STARTUP              0x010   /* job allocation at startup 
					     */

typedef int (*RsrcReqHandler_CheckAllocFn)(
    void *info,
    INT_Job *job, 
    INT_Alloc *alloc,
    INT_AllocLimitList *allocLimitList);

typedef int (*RsrcReqHandler_NotifyAllocFn)(
    void *info,
    INT_Job *job, 
    INT_Alloc *alloc,
    INT_AllocLimitList *allocLimitList,
    int  flag);

/* 
 * Data structure containing handler-specific functions for resource
 * requirements.
 *
 * SEE ALSO
 *
 * extsched_resreq_registerhandler
 * RsrcReqHandler_NewFn
 * RsrcReqHandler_FreeFn
 * RsrcReqHandler_MatchFn
 * RsrcReqHandler_SortFn
 * RsrcReqHandler_NotifyAllocFn
 *
 */
typedef struct _RsrcReqHandlerType {

    /* Handler-specific function to handle newly submitted
     * jobs' resource requirement. 
     */
    RsrcReqHandler_NewFn         newFn;

    /* Handler-specific function to free handler-specific
     * data when no more peer jobs refer to a resource
     * requirement. 
     */
    RsrcReqHandler_FreeFn        freeFn;

    /* Handler-specific function to check candidate hosts
     * against peer jobs' resource requirements. 
     */
    RsrcReqHandler_MatchFn       matchFn;

    /* Handler-specific function to sort candidate hosts by
     * peer jobs' preferences. 
     */
    RsrcReqHandler_SortFn        sortFn;

    /* This function is invoked when a job dispatch decision 
     * was made and plugins may want to update their own 
     * internals.
     */
    RsrcReqHandler_NotifyAllocFn   notifyAllocFn;

    /* This function is invoked when a job dispatch decision 
     * was made and plugins may want to check 
     */
    RsrcReqHandler_CheckAllocFn   checkAllocFn;

} RsrcReqHandlerType;

/* 
 * Register a new handler for job resource requirements.
 *
 * Registered handler functions will be called during scheduling 
 * to handle peer jobs' resource requirements.
 *
 * PARAMS
 * 
 * handlerId (IN) identifier for the handler
 * handler (IN) pointer to the handler.  Memory must be dynamically allocated.
 *         Memory must not be freed.
 * 
 * PRECONDITIONS
 *
 * The handlerId must be a number between 100 and 199.  Each handler must have
 * a different identifier.
 * 
 * Fields of RsrcReqHandlerType that are not used must be set to NULL.
 *
 * SEE ALSO
 *
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * RsrcReqHandler_FreeFn
 * RsrcReqHandler_MatchFn
 * RsrcReqHandler_SortFn
 * RsRcReqHandler_NotifyAlloc
 *
 */
extern int extsched_resreq_registerhandler(int handlerId, 
					   RsrcReqHandlerType *handler);

/* 
 * Attach handler-specific data to a set of peer jobs.
 *
 * Jobs are classified as peer jobs when they have the same handler-specific
 * data attached (as indicated by a unique string representation).  Peer jobs
 * are scheduled together.
 *
 * PARAMS
 *
 * resreq - (IN) Internal object representing resource requirements of
 *          peer jobs.
 * handlerId - (IN) identifier for the handler.
 * key - (IN) the unique string representing the handler-specific data.
 * handlerData - (IN) handler-specific data to be attached to the peer jobs.
 *
 * PRECONDITIONS
 *
 * 1. The handlerId must be an ID previously registered with
 *    extsched_resreq_registerhandler.
 *
 * 2. Peer jobs are scheduled together, so handlerData attached with the same
 *    key must GUARANTEE to produce the same list of candidate hosts for all
 *    peer jobs.
 *
 * 3. The key will be copied.
 *
 * 4. The handlerData should not be freed until framework calls the freeFn of
 *    the registered handler type.
 *
 * SEE ALSO
 *
 * extsched_resreq_registerhandler
 * RsrcReqHandler_NewFn
 * RsrcReqHandler_FreeFn
 */
extern void extsched_resreq_setobject(INT_RsrcReq *resreq, 
				      int handlerId, 
				      char *key, 
				      void *handlerData);

/* 
 * Signature for functions that adjusts allocation decisions for jobs. 
 *
 * Use these functions to adjust allocations made by internal scheduling
 * functions.
 *
 * Note that these functions will only be called for jobs that have 
 * some potentially usable candidate hosts.  These functions may not
 * be called for certain jobs if 
 * - all candidate hosts have been previously determined to not satify 
 *   resource requirements of the peer jobs.
 * - the decision, SCH_MOD_DECISION_PENDPEERJOBS, has been given for
 *   a peer job to this job.
 *
 * PARAMS
 *
 * job - (IN) Internal job object. Make allocation decision for this job.
 * groupList - (IN/OUT) List of candidate host groups usable for the job.
 * reasonPtr - (IN/OUT) Internal object to collect pending reasons for the
 *             job.
 * alloc - (IN/OUT) Internal object representing resources allocated to the
 * 	   job.
 *
 * RETURNS
 *
 * Decision for the job:
 * - SCH_MOD_DECISION_NONE: The function does not care to alter the decision
 *   for this job.
 * - SCH_MOD_DECISION_DISPATCH: Decide to dispatch the job with the given
 *   allocation in alloc.
 * - SCH_MOD_DECISION_RESERVE: Decide to reserve resources for the job with
 *   the given reservation in alloc.
 * - SCH_MOD_DECISION_PENDPEERJOBS: Decide to keep this job pending, along
 *   with all its peer jobs.
 * - SCH_MOD_DECISION_PENDJOB: Decide to keep this job only pending. Peer
 *   jobs will be scheduled separately.
 *
 * SEE ALSO
 *
 * extsched_alloc_registerallocator
 * extsched_cand_getnextgroup
 * candHostGroup
 * extsched_alloc_modify
 * extsched_alloc_gethostslot
 * extsched_job_getaskedslot
 * extsched_job_getextresreq
 * extsched_job_getrsrcreqobject
 * extsched_resreq_setobject
 */
typedef int (*AllocatorFn)(INT_JobBlock *jobBlock, /* not an INT_Job pointer */
			   INT_CandGroupList *groupList,
			   INT_Reason *reasonPtr,
			   INT_Alloc **alloc);

#define SCH_MOD_DECISION_NONE            0
#define SCH_MOD_DECISION_DISPATCH        1
#define SCH_MOD_DECISION_RESERVE         2
#define SCH_MOD_DECISION_PENDPEERJOBS    3
#define SCH_MOD_DECISION_PENDJOB         4

/* 
 * Register a functions that adjusts allocation decisions for jobs. 
 *
 * These functions will be called after internal scheduling policies
 * have decided allocations for jobs.
 *
 * PARAMS
 *
 * allocator - (IN) Pointer to the function that adjusts alocations.
 *
 * RETURN
 *
 * 0 if succesfully registered. <0 if failed to register the function.
 *
 * SEE ALSO
 *
 * AllocatorFn
 */
extern int extsched_alloc_registerallocator(AllocatorFn  allocator);

/* 
 * Get user-specified resource requirement message for the peer jobs.
 *
 * PARAMS
 *
 * resreq - (IN) Internal object representing resource requirements for peer
 *          jobs.
 * msgCnt - (OUT) Number of user-specified messages submitted with the
 *          peer jobs.
 * RETURNS
 *
 * Array of user-specified messages submitted with the peer jobs.
 * If there are no such messages, returns NULL.
 *
 * SEE ALSO
 *
 * extsched_job_getextresreq
 */
extern char **extsched_resreq_getextresreq(INT_RsrcReq *resreq, int *msgCnt);

/*
 *-----------------------------------------------------------------------
 * extsched_resreq_getqueextsched
 *
 * PARAMS
 *
 * resreq - (IN) Internal object representing resource requirements for peer
 *          jobs.
 * mand_extsched - (OUT) return the setting of MANDATORY_EXSCHED of job 
 *          queue, mand_extsched should not be NULL.
 * default_extsched - (OUT) return the setting of DEFAULT_EXSCHED of job
 *          queue, default_extsched should not be NULL. It will points the 
 *
 * DESCRIPTION
 *
 * Get queue level MANDATORY_EXTSCHED and DEFAULT_EXTSCHED for job
 *
 * RETURNS
 *
 * If no error occurs, return 0, otherwise return -1,
 * If MANDATORY_EXTSCHED has been set in job's queue, *mand_extsched will
 *    points to the string, otherwise it will be NULL.
 * If DEFAULT_EXTSCHED has been set in job's queue, *default_extsched will
 *    points to the string, otherwise it will be NULL.
 *
 *-----------------------------------------------------------------------
 */
extern int extsched_resreq_getqueextsched(INT_RsrcReq *resreq, 
					  char **mand_extsched,
					  char  **default_extsched);

/* 
 *-----------------------------------------------------------------------
 * extsched_job_getextresreq
 *
 * PARAMS
 *
 * job - (IN) Internal job object.
 * msgCnt - (OUT) Number of user-specified messages submitted with the
 *          peer jobs.
 *
 * DESCRIPTION
 *
 * Get the user-specified resource requirements messages for the job.
 * 
 * Same as extsched_resreq_getextresreq, but uses pointer to the job,
 * rather than the resource requirements of the peer jobs.
 *
 * RETURNS
 *
 * Array of user-specified messages submitted with the peer jobs.
 * If there are no such messages, returns NULL.
 *
 *-----------------------------------------------------------------------
 */
extern char **extsched_job_getextresreq(INT_JobBlock *jobPtr, int *msgCnt);

/* 
 *-----------------------------------------------------------------------
 * extsched_job_getaskedslot
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object.
 *
 * DESCRIPTION
 * 
 * Get the number of slots requested by a job.
 *
 * RETURNS
 *
 * Returns (max) number of slots requested by job.
 *-----------------------------------------------------------------------
 */
extern int extsched_job_getaskedslot(INT_JobBlock *jobPtr);

/* 
 *-----------------------------------------------------------------------
 * extsched_alloc_gethostslot
 *
 * PARAMS
 *
 * alloc - (IN) Internal allocation object. Allocated slots for the job.
 * candGroupList - (IN) list of candidate host groups for the job.
 * hostCnt - (OUT) number of hostSlot information returned.
 * group - (OUT) which candidate host group is allocated.
 *
 * DESCRIPTION
 *
 * Get the distribution of slots on various candidate hosts in job's
 * allocation.
 *
 * RETURNS
 *
 * Array of hostSlot data structures, giving the number of slots given
 * to the job on each host, and slots available on the host.
 *-----------------------------------------------------------------------
 */
extern struct hostSlot* extsched_alloc_gethostslot(
    INT_Alloc *alloc, 
    INT_CandGroupList *candGroupList, 
    int *hostCnt,
    struct candHostGroup **group);

/* 
 *-----------------------------------------------------------------------
 * extsched_alloc_type
 *
 * PARAMS
 *
 * alloc - (IN) Internal allocation object. Allocated slots for the job.
 *
 * DESCRIPTION
 *
 * Return the type of allocation given to the job (reservation or dispatch).
 *
 * RETURNS
 *
 * SCH_MOD_DECISION_NONE: alloc is NULL. No decision has been made.
 * SCH_MOD_DECISION_DISPATCH: alloc is a dispatch decision.
 * SCH_MOD_DECISION_RESERVE: alloc is a reservation decision.
 *-----------------------------------------------------------------------
 */
extern int extsched_alloc_type(INT_Alloc *alloc);

/*
 *-----------------------------------------------------------------------
 * extsched_alloc_modify
 *
 * PARAMS
 *
 * jobptr - (IN) jobblock pointer passed into allocator function, this is
 *               not an INT_Job pointer.
 * alloc - (IN/OUT) Internal allocation object. 
 * host - (IN) The host to be modified. nslotAlloc field in hostSlot indicates
 *             how many slots need to be allocated on the host.
 * reason - (IN) Internal reason object.
 * group - (IN) which candidate host group is used in the allocation. 
 *
 * DESCRIPTION
 *
 * Adjust the number of slots allocated to a job on a candidate host,
 * if possible.
 *
 * Reduce or increaese the number of slots allocated to a job on a host.  If
 * increasing, this function will check that other scheduling policies are not
 * violated by the new allocation.
 *
 * Use this function from with an allocator function.
 *
 * The nslotAlloc field of host parameter must be greater than 1 (that is,
 * hosts cannot be removed from an allocation).
 *
 * RETURNS
 *
 * TRUE if allocation was successfuly adjusted.  FALSE if adjustment
 * failed (due to conflict with other scheduling policies).
 *
 *-----------------------------------------------------------------------
 */
extern int extsched_alloc_modify(INT_JobBlock *jobptr,
				 INT_Alloc **alloc, 
				 struct hostSlot *host,
				 INT_Reason *reason,
				 struct candHostGroup *group);


/*
 *-----------------------------------------------------------------------
 * JobOrderFn4Que
 *
 * PARAMS
 * 
 * queueName - (IN) the name of the queue to which the job list belongs
 * jobList   - (IN) the ready job list belonging to the queue
 *
 * DESCRIPTION
 *
 * Signature for the function to decide job dispatch order.
 *
 * Use this funtion to decide which job in the ready job list passed in
 * from the queue should go first
 *
 * RETURNS
 *
 * Pointer to the job which should go first. If no decision can be made
 * then a NULL will be returned. 
 *
 *-----------------------------------------------------------------------
 */
typedef INT_JobBlock * (*JobOrderFn4Que) (char *queueName, INT_JobList *jobList);

/* 
 *-----------------------------------------------------------------------
 * extsched_order_registerOrderFn4AllQueues
 *
 * PARAMS
 *
 * fcfsOrderFn - (IN) Pointer to the function that decides job dispatch order.
 *
 * DESCRIPTION
 *
 * Register a function that decides job dispatch order for all queues.
 *
 * RETURN
 *
 * 0 if succesfully registered. <0 if failed to register the function.
 *
 *-----------------------------------------------------------------------
 */
extern int extsched_order_registerOrderFn4AllQueues(JobOrderFn4Que  orderFn);

/* 
 *-----------------------------------------------------------------------
 * extsched_order_isJobListEmpty
 *
 * PARAMS
 *
 * jobList - (IN) Ready to dispatch job list.
 *
 * DESCRIPTION
 *
 * Check if the jobList is empty.
 *
 * RETURNS
 *     TRUE  - if no job in the list
 *     FALSE - there are jobs in the job list
 *
 *-----------------------------------------------------------------------
 */
extern int extsched_order_isJobListEmpty(INT_JobList *jobList); 

/* 
 *-----------------------------------------------------------------------
 * extsched_order_getFirstJobOfList
 *
 * PARAMS
 *
 * jobList - (IN) The passed in job list.
 *
 * DESCRIPTION
 *
 * Get the first job in the job list.
 *
 * RETURNS
 *     Return the first job in jobList if jobList is not empty,
 *     otherwise return NULL. 
 *
 *-----------------------------------------------------------------------
 */
extern INT_JobBlock *extsched_order_getFirstJobOfList(INT_JobList *jobList);


/* 
 *-----------------------------------------------------------------------
 * extsched_order_getNextJobOfList
 *
 * PARAMS
 *
 * currJob - (IN) Pointer to the current job.
 * jobList - (IN) The passed in job list.
 *
 * DESCRIPTION
 *
 * Get the next job in the job list.
 *
 * RETURNS
 *     Return the next job in jobList if jobList is not empty,
 *     otherwise return NULL. 
 *
 *-----------------------------------------------------------------------
 */
extern INT_JobBlock *extsched_order_getNextJobOfList(INT_JobBlock *currJob,  
						     INT_JobList *jobList);


/* 
 *-----------------------------------------------------------------------
 * extsched_order_getPreJobOfList
 *
 * PARAMS
 *
 * currJob - (IN) Pointer to the current job.
 * jobList - (IN) The passed in job list.
 *
 * DESCRIPTION
 *
 * Get the previous job in the job list.
 *
 * RETURNS
 *     Return the previous job in jobList if jobList is not empty,
 *     otherwise return NULL. 
 *
 *-----------------------------------------------------------------------
 */ 
extern INT_JobBlock *extsched_order_getPreJobOfList(INT_JobBlock *currJob,  
						    INT_JobList *jobList);


/* 
 *-----------------------------------------------------------------------
 * extsched_order_getJobNumOfList
 *
 * PARAMS
 *
 * jobList - (IN) The passed in job list.
 *
 * DESCRIPTION
 *
 * Get the number of jobs in the job list.
 *
 * RETURNS
 *     The number of jobs in the job list.
 *
 *-----------------------------------------------------------------------
 */ 
extern int extsched_order_getJobNumOfList(INT_JobList *jobList);


/* 
 *-----------------------------------------------------------------------
 * extsched_order_getJobPriority
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object, this is jobblock not INT_Job pointer
 *
 * DESCRIPTION
 *
 * Get the priority of a job, which could be defined by bsub -sp. 
 *
 * RETURNS
 *
 * Returns priority of job.
 *
 *-----------------------------------------------------------------------
 */
extern int extsched_order_getJobPriority(INT_JobBlock *job);


/* 
 *-----------------------------------------------------------------------
 * extsched_order_getJobSeqNum
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object, this is jobblock not INT_Job pointer
 *
 * DESCRIPTION
 *
 * Get the sequence number of a job. 
 *
 * Sequence number is used by LSF internally to decide a job's FCFS 
 * priority in a queue. Beside a job's submission time, btop/bbot and
 * other relevant features; e.g, job requeue, are also considered when 
 * determining a job's sequential number.
 *
 * RETURNS
 *
 * Returns the sequence number of a job.
 *
 *-----------------------------------------------------------------------
 */
extern unsigned int extsched_order_getJobSeqNum(INT_JobBlock *job);


/* 
 *-----------------------------------------------------------------------
 * extsched_order_getJobSubmitTime
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object, this is jobblock not INT_Job pointer
 *
 * DESCRIPTION
 *
 * Get the submission time of a job. 
 *
 * RETURNS
 *
 * Returns the submission time of job.
 *
 *-----------------------------------------------------------------------
 */
extern time_t extsched_order_getJobSubmitTime(INT_JobBlock *job);


/* 
 *-----------------------------------------------------------------------
 * extsched_order_getJobUser
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object, this is jobblock not INT_Job pointer
 *
 * DESCRIPTION
 *
 * Get the user name of a job. 
 *
 * RETURNS
 *
 * Returns the user name of job.
 *
 *-----------------------------------------------------------------------
 */
extern char *extsched_order_getJobUser(INT_JobBlock *job);



/* the contexts in which checkAllocFn can be invoked
 */
typedef enum {
    SCHED_CONTEXT_UNKNOWN,  /* unknown state */
    SCHED_CONTEXT_ALLOC,    /* normal allocation */
    SCHED_CONTEXT_RESERVE,  /* reserving for PEND job */ 
    SCHED_CONTEXT_BACKFILL, /* backfilling a reservation */
    SCHED_CONTEXT_PREEMPT1, /* check whether to preempt */
    SCHED_CONTEXT_PREEMPT2, /* check currently available resources */
    SCHED_CONTEXT_PREEMPT3, /* test for small job set */
    SCHED_CONTEXT_PREEMPT4, /* validate previous preempt decision */
    SCHED_CONTEXT_TIMERSV /* time-based reservation */
} extsched_SchedContext;

typedef struct { 
    char   *host;           /* host on which the resource appears */
    char   *rsrc;           /* resource name */
    float  amount;          /* amount in allocation */
} extsched_AllocInst;

typedef enum {
    ALLOC_TYPE_UNKNOWN,     /* unknown type */
    ALLOC_TYPE_ALLOC,       /* normal allocation */
    ALLOC_TYPE_RESERVE,     /* reservation */
    ALLOC_TYPE_FORCERUN,    /* allocation for brun job */
    ALLOC_TYPE_ADDALLOC     /* resizable job growing */
} extsched_AllocType;

typedef struct {
    extsched_AllocType   type;   /* alloc/reserve/force-run */
    int                  nInst;  /* number of resource instances */
    extsched_AllocInst   **inst; /* resource instance */
} extsched_Alloc;

typedef struct {
    int                  nInst;         /* number of resource instances */
    extsched_AllocInst   **inst;        /* resource instances */
    float                reqAmt;        /* required amount */
    float                availAmt;      /* available amount */
    int                  handlerId;     /* who created the limit? */
    char                 *key;          /* limit key */
    int                  reasonId;      /* reason ID */
    char                 *reasonDetail; /* reason details */
    int                  isMainReason;  /* limit applies accross all hosts */
    int                  bucketReason;  /* is this bucket level instance */
} extsched_AllocLimit; 

typedef struct {
    int nLimit;
    extsched_AllocLimit **limit;
} extsched_AllocLimitList;


typedef struct {
    char              *rsrcName;         /** Resource name             */
    float             amount;            /** Maximum resource use      */
    int               delay;             /** Delay in achieve max use  */
    float             rate;              /** Rate at achieving max use */    
    int               numPhases;         /** Number of phases          */
    float             *phaseAmount;      /** Resource usage of phases  */
    int               *phaseDelay;       /** Delay of phases           */
    float             *phaseRate;        /** Rate of phases            */
    float              threshold;        /** minval for decresing resource, maxval for increasing resource */
} extsched_rsrcConsump;

typedef struct {
    int nRsrcConsump;
    extsched_rsrcConsump *rsrcConsump; /* rusage info */
    char *selectStr;
} extsched_rsrcReqInfo;

/*
 *-----------------------------------------------------------------------
 *
 * extsched_getCheckAllocContext --
 *
 * DESCRIPTION
 * 
 *  Function to be called from within checkAllocFn to determine
 *  the context in which this function is called. 
 * 
 * PARAMETERS
 *
 *  none
 *
 * RETURN
 *
 *  Current scheduling context.
 *
 *-----------------------------------------------------------------------
 */
extern extsched_SchedContext
extsched_getCheckAllocContext(void);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_addAllocLimit --
 *
 * PARAMETERS
 *
 *  allocLimitList [IN/OUT] : allocation limit list
 *  alloc              [IN] : an allocation
 *  host               [IN] : a host in the alloc
 *  maxTasks           [IN] : number of tasks allowed
 *  reason             [IN] : pending reason ID
 *  handlerId          [IN] : plugin handler ID
 *  bucketReason       [IN] : apply to all jobs in bucket 
 *
 * DESCRIPTION
 *
 *  To be called from within a registered checkAllocFn, in order
 *  to block a job from using the allocation.
 *
 *  The function adds a limit to allocLimitList.
 *
 *  Setting the host parameter to a particular host means that 
 *  the job is forbidden to dispatch on the particular host.
 *  Setting it to NULL means that the limit applies across all hosts. 
 *
 *  availSlots is the number of slots available.  The job is 
 *  limited to this number of slots.  Set to 0 to prevent the 
 *  job from dispatching on the host. 
 *
 *  reason is the pending reason ID that will be reported by bjobs.
 *
 *  handler is the ID for this plugin handler.
 * 
 *  If bucketReason is FALSE (=0), then the limit applies only to this job.
 *  Setting this TRUE (=1) means that the limit applies to all jobs
 *  in the same bucket (i.e. with the same scheduling attributes and 
 *  resource requirements).
 *  
 *  bucketReason should be set TRUE whenever possible, to help with
 *  scheduling performance.
 *  
 * RETURN
 *
 *  0, upon success.  -1 otherwise.
 *
 *-----------------------------------------------------------------------
 */
extern int 
extsched_addAllocLimit(INT_AllocLimitList *allocLimitList,
                       INT_Alloc *alloc,
                       char *host,
                       int maxTasks,
                       int reasonId,
                       char *reasonDetail,
                       int handlerId,
                       int bucketReason);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getPreemptableJobs --
 *
 * PARAMETERS
 *
 *  job             [IN]  : a job
 *  host            [IN]  : a host
 *  nPreemptableJob [OUT] : number of preemptable jobs
 *  preemptableJob  [OUT] : list of preemptable jobs
 *
 * DESCRIPTION
 *
 *  Function to get the list of jobs that are preemptable by the
 *  given job on the given host.
 *
 *  The list includes both SUSP and PEND jobs that are on the host.
 *
 *  Caller should call free() on preemptableJob when done with this.
 *
 * RETURN
 *
 *  0, upon success.  -1 otherwise.
 *
 *-----------------------------------------------------------------------
 */
extern int 
extsched_getPreemptableJobs(INT_Job *job,
                            char *host,
                            int *nPreemptableJob,
                            INT_Job ***preemptableJob);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getAlloc --
 *
 * PARAMETERS
 *
 *  alloc    [IN]  : internal allocation data structure
 *  infoPtr  [OUT] : describes the allocation
 *
 * DESCRIPTION
 *
 *  Function to extract information from the internal representation
 *  of a resource allocation.
 *
 *  The caller should free infoPtr with extsched_freeAlloc().
 *
 * RETURN
 *
 *  0, upon success.  -1 otherwise.
 *
 *-----------------------------------------------------------------------
 */
extern int
extsched_getAlloc(INT_Alloc *alloc,
                  extsched_Alloc **infoPtr);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_freeAlloc --
 *
 * PARAMETERS
 *
 *  info  [IN/OUT] : describes the allocation
 *
 * DESCRIPTION
 *
 *  Frees the data structure created by extsched_getAlloc().
 *
 *-----------------------------------------------------------------------
 */
extern void 
extsched_freeAlloc(extsched_Alloc **info);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getAllocLimitList --
 *
 * PARAMETERS
 *
 *  allocLimitList [IN]  : an allocation limit list  
 *  infoPtr        [OUT] : info describing the list
 *
 * DESCRIPTION
 *
 *  An allocation limit list is generated during a call to checkAllocFn.
 *
 *  Each registered checkAllocFn can add limits to this
 *  list, which serves to communicate to the scheduler framework whether 
 *  the given allocation is permitted or not.
 *
 *  extsched_getAllocLimitListInfo() generates info on the
 *  internal allocation limit 
 *  list data structure.  It is intended to be called from within a
 *  checkAllocFn.
 *
 *  Caller should free the allocated data using the function
 *  extsched_freeAllocLimitListInfo(). 
 *
 * RETURN
 *
 *  0, upon success.  -1 otherwise.
 *
 *-----------------------------------------------------------------------
 */
extern int
extsched_getAllocLimitList(INT_AllocLimitList *allocLimitList,
                           extsched_AllocLimitList **infoPtr);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_freeAllocLimitList --
 *
 * PARAMETERS
 *
 *  info           [IN/OUT] : describes an allocation limit list
 *
 * DESCRIPTION
 *
 *  Frees the data structure generated by extsched_getAllocLimitList().
 *
 *-----------------------------------------------------------------------
 */
extern void
extsched_freeAllocLimitList(extsched_AllocLimitList **info);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getChunkRef --
 *
 * PARAMETERS
 *
 *  job  [IN] : a job
 *
 * DESCRIPTION
 *
 *  Whenever an allocation is made a *chunk* is created, where
 *  a chunk is a container for one or more jobs that share 
 *  the allocation.  
 *
 *  The primarily purpose is to support the CHUNK_JOB_SIZE parameter. 
 *  However, even when this is disabled allocations are made to 
 *  degenerate chunks of size 1.
 *
 *  This function to get the chunk reference of a job may be useful
 *  to some external scheduler plugins that register notifyAlloc()
 *  callbacks.  
 *
 *  When an allocation is made, the job given as input to the 
 *  notifyAlloc() callback may be different to the job that is 
 *  given when the same allocation is destroyed.  However, the  
 *  two jobs must have the same chunk reference.
 *
 *  Use this function in the notifyAlloc() callback to get a pointer
 *  to the internal (opaque) chunk structure.
 *  
 * RETURN
 *
 *  The chunk reference for a job, if it exists. NULL otherwise.
 *
 *-----------------------------------------------------------------------
 */
extern INT_ChunkRef *
extsched_getChunkRef(INT_Job *job);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getJobAlloc --
 *
 * PARAMETERS
 *
 *  job  [IN] : a job
 *
 * DESCRIPTION
 *
 *  This function returns the current allocation that is made to a job,
 *  if it exists.  
 *
 * RETURN
 * 
 *  Pointer to the internal (opaque) allocation structure currently 
 *  assigned to the job.  NULL if there is no allocation.
 *
 *-----------------------------------------------------------------------
 */
extern INT_Alloc *
extsched_getJobAlloc(INT_Job *job);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_freeIntAlloc --
 *
 * PARAMETERS
 *
 *  allocPtr [IN/OUT] : address of an allocation structure
 *
 * DESCRIPTION
 *
 *  Use with caution.
 *
 *  Routine frees the allocation structure and sets *allocPtr NULL.
 *
 *  This function will fail to remove an allocation if that allocation
 *  has already been assigned to a job.
 *
 *  This should not be used inside of checkAlloc(), notifyAlloc() or any
 *  handler callback.
 * 
 *  Rather, this should only be called by an allocator function registered 
 *  by extsched_alloc_registerallocator().  The purpose is to destroy
 *  allocations proposed by the internal allocators.
 *
 *-----------------------------------------------------------------------
 */
extern void
extsched_freeIntAlloc(INT_Alloc **allocPtr);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getRsrcReqForJob --
 *
 * PARAMETERS
 *
 *  job [IN] : internal representation of a job
 *
 * DESCRIPTION
 *
 *  Routine to get the address of the (internal) resource requirement
 *  structure associated with the given job.
 *
 *  The caller should not free this data.
 *
 * RETURN
 *
 *  Address of the internal resource requirement structure.
 *
 *-----------------------------------------------------------------------
 */
extern INT_RsrcReq *
extsched_getRsrcReqForJob(INT_Job *job);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getRsrcReqInfo
 *
 * PARAMETERS
 *
 * rsrcreq [IN] : internal representation of rsrcreq 
 *
 * DESCRIPTION
 *
 * Routine creates extsched_rsrcReqInfo object that exposes part
 * of the job's internal resrource requirement. Currently it exposes
 * the job's select string, and the job's requested resource usage.
 *
 * The caller is responsible to free the returned data by using
 * extsched_freeRsrcReqInfo().
 *
 * RETURN
 *
 * Address to a dynamically allocated extsched_rsrcReqInfo data
 * object.
 *
 *-----------------------------------------------------------------------
 */
extern extsched_rsrcReqInfo *
extsched_getRsrcReqInfo(INT_RsrcReq *rsrcreq);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_freeRsrcReqInfo
 *
 * PARAMETERS
 *
 * info [IN|OUT] : extsched_rsrcReqInfo data to be freed
 *
 * DESCRIPTION
 *
 * The counterpart for extsched_getRsrcReqInfo() to free the returned
 * object.
 *
 * RETURN
 *
 * void
 *
 *-----------------------------------------------------------------------
 */
extern void
extsched_freeRsrcReqInfo(extsched_rsrcReqInfo **info);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_getJobInfo --
 *
 * PARAMETERS
 *
 *  job   [IN] : internal representation of a job
 *  info [OUT] : info extracted from the job
 *
 * DESCRIPTION
 *
 *  Routine to extract info from the internal representation of a job. 
 *
 *  Caller should invoke extsched_freeJobInfo() to free the returned
 *  info structure.
 *
 * RETURN
 *
 *  0 upon success. -1 otherwise.
 *
 *-----------------------------------------------------------------------
 */
extern int
extsched_getJobInfo(INT_Job *job, 
                    struct jobInfo *info);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_freeJobInfo --
 *
 * PARAMETERS
 *
 *  info [IN/OUT] : info on a job
 *
 * DESCRIPTION
 *
 *  Frees data in the info structure, allocated by extsched_getJobInfo().
 *  Does not call free() on info itself. 
 *
 *-----------------------------------------------------------------------
 */
extern void
extsched_freeJobInfo(struct jobInfo *info);


/*
 *-----------------------------------------------------------------------
 *
 * extsched_modifyJob --
 *
 * PARAMETERS
 *
 *  job     [IN] : internal representation of a job
 *  rsrcReq [IN] : a resource requirement expression 
 *
 * DESCRIPTION
 *
 *  Routine allowing an external scheduling plugin to request a 
 *  modification of a job's resource requirement expression, i.e.
 *  the bsub -R string (job level rsscreq).
 *
 *  Modification is not done immediately when this function is called.
 *  Instead, modification requests are stored in the mbschd memory, and 
 *  published at the end of a scheduling session to mbatchd.
 *
 *  It is possible that a modification request will fail if the 
 *  request violates mbatchd job modification policies.
 * 
 *  If the modification succeeds, it will take effect prior to the next 
 *  scheduling session.
 *
 *  If the modification fails, mbatchd will log a message at the 
 *  LOG_WARNING level.
 *
 *  It is possible that the ‘job’ pointer may represent more than one
 *  element from the same job array. When this occurs only the first
 *  element is acted upon by the mbatchd.
 *
 * RETURN
 *
 *  0 upon success, -1 otherwise.   
 *
 *-----------------------------------------------------------------------
 */
extern int
extsched_modifyJob(INT_Job *job, 
                   char *rsrcReq);


/*
 *-----------------------------------------------------------------------
 * extsched_resetJob
 *
 * PARAMETERS
 *
 *  job     [IN] : internal representation of a job
 *
 * DESCRIPTION
 *
 *  Routine allowing an external scheduling plugin to request a 
 *  modification of a job's resource requirement expression to it's original
 *  value at submission, or the latest bmod value.
 *
 *  Modification is not done immediately when this function is called.
 *  Instead, modification requests are stored in the mbschd memory, and 
 *  published at the end of a scheduling session to mbatchd.
 *
 *  It is possible that a modification request will fail if the 
 *  request violates mbatchd job modification policies.
 * 
 *  If the modification succeeds, it will take effect prior to the next 
 *  scheduling session.
 *
 *  If the modification fails, mbatchd will log a message at the 
 *  LOG_WARNING level.
 *
 *  It is possible that the ‘job’ pointer may represent more than one
 *  element from the same job array. When this occurs only the first
 *  element is acted upon by the mbatchd.
 *
 * RETURN
 *
 *  0 upon success, -1 otherwise.   
 *
 *-----------------------------------------------------------------------
 */
extern int
extsched_resetJob(INT_Job *job);


typedef struct {
    char *clusterId;
    int baseId;
    int index;	
} extsched_JobElement;

/*
 *-----------------------------------------------------------------------
 * extsched_getSchedJobModError
 *
 * PARAMETERS
 *
 *  job     [IN]  : internal representation of a job
 *  nElem   [OUT] : number of elements in jobList and errList
 *  jobList [OUT] : list of job elements
 *  errList [OUT] : list of LSBE_* error number of last call to
 *                  extsched_resetJob/extsched_modifyJob
 *
 * NOTE: The caller is responsible to free jobList and errList. Remember
 *       to free clusterId in each element of jobList.
 *
 * DESCRIPTION
 *
 * This call reports success/failure about the last call to 
 * extsched_modifyJob or extsched_resetJob for jobs. The associated job
 * array is broken into elements
 * and the LSBE_* error is determined. If the LSBE error is 0 it means
 * the call extsched_resetJob/extsched_modifyJob  was ultimately
 * successful, or it was never called. If a call to
 * extsched_resetJob/extsched_modifyJob is made in 
 * the current session then the success/failure should be known in the
 * next session.
 *
 * RETURN
 *
 * 0     no error
 * -1    failure
 *-----------------------------------------------------------------------
 */
extern int
extsched_getSchedJobModError(INT_Job *job,
			     int *nElem,
			     extsched_JobElement **jobList,
			     int **errList);

/*
 *-----------------------------------------------------------------------
 *
 * extsched_setMainReason --
 *
 * PARAMETERS
 *
 *  reason      [IN] : internal representation of a pending reason
 *  msg         [IN] : a description of why the reason is being set
 *  bucketLevel [IN] : if FALSE the pending reason applies specifically
 *                     to the job
 *
 * DESCRIPTION
 *  Set the main pending reason (not host based). PEND_EXTSCHED_REASON
 *
 * RETURN
 *
 *  0 upon success, -1 otherwise.   
 *
 *-----------------------------------------------------------------------
 */
extern int
extsched_setMainReason(INT_Reason *reason,
                       char *msg,
                       int bucketLevel);



/*
 *-----------------------------------------------------------------------
 *
 * extsched_getHost
 *
 * PARAMETERS
 *
 * hostname    [IN] : the name of the host
 * clustername [IN] : the name of the cluster
 *
 * RETURN
 *  pointer to internal host data
 *
 *-----------------------------------------------------------------------
 */
extern INT_Host *
extsched_getHost(char *hostname, char *clustername);


/*
 *-----------------------------------------------------------------------
 * 
 * extsched_clustername
 *
 * PARAMETERS
 *
 * none
 *
 * DESCRIPTION
 *
 * Returns the local cluster's name. Do not free the returned pointer.
 *
 *------------------------------------------------------------------------
 */
extern char *
extsched_clustername(void);


/*
 *-----------------------------------------------------------------------
 * 
 * extsched_getHostID
 *
 * This function will provide the host ID (hostname and clusternam)
 * provided with the internal host pointer. This function will
 * allocate the memory for the hostname and clustername so the caller
 * is responsible to free.
 *
 * PARAMETERS
 *
 * hostPtr     [IN] : pointer to internal host data
 * hostname    [OUT] : the name of the host
 * clustername [OUT] : the name of the cluster the host belongs to
 *
 * RETURN
 *
 *  0 upon success, -1 otherwise.   
 *
 *------------------------------------------------------------------------
 */
extern int
extsched_getHostID(INT_Host *hostPtr, char **hostname, char **clustername);


/*
 *-----------------------------------------------------------------------
 * extsched_get_INT_Job
 *
 * This is meant to be used from inside an allocator function. Pass in 
 * the jobBlock pointer to obtain the corresponding INT_Job pointer.
 *
 * PARAMETERS
 *
 * jobBlock    [IN] : the jobBloack parameter passed into the allocator
 *                    function
 *
 * RETURN
 *
 *  an INT_Job pointer, do not free the returned pointer  
 *
 *------------------------------------------------------------------------
 */
extern INT_Job *
extsched_get_INT_Job(INT_JobBlock *jobBlock);


/*
 *-----------------------------------------------------------------------
 * extsched_determineEvent
 *
 * This is a helper function to be used from within a notifyAlloc
 * callback function to determine what is happening with the job.
 *
 * PARAMETERS
 *
 * job            [IN] : job pointer passed into notifyAlloc callback
 *                       function
 * alloc          [IN] : alloc pointer passed into notifyAlloc callback
 *                       function
 * allocLimitList [IN] : allocLimitList pointer passed into notifyAlloc
 *                       callback function
 * flag           [IN] : flag passed into notifyAlloc callback function
 *
 * RETURN
 *
 * a positive interger describing the event (SCH_FM_EVE_*)
 * 0 if unknown
 *------------------------------------------------------------------------
 */
extern int
extsched_determineEvent(INT_Job *job, 
			INT_Alloc *alloc,
			INT_AllocLimitList *allocLimitList,
			int flag);

/* event type returned by extsched_determineEvent() */
#define SCH_FM_EVE_ALLOCATE             0x001
#define SCH_FM_EVE_DEALLOCATE           0x002
#define SCH_FM_EVE_RESERVE              0x004
#define SCH_FM_EVE_CANCEL               0x008
#define SCH_FM_EVE_SUSPEND              0x010
#define SCH_FM_EVE_RESUME               0x020
#define SCH_FM_EVE_REPLAY               0x040
#define SCH_FM_EVE_ALLOCATE_BRUN        0x080
#define SCH_FM_EVE_ADDALLOC             0x100
#define SCH_FM_EVE_RMALLOC              0x200

/* job status */
#define CORE_JOBSTAT_NULL         0x00
#define CORE_JOBSTAT_PEND         0x01
#define CORE_JOBSTAT_PSUSP        0x02
#define CORE_JOBSTAT_RUN          0x04
#define CORE_JOBSTAT_SSUSP        0x08
#define CORE_JOBSTAT_USUSP        0x10
#define CORE_JOBSTAT_EXIT         0x20
#define CORE_JOBSTAT_DONE         0x40
#define CORE_JOBSTAT_PDONE       (0x80) /* Post job process done successfully */
#define CORE_JOBSTAT_PERR       (0x100) /* Post job process has error */
#define CORE_JOBSTAT_WAIT       (0x200) /* Chunk job waiting its turn to exec */
#define CORE_JOBSTAT_SCHED       0x400  /* Dispatched job from Scheduler*/
#define CORE_JOBSTAT_PREEMPTED   0x800  /* Preempted job*/
#define CORE_JOBSTAT_LOCK       0x1000  /* Locked SSUSP job should be resumed
                                           by scheduler. */
#define CORE_JOBSTAT_UNKWN     0x10000
#define CORE_JOBSTAT_MIG    0x40000000  /* Job is being migrated */


/*----------------------------------------------------------------------
 * Signature for functions that determine if a job should be preempted.
 *
 * Use these functions to allow/disallow the scheduler to preempt
 * (ie. suspend) a specific job.
 *
 * PARAMS
 *
 * job - (IN) internal job object, should this job be preempted?
 *
 * RETURNS
 *
 * 0: The job should not be preempted.
 * !=0: It is ok if the job is preempted.
 *
 * SEE ALSO
 *
 * extsched_register_canBePreempted
 *----------------------------------------------------------------------
 */
typedef int (*CanBePreemptedFn)(INT_Job *job);

/*------------------------------------------------------------------------
 * extsched_register_canBePreempted
 *
 * Register a function that will decide if a job can be preempted.
 *
 * The registered function will be called during preemption when
 * considering whether a job should be preempted.
 *
 * PARAMS
 *
 * canBePreemptedFunc - (IN) pointer to the function that decides if
 *                           a job can be preempted
 *
 * RETURN
 *
 * 0 if succesfully registered. <0 if failed to register the function.
 *
 * SEE ALSO
 *
 * CanBePreemptedFn
 *------------------------------------------------------------------------
 */
int extsched_register_canBePreempted( CanBePreemptedFn canBePreemptedFunc );


/* hash table API
 */
typedef void INT_hTab; /* hash table */
typedef void INT_hEnt; /* table entry */
typedef void INT_sTab; /* search */

/*------------------------------------------------------------------------
 * extsched_createTabAndInit
 *
 * Create and initialize a hash table object.
 *
 * PARAMETERS
 *
 * slots [IN] number of slots
 *
 * RETURN
 *
 * pointer to a hash table object
 *------------------------------------------------------------------------
 */
extern INT_hTab *extsched_createTabAndInit(int slots);

/*------------------------------------------------------------------------
 * extsched_createTab
 *
 * Create an un-initialized hash table object. You probabably should
 * use extsched_createTabAndInit().
 *
 * PARAMETERS
 *
 * none
 *
 * RETURN
 *
 * pointer to a (un-initialized) hash table object
 *------------------------------------------------------------------------
 */
extern INT_hTab *extsched_createTab(void);

/*------------------------------------------------------------------------
 * extsched_destroyTab
 *
 * Free the hash table object. The table should be emptied first.
 *
 * PARAMETERS
 *
 * t [IN|OUT] a hash table
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_destroyTab(INT_hTab **t);

/*------------------------------------------------------------------------
 * extsched_create_sTab
 *
 * Create a search table object.
 *
 * PARAMETERS
 *
 * none
 *
 * RETURN
 *
 * pointer to a search object
 *------------------------------------------------------------------------
 */
extern INT_sTab *extsched_create_sTab(void);

/*------------------------------------------------------------------------
 * extsched_destroy_sTab
 *
 * Free the search table object.
 *
 * PARAMETERS
 *
 * s [IN|OUT] search object
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_destroy_sTab(INT_sTab **s);

/*------------------------------------------------------------------------
 * extsched_get_hData
 *
 * Retreive the data associated with a hash table entry.
 *
 * PARAMETERS
 *
 * e [IN] pointer to a hash table entry
 *
 * RETURN
 *
 * pointer to the data associated with the entry
 *------------------------------------------------------------------------
 */
extern void *extsched_get_hData(INT_hEnt *e);

/*------------------------------------------------------------------------
 * extsched_set_hData
 *
 * Set the data associated with a hash table entry.
 *
 * PARAMETERS
 *
 * e [IN|OUT] pointer to a hash table entry
 * d [IN] pointer to the data
 *
 * RETURN
 *
 * 0 success
 * -1 failure
 *------------------------------------------------------------------------
 */
extern int extsched_set_hData(INT_hEnt *e, void *d);

/*------------------------------------------------------------------------
 * extsched_get_key
 *
 * Retreive the key associated with a hash table entry.
 *
 * PARAMETERS
 *
 * e [IN] pointer to a hash table entry
 *
 * RETURN
 *
 * pointer to the key name
 *------------------------------------------------------------------------
 */
extern char *extsched_get_key(INT_hEnt *e);

/*------------------------------------------------------------------------
 * extsched_initTab_
 *
 * Initialize a hash table object.
 *
 * PARAMETERS
 *
 * t [IN|OUT] pointer to a hash table object
 * slots [IN] number of slots
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_initTab_(INT_hTab *t, int slots);

/*------------------------------------------------------------------------
 * extsched_freeTab_
 *
 * Remove everything from a hash table and free up
 * the memory using custom provided free function.
 *
 * PARAMETERS
 *
 * t [IN|OUT] pointer to a hash table object
 * freeFunc [IN] pointer to a custom free function, if NULL free()
 * is used
 * freeTabFlag [IN] if TRUE the table object is freed and pointer
 * set to NULL, otherwise the table object is in
 * an uninitialized state
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_freeTab_(INT_hTab **t, void (*freeFunc)(void *),
			      int freeTabFlag);

/*------------------------------------------------------------------------
 * extsched_TabEmpty_
 *
 * Check whether the table is empty.
 *
 * PARAMETERS
 *
 * t [IN] pointer to a hash table object
 *
 * RETURN
 * 1 table is empty
 * 0 table is not empty
 * -1 error, t was NULL
 *------------------------------------------------------------------------
 */
extern int extsched_TabEmpty_(INT_hTab *t);

/*------------------------------------------------------------------------
 * extsched_getEnt_
 *
 * Search a hash table for an entry corresponding to key.
 *
 * PARAMETERS
 *
 * t [IN] pointer to a hash table object
 * k [IN] key name
 *
 * RETURN
 *
 * pointer to an entry object
 *------------------------------------------------------------------------
 */
extern INT_hEnt *extsched_getEnt_(INT_hTab *t, const char *k);

/*------------------------------------------------------------------------
 * extsched_addEnt_
 *
 * Add a new entry in hash table. If the entry already exists,
 * then nothing is done.
 *
 * PARAMETERS
 *
 * t [IN|OUT] pointer to a hash table object
 * k [IN] key name
 * n [OUT] new flag
 *
 * RETURN
 *
 * The return value is a pointer to the entry. If *n
 * isn't NULL, then *n is filled in with TRUE if a new entry
 * was created, and FALSE if an entry already exists with the given key.
 *------------------------------------------------------------------------
 */
extern INT_hEnt *extsched_addEnt_(INT_hTab *t, const char *k, int *n);

/*------------------------------------------------------------------------
 * extsched_rmEnt_
 *
 * Remove the given hash table entry and free memory
 * associated with it, but do not free hData.
 *
 * PARAMETERS
 *
 * t [IN|OUT] pointer to a hash table object
 * e [IN|OUT] pointer to entry object
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_rmEnt_(INT_hTab *t, INT_hEnt *e);

/*------------------------------------------------------------------------
 * extsched_rmEntAndFreeData
 *
 * Remove the given hash table entry and free memory
 * associated with it, and free hData.
 *
 * PARAMETERS
 *
 * t [IN|OUT] pointer to a hash table object
 * e [IN|OUT] pointer to entry object, after this call e is no
 * longer valid
 * freeFunc [IN] pointer to a custom free function, if NULL free()
 * is used
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_rmEntAndFreeData(INT_hTab *t, INT_hEnt *e,
				      void (*freeFunc)(void *));

/*------------------------------------------------------------------------
 * extsched_firstEnt_
 *
 * Sets things up for a complete search of all entries in
 * the hash table.
 *
 * PARAMETERS
 *
 * t [IN] pointer to a hash table object
 * s [OUT] pointer to search object
 *
 * RETURN
 *
 * Return the address of the first entry in the hash table, or NULL if
 * the table is empty.
 *------------------------------------------------------------------------
 */
extern INT_hEnt *extsched_firstEnt_(INT_hTab *t, INT_sTab *s);

/*------------------------------------------------------------------------
 * extsched_nextEnt_
 *
 * This function returns successive entries in the hash table.
 *
 * PARAMETERS
 *
 * s [IN|OUT] pointer to search object
 *
 * RETURN
 *
 * Return a pointer to the next entry in the table, or NULL when the end
 * of the table is reached.
 *------------------------------------------------------------------------
 */
extern INT_hEnt *extsched_nextEnt_(INT_sTab *s);

/*------------------------------------------------------------------------
 * extsched_freeRefTab_
 *
 * Free the hash table used as reference. When more than one table contains
 * addresses of the same data structure in memory then only the first
 * hash table has to destroy the data, the other tables, called reference
 * tables just need to destroy their entries.
 *
 * PARAMETERS
 *
 * t [IN|OUT] hash table object
 * freeTabFlag [IN] if TRUE the table object is freed and pointer
 * set to NULL, otherwise the table object is in
 * an uninitialized state
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_freeRefTab_(INT_hTab **t, int freeTabFlag);

/*------------------------------------------------------------------------
 * extsched_delRef_
 *
 * This function is to be used when the hash table uses reference
 * data, i.e. more that one hash table contains the address of the data.
 * This is usefull when the data can be searched using different keys.
 * Using the reference avoid to duplicate the memory.
 *
 * PARAMETERS
 *
 * t [IN|OUT] pointer to a hash table object
 * e [IN|OUT] pointer to entry object
 *
 * RETURN
 *
 * none
 *------------------------------------------------------------------------
 */
extern void extsched_delRef_(INT_hTab *t, INT_hEnt *e);

/* end of hash table API
 */


/*-------------------------------------------------------------------------
 * deprecated functions 
 *-------------------------------------------------------------------------
 */
extern struct candHostGroup *lsb_cand_getnextgroup(void *candGroupList);
extern struct hostResources *lsb_host_resources(INT_Host *host);
extern char *getResType(const struct resources *);
extern struct jobInfo *lsb_get_jobinfo(void *job,  int phase);
extern int lsb_reason_set(void *reasonPtr, 
			  struct candHost *candHost, 
			  int reasonId);
extern void lsb_cand_removehost(struct candHostGroup *group, int index);
extern struct hostSlot *lsb_cand_getavailslot(struct candHost *candHost);
extern int lsb_resreq_registerhandler(int handlerId, 
				      RsrcReqHandlerType *handler);
extern void lsb_resreq_setobject(INT_RsrcReq *resreq, 
				 int handlerId, 
				 char *key, 
				 void *handlerData);
/* lsb_job_getrsrcreqobject() always returns NULL */
extern void *lsb_job_getrsrcreqobject(INT_Job *jobPtr, int handlerId);
extern int lsb_alloc_registerallocator(AllocatorFn  allocator);
extern char **lsb_resreq_getextresreq(INT_RsrcReq *resreq, int *msgCnt);
extern int lsb_resreq_getqueextsched(INT_RsrcReq *resreq, 
                                     char **mand_extsched,
                                     char  **default_extsched);
extern char **lsb_job_getextresreq(INT_JobBlock *jobPtr, int *msgCnt);
extern int lsb_job_getaskedslot(void *jobPtr);
extern struct hostSlot* lsb_alloc_gethostslot(void *alloc, 
					      void *candGroupList, 
				              int *hostCnt,
					      struct candHostGroup **group);
extern int lsb_alloc_type(void *alloc);
extern int lsb_alloc_modify(void *jobptr, void **alloc, 
			    struct hostSlot *host,
			    void *reason,
			    struct candHostGroup *group);
extern int lsb_order_registerOrderFn4AllQueues(JobOrderFn4Que  orderFn);
extern int lsb_order_isJobListEmpty(void * jobList); 
extern void *lsb_order_getFirstJobOfList(void * jobList);
extern void *lsb_order_getNextJobOfList(void *currJob,  
					void *jobList);
extern void *lsb_order_getPreJobOfList(void *currJob,  
				       void *jobList);
extern int lsb_order_getJobNumOfList(void *jobList);
extern int lsb_order_getJobPriority(void* job);
extern unsigned int lsb_order_getJobSeqNum(void* job);
extern time_t lsb_order_getJobSubmitTime(void* job);
extern char * lsb_order_getJobUser(void *job);

#endif /* _LSSCHED_H_ */
