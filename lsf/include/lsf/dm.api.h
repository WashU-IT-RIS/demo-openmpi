
#ifndef _DM_API_H_
#define _DM_API_H_

#ifdef __cplusplus
extern "C"
{
#endif


extern int daserrno;

extern struct dmParams* dm_params(int);
extern int dm_admin(bdataSubcommandOptionID, char*);
extern struct queryClustersReply* dm_clusters();
extern struct dmCacheResult* dm_cache(struct dmCacheRequest*, struct remoteDMs*);
extern int dm_local(char**, char**, int*);
extern int dm_stage_in(struct dmStginRequest*, char **);
extern int dm_stage_out(struct dmStgoutRequest*, char **);
extern struct dmTagInfo* dm_list_tags(char *, int*, struct remoteDMs*);
extern int dm_delete_tag(char *, char *, struct remoteDMs *);
extern int dm_chgrp(char *, char *, char *, struct remoteDMs *,
		    int, struct dmChPartialSuccess *);
extern int dm_chmod(char *, char *, char *, struct remoteDMs *,
		    int, struct dmChPartialSuccess *);
extern int getRegisteredDmdCluster(char *, struct remoteDMs **);
extern char *das_strerror(int);
extern void das_perror(int, const char *);
extern int das_init(int);
extern void das_reset(void);
extern void freeDmParams(struct dmParams **ppDasParams);


#ifdef __cplusplus
}
#endif

#endif /* _DM_API_H_ */

