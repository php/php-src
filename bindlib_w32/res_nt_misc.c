/* this function for Windows NT specific resolver stuff L. Kahn */

#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <stdio.h>
#include <ctype.h>
#include <resolv.h>
#include <netdb.h>
#include "conf/portability.h"

char *pathnetworks, *pathhosts, *pathresconf;
int res_paths_initialized = 0;


/* lgk new function to initialize path variables that are outside the scope of the main */
/* for backwards compatibility allow either resolv.conf or resolv.ini */
void init_res_paths()
{
   FILE *fp;
   
	pathhosts = (char *)malloc(MAX_PATH);
	if (!ExpandEnvironmentStrings(_PATH_HOSTS, pathhosts, MAX_PATH)) 
		syslog(LOG_ERR, "ExpandEnvironmentStrings(_PATH_HOSTS) failed: %m\n");

	pathnetworks = (char *)malloc(MAX_PATH);
	if (!ExpandEnvironmentStrings(_PATH_NETWORKS, pathnetworks, MAX_PATH))
		syslog(LOG_ERR, "ExpandEnvironmentStrings(_PATH_NETWORKS) failed: %m\n");

	pathresconf = (char *)malloc(MAX_PATH);
	if (!ExpandEnvironmentStrings(_PATH_RESCONF, pathresconf, MAX_PATH)) 
		syslog(LOG_ERR, "ExpandEnvironmentStrings(_PATH_RESCONF) failed: %m\n");

     	else
          {
            if ((fp = fopen(pathresconf, "r")) == NULL)
              {
                /* try alternate name */
 	        if (!ExpandEnvironmentStrings(_ALT_PATH_RESCONF, pathresconf, MAX_PATH)) 
	  	  syslog(LOG_ERR, "ExpandEnvironmentStrings(_ALT_PATH_RESCONF) failed: %m\n");
              }
            else fclose(fp);
	  }

		
    res_paths_initialized = 1;
}


