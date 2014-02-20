#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GROUPS 256
#define MAX_MESSGS 5

#include <sys/types.h>

/* Subscribers Structure */
typedef struct
{
        /* Subscriber Process ID */
        int pid_;

        /* Messages Bitmap */
        int mesg[5];
}ss;

/* INTEREST GROUP STRUCTURE */
typedef struct
{
        /* Interest Group Name */
        char g_name[100];

        /* 5 Max Interest Grop Messages */
        char messages[5][256];

        /* Interest Group Creator/Publisher - Process ID*/
        int publisher_id;

        /* Number of Subscribers in the group */
        int sub_count;

        /* Subscribers PID's and Message Buffer Housekeeping*/
        ss subscribers_[256];

        /* Number of Subscibers that have retrieved Message */
        int retrieve_count[5];

}interest_group;

interest_group groups[MAX_GROUPS];
int NUM_GROUPS;
int FIRST;

#include "pm.h"
#include <minix/callnr.h>
#include <signal.h>
#include <sys/svrctl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/sysinfo.h>
#include <minix/type.h>
#include <minix/vm.h>
#include <string.h>
#include <machine/archtypes.h>
#include <lib.h>
#include <assert.h>
#include "mproc.h"
#include "param.h"
#include "cs551_ipc.h"

/* Input: char* group_name, int creator_pid     */
/* Output:                                      */
/* Returns  0      {Success}               */
/* Returns  1      {Groups Exists}         */
/* Returns  2      {Groups Maxed}          */
PUBLIC int do_group_create(void)
{
	int creator_pid = m_in.m1_i1; 		/* Get the group creator pid */
	int str_size=m_in.m1_i2; 		/* Group Name Size           */
	register struct mproc *resp = mp;	/* Return structure	     */
	char *group_name;			/* Group Name String 	     */
	int flag=-1;
	int i,j; i=j=0;
	
	/* Initialize the Data Structure IPC */
	if(!FIRST)
		do_init();

	/* Get the Group Name From the User */
	group_name=(char *)malloc(str_size + 1);
	do_getname((char*)m_in.m1_p1, group_name, str_size);

	/* Test if the group is already created */
	i=do_group_exists(group_name);

	if(i != -1)
	{
		/* Group Exists */
                resp->mp_reply.m1_i1=1;
                free(group_name);
                return 1;
	}
	else
	{
		i=do_find_first_empty();
		if(i==-1)
		{
    			/* Groups Maxed */
                        resp->mp_reply.m1_i1=2;
                        free(group_name);
                        return 2;
		}
		else
		{	
			/* Create New Group */
			NUM_GROUPS++;

			/* Set Group Name */
			memset(groups[i].g_name,0,sizeof(char)*100);
			strncpy(groups[i].g_name,group_name,strlen(group_name));
			strncat(groups[i].g_name,"\0",1);

			/* Set the Initial Messages to null */
			memset(groups[i].messages,0,5*256*sizeof(char));
			for(j=0; j<5; j++)
				strncat(groups[j].messages[j],"\0",1);
		
			/* Set the Subscriber Count to 0 */
			groups[i].sub_count=0;
	
			/* Set the creator ID */		
			groups[i].publisher_id=creator_pid;
	
			/* Return Success Group Created */
			resp->mp_reply.m1_i1=0;	
   			free(group_name);
			return 0;
		}
	}	
}

/* Input: char* group_name, int sub_pid         */
/* Output:                                      */
/* Returns  0   {Success}                       */
/* Returns  1   {Group Not Found}               */
/* Returns  2   {Already Member }               */
/* Returns  3   {Publisher Role }               */
PUBLIC int do_group_subscribe(void)
{


        int sub_pid = m_in.m1_i1;           	/* Get the subscriber pid    */
        int str_size=m_in.m1_i2;                /* Group Name Size           */
        register struct mproc *resp = mp;       /* Return structure          */
        char *group_name;                       /* Group Name String         */
        int i,j,flag; i=j=flag=0;

        /* Get the Group Name From the User */
        group_name=(char *)malloc(str_size + 1);
        do_getname((char*)m_in.m1_p1, group_name, str_size);

        /* Test if the group Exists */
        i=do_group_exists(group_name);

	/* Group Not Found */
	if(i == -1)
	{
        	resp->mp_reply.m1_i1=1;
                free(group_name);
                return 1;
	}
	else
	{
		/* Publisher trying to subscribe */
		if(groups[i].publisher_id == sub_pid)
		{
			resp->mp_reply.m1_i1=3;
                	free(group_name);
                	return 3;
		}

		/* Make Sure Not Already a Subscriber */
		for(j=0; j<MAX_GROUPS; j++)
		{
			/* Already a Subscriber of this Group */
			if(groups[i].subscribers_[j].pid_==sub_pid)
			{	
                      		resp->mp_reply.m1_i1=2;
                        	free(group_name);
                        	return 2;
			}
		
			/* Get first 0 index */
			if(flag==0 && groups[i].subscribers_[j].pid_==0)
				flag=j;
		}
	
		/* Register the Subscriber to the Group */ 
		groups[i].subscribers_[flag].pid_=sub_pid;
		groups[i].sub_count++;	
		resp->mp_reply.m1_i1=0;
                free(group_name);
                return 0;
	}
}

/* Input: char* group_name, int sub_pid, char* mesg */
/* Output:                                          */
/* Returns  0   {Success}                           */
/* Returns  1   {Group Not Found  }                 */
/* Returns  2   {Not the Publisher}                 */
/* Returns  3   {Buffer is Full   }                 */

PUBLIC int do_group_publish(void)
{



        int publisher_pid = m_in.m1_i1;         /* Get the Publisher pid     */
        int str_size=m_in.m1_i2;                /* Group Name Size           */
	int msg_size=m_in.m1_i3;		/* Message Size              */
        register struct mproc *resp = mp;       /* Return structure          */
        char *group_name;                       /* Group Name String         */
	char *mesg;				/* Message To Write	     */
        int i,j,k,flag; i=j=k=flag=0;

        /* Get the Group Name From the User */
        group_name=(char *)malloc(str_size + 1);
        do_getname((char *)m_in.m1_p1, group_name, str_size);

        /* Test if the group Exists */
        i=do_group_exists(group_name);

        /* Group Not Found */
        if(i == -1)
        {
                resp->mp_reply.m1_i1=1;
                free(group_name);
                return 1;
	}
	else
	{
		/* Not the Publisher */
        	if(groups[i].publisher_id != publisher_pid)
        	{
     	        	resp->mp_reply.m1_i1=2;
                	free(group_name);
                	return 2;
        	}

		/* Get the message via virtual copy */
		mesg=(char *)malloc(msg_size + 1);
        	do_getname((char *)m_in.m1_p2, mesg, msg_size);

        	/* Write Message if Buffer NOT FULL */
        	for(j=0; j<5; j++)
        	{
                	/* Write the message here */
                	if(!strcmp(groups[i].messages[j],"\0") )
                	{
                        	memset(groups[i].messages[j],0,sizeof(char)*256);
                        	strncpy(groups[i].messages[j],mesg,msg_size);
                        	strncat(groups[i].messages[j],"\0",1);
				
                        	/* Alert the Subscribers that they have unread messages */
                        	for(k=0; k<256; k++)
                                	groups[i].subscribers_[k].mesg[j]=1;

                        	flag=1;
                                resp->mp_reply.m1_i1=0;
                        	free(group_name);
               			free(mesg);	
				return 0;
                	}
        	}

        	/* Buffer is FULL if flag==0 */
        	if(!flag)
        	{
                	resp->mp_reply.m1_i1=3;
                        free(group_name);
                        free(mesg);
                	return 3;
        	}
	}
}


/* Input: char* group_name, int sub_pid      */
/* Output:                                   */
/* Returns  0   {Success}                    */
/* Returns  1   {Group Not Found  }          */
/* Returns  2   {Not a Subscriber }          */
/* Returns  3   {No Messages 	  }          */
PUBLIC int do_group_retrieve(void)
{


        int sub_pid = m_in.m1_i1;         	/* Get the Subscriber pid    */
        int str_size=m_in.m1_i2;                /* Group Name Size           */
        register struct mproc *resp = mp;       /* Return structure          */
        char *group_name;                       /* Group Name String         */
        char *response_string;                  /* Retrieved Messages        */
        int i,j,k,flag; i=j=k=flag=0;

        /* Get the Group Name From the User */
        group_name=(char *)malloc(str_size + 1);
        do_getname((char *)m_in.m1_p1, group_name, str_size);

        /* Test if the group Exists */
        k=do_group_exists(group_name);

        /* Group Not Found */
        if(k == -1)
        {
                resp->mp_reply.m1_i1=1;
                free(group_name);
                return 1;
        }
        else
        {
		/* Check if they are a subscriber */
        	/* Find the subscriber struct intex */
        	for(i=0; i<256; i++)
        	{
                	/* Subscriber Found at index i */
                	if(groups[k].subscribers_[i].pid_==sub_pid)
                	{
                        	flag=1;
                       	 	break;
                	}
        	}

        	/* Subscriber Not Found */
        	if(!flag)
        	{
                	resp->mp_reply.m1_i1=2;
                	free(group_name);
                	return 2;
        	}
		/* Get the Messages Subscribers Messages*/
		else
        	{
			response_string=(char *)malloc(sizeof(char)*1024);
                	memset(response_string,0,sizeof(char)*1024);
                	strncat(response_string,"\t-",2);
                	flag=0;

                	for(j=0;j<5;j++)
                	{
                        	if(groups[k].subscribers_[i].mesg[j]==1)
                        	{
                                	if(strcmp(groups[k].messages[j],"\0"))
                                	{
						/* Write Message To Retrun String */
     						strncat(response_string,groups[k].messages[j],strlen(groups[k].messages[j]));
                                		strncat(response_string,"\n\t-",3);
               
                                        	groups[k].subscribers_[i].mesg[j]=0;
                                        	groups[k].retrieve_count[j]++;

                                        	/* Determine if the message should be deleted */
                                        	if(groups[k].sub_count ==  groups[k].retrieve_count[j])
                                        	{
                                                	memset(groups[k].messages[j],0,sizeof(char)*256);
                                                	strncat(groups[k].messages[j],"\0",1);
                                                	groups[k].retrieve_count[j]=0;
                                        	}
                                        	flag=1;
                                	}
                        	}
                	}
			/* No Messages For the Subscriber */			
			if(!flag)
                        {
                		resp->mp_reply.m1_i1=3;
				free(response_string);
                		free(group_name);
                		return 3;
                        }
			else
			{                	
				strncat(response_string,"\0",1);
                		response_string[strlen(response_string)-2]='\0';

				/* Send Messages back to User in User-Space */	
              			do_return_message(m_in.m1_p2,response_string,strlen(response_string));
			
				resp->mp_reply.m1_i1=0;
				free(response_string);
                                free(group_name);
                                return 0;

			}
		}
	}
}



/* Input: char* group_name, int pid      */
/* Output:                                   */
/* Returns  0   {Success}                    */
/* Returns  1   {Group Not Found  }          */
/* Returns  2   {Not a Publisher  }          */
PUBLIC int do_group_delete(void)
{


        int sub_pid = m_in.m1_i1;         	/* Get the Publisher pid     */
        int str_size=m_in.m1_i2;                /* Group Name Size           */
        register struct mproc *resp = mp;       /* Return structure          */
        char *group_name;                       /* Group Name String         */
        int i,j,k,flag; i=j=k=flag=0;

        /* Get the Group Name From the User */
        group_name=(char *)malloc(str_size + 1);
        do_getname((char *)m_in.m1_p1, group_name, str_size);

        /* Test if the group Exists */
        i=do_group_exists(group_name);

        /* Group Not Found */
        if(i == -1)
        {
                resp->mp_reply.m1_i1=1;
                free(group_name);
                return 1;
        }
	else
	{
                /* Not the Group Owner */
                if(groups[i].publisher_id != sub_pid)
                {
                        resp->mp_reply.m1_i1=2;
                        free(group_name);
                        return 2;
                }
		
		/* Decrement Subscriber Count */
		NUM_GROUPS--;
		
		/* Delet the Group Name */
                memset(groups[i].g_name,0,100);
                strncpy(groups[i].g_name,"NULL\0",5);

                /* Re-Set the Initial Messages to null */
                memset(groups[i].messages,0,5*256*sizeof(char));
                for(j=0; j<5; j++)
                        strncat(groups[j].messages[j],"\0",1);

                /* Set the Subscriber Count to 0 */
                groups[i].sub_count=0;

                /* Set the creator ID to 0*/
                groups[i].publisher_id=0;
	
		/* Reset the subscribers */
		memset(groups[i].subscribers_,0,sizeof(ss)*256);

                /* Return Success Group Deleted */
                resp->mp_reply.m1_i1=0;
                free(group_name);
		return 0;
	}
}

/* Input: char* group_name, int sub_pid      */
/* Output:                                   */
/* Returns  0   {Success}                    */
/* Returns  1   {Group Not Found  }          */
/* Returns  2   {Not a Publisher  }          */
PUBLIC int do_list_members(void)
{


        int sub_pid = m_in.m1_i1;               /* Get the Publisher pid     */
        int str_size=m_in.m1_i2;                /* Group Name Size           */
        register struct mproc *resp = mp;       /* Return structure          */
        char *group_name;                       /* Group Name String         */
	char *response_string;		        /* List of Group Members     */
	char temp_pid[5];			/* Temp pids to send back    */
        int i,j,k,flag; i=j=k=flag=0;

        /* Get the Group Name From the User */
        group_name=(char *)malloc(str_size + 1);
        do_getname((char *)m_in.m1_p1, group_name, str_size);

        /* Test if the group Exists */
        i=do_group_exists(group_name);

        /* Group Not Found */
        if(i == -1)
        {
                resp->mp_reply.m1_i1=1;
                free(group_name);
                return 1;
        }
        else
        {
                /* Not the Group Owner */
                if(groups[i].publisher_id != sub_pid)
                {
               		resp->mp_reply.m1_i1=2;
                        free(group_name);
                        return 2;
                }

		/* No Subscribers Found */
		if(groups[i].sub_count==0)
		{			
      			resp->mp_reply.m1_i1=3;
                        free(group_name);
                        return 3;
		}

		response_string=(char *)malloc(sizeof(char)*1024);
		memset(response_string,0,sizeof(char)*1024);
		strncat(response_string,"\t-",2);
		/* Get the list of Subscribers PID's */
		for(j=0; j<256; j++)
		{
			if(groups[i].subscribers_[j].pid_ != 0)
			{
				memset(temp_pid,0,5);
				sprintf(temp_pid,"%d",groups[i].subscribers_[j].pid_);
				strncat(response_string,temp_pid,strlen(temp_pid));
				strncat(response_string,"\n\t-",3);
			}
		}
		strncat(response_string,"\0",1);
		response_string[strlen(response_string)-2]='\0';

		/* Virtual Copy the Return String */
		do_return_message(m_in.m1_p2,response_string,strlen(response_string));

		/* Clean Up */
		free(response_string);
		free(group_name);
		resp->mp_reply.m1_i1=0;
        	return 0;
	}
}

/* Input:   No input                         */
/* Output:                                   */
/* Returns  0   {Success}                    */
/* Returns  1   {Group Not Found  }          */

PUBLIC int do_list_groups(void)
{


        register struct mproc *resp = mp;       /* Return structure          */
        char *response_string;                  /* List of Group Members     */
        int i,j,k,flag; i=j=k=flag=0;
	
	if(NUM_GROUPS==0)
	{
		resp->mp_reply.m1_i1=1;
		return 1;
	}
	else
	{
 		response_string=(char *)malloc(sizeof(char)*1024);
                memset(response_string,0,sizeof(char)*1024);
                strncat(response_string,"\t-",2);

                /* Get the list of Subscribers PID's */
                for(i=0; i<256; i++)
                {
                        if(strcmp(groups[i].g_name,"NULL"))
                        {
                                strncat(response_string,groups[i].g_name,strlen(groups[i].g_name));
                                strncat(response_string,"\n\t-",3);
                        }
                }
                strncat(response_string,"\0",1);
                response_string[strlen(response_string)-2]='\0';

                /* Virtual Copy the Return String */
                do_return_message(m_in.m1_p2,response_string,strlen(response_string));

                /* Clean Up */
                free(response_string);
                resp->mp_reply.m1_i1=0;
	        return 0;
	}
}

PUBLIC int do_init()
{
	int i=0;
	for(i=0;i<MAX_GROUPS;i++)
        {
        	memset(groups[i].g_name,0,100);
                strncpy(groups[i].g_name,"NULL\0",5);
        }
	FIRST++;
}


PUBLIC int do_getname(char* user_side, char* ker_side, int size)
{
        /* Perform Virtual Data Copy from user space */
        sys_datacopy(who_e,(vir_bytes)user_side,PM_PROC_NR,(vir_bytes)ker_side,(phys_bytes)size+1);
}

PUBLIC int do_return_message(char* user_side, char* ker_side, int size)
{
        /* Perform Virtual Data Copy from user space */
        sys_datacopy(PM_PROC_NR,(vir_bytes)ker_side,who_e,(vir_bytes)user_side,(phys_bytes)size+1);
}

/* Input:   No input                         */
/* Output:                                   */
/* Returns:  i index of the Group Exists     */
/* Returns: -1 if the Group !Exists    	     */
PUBLIC int do_group_exists(char* group_name)
{

	int i=0;	

        /* Test if the group is already created */
        for(i=0; i<MAX_GROUPS; i++)
                if(!strcmp(groups[i].g_name,group_name) )
                        return i;
	return -1;
}

/* Input:   No input                            */
/* Output:                                      */
/* Returns:  i index of firt empty group    	*/
/* Returns: -1 Groups Capacity Reached 256 	*/
PUBLIC int do_find_first_empty(void)
{

	int i=0;
	 
	/* Find the first NULL Structure */
        for(i=0; i<MAX_GROUPS; i++)
                if(!strcmp(groups[i].g_name,"NULL") )
                        return i;

        if(i==MAX_GROUPS)
                return -1;
}
