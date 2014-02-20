/* Include the Groups Functions*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Api Function Calls */
int api_create(int);
int api_subscribe(int);
int api_publish(int);
int api_delete(int);
int api_retrieve(int);
int api_list_members(int);
int api_list_groups(int);
void get_name(char*);

int main(int argc, char* argv)
{
	char *name;
	char c;
	int X=0;
	int i=0;
	int choice=0;
	int pid = getpid();
	key_t key_id;

	/* Interactive While Loop */
	while(1)
	{

		system("/usr/bin/clear");
		printf("********************************************************\n");
        printf("*  PID=%d   |  CS 551  PROJECT 2A - IPC |  Group: 11   *\n",pid);
 		printf("********************************************************\n");
		printf("\n");
        printf("<<<<<<<<<<<<<<<<<<<<< Main Screen >>>>>>>>>>>>>>>>>>>>>>>>\n");
		printf(" Press 1 To create a New Interest Group.  \n");
		printf(" Press 2 To subscribe to Interest Group.  \n");
		printf(" Press 3 To publish to an Interest Group. \n");
		printf(" Press 4 To retrieve the Messages from Interest Group. \n");
		printf(" Press 5 To delete the Interest Group.  \n");
		printf(" Press 6 To display the Interest Group Members.  \n");
		printf(" Press 7 display all Active Interest Groups. \n");
		printf(" Press 8 to quit application \n");
        printf("Please Enter your Choice for : %d: ",pid);
        scanf("%i",&choice);		
		getchar();
		
		/* Create New Group */
		if(choice==1)
		{
			X=api_create(pid);
			printf("\n");
			switch(X)
			{
				case 0: 
					printf("*******************************************\n");
					printf("*   <<  Success: New Group Created!  >>   *\n");			
					printf("*******************************************\n\n");					
					break;				
				case 1:
					printf("********************************************\n");
					printf("*  $&@# Error: Group Already Exists! $&@#  *\n");
					printf("********************************************\n\n");
					break;
				case 3:
					printf("****************************************************\n");
					printf("*  $&@# Error: No More Groups May be added! $&@#   *\n");
					printf("****************************************************\n\n");
					break;
				default:
					break;
			}
			printf("Press Enter To Continue....");		
			getchar();
			choice=0;
			X=0;		
		}
		
		if(choice==2)
		{
			X=api_subscribe(pid);
                        printf("\n");
			switch(X)
                        {
                                case 0:
                                        printf("*******************************************\n");
					printf("* <<  Success: Subscriber Registered! >>  *\n");			
					printf("*******************************************\n\n");	
                                        break;
                                case 1:
                                        printf("********************************************\n");
					printf("*    $&@# Error: Group Not Found! $&@#     *\n");
					printf("********************************************\n\n");
                                        break;
                                case 2:
                                        printf("***********************************************\n");
					printf("* $&@# Error: PID=%d Already Subscribed! $&@# *\n", pid);
					printf("***********************************************\n\n");
                                        break;
				case 3:
					printf("\n  $&@# Error: Publisher Role! $&@#\n\n");
                                        printf("*************************************************\n");
                                        printf("* Publisher may not subscribe to the same group.*\n");    
                                        printf("*************************************************\n");
                                default:
                                        break;
                        }
			printf("Press Enter To Continue....");		
			getchar();
			choice=0;
			X=0;
		}

		if(choice==3)
		{
			X=api_publish(pid);
			printf("\n");                        
        		switch(X)
                        {
                                case 0:
                                        printf("*******************************************\n");
					printf("*    <<  Success: Message Published!  >>  *\n");			
					printf("*******************************************\n\n");
                                        break;
                                case 1:
                                        printf("********************************************\n");
					printf("*    $&@# Error: Group Not Found! $&@#     *\n");
					printf("********************************************\n\n");
                                        break;
                                case 2:
                                        printf("\n  $&@# Error: Only Publisher Can Publish Messages! $&@#\n\n");
                                        break;
                                case 3:
                                        printf("**********************************************\n");
					printf("*  $&@# Error: Message Buffer is full! $&@#  *\n");
					printf("**********************************************\n\n");
                                default:
                                        break;
                        }
			printf("Press Enter To Continue....");		
			getchar();
			choice=0;
			X=0;
		}

		if(choice==4)
		{
			X=api_retrieve(pid);
                        printf("\n");
      			switch(X)
                        {
                                case 0:

                                        printf("*******************************************\n");
					printf("*   <<  Success: Messages Retrieved!  >>  *\n");			
					printf("*******************************************\n\n");
                                        break;
                                case 1:
                                        printf("**********************************************\n");
					printf("*      $&@# Error: Group Not Found! $&@#     *\n");
					printf("**********************************************\n\n");
                                        break;
                                case 2:
                                        printf("****************************************************\n");
					printf("* $&@# Error: Not a Subscriber of this Group! $&@# *\n");
					printf("****************************************************\n\n");
                                        break;
                                case 3:
                                        printf("****************************************************\n");
					printf("*  $&@# Error: No Unread Messages Availabe! $&@#   *\n");
					printf("****************************************************\n\n");
                                default:

                                        break;
                        }
			printf("Press Enter To Continue....");		
			getchar();
			choice=0;
			X=0;
		}

		if(choice==5)
		{
			X=api_delete(pid);
			printf("\n");  			
                        switch(X)
                        {
                                case 0:
                                        printf("*******************************************\n");
					printf("*        <<  Success: Group Deleted!  >>  *\n");			
					printf("*******************************************\n\n");
                                        break;
                                case 1:
                                        printf("************************************************\n");
					printf("*      $&@# Error: Group Not Found! $&@#       *\n");
					printf("************************************************\n\n");
                                        break;
                                case 2:
                                        printf("****************************************************\n");
					printf("*  $&@# Error: Only Owner Can Delete Group! $&@#   *\n");
					printf("****************************************************\n\n");
                                        break;
                                default:
                                        break;
                        }
                        printf("Press Enter To Continue....");			
			getchar();
			choice=0;
			X=0;
		}
		
		if(choice==6)
		{
			X=api_list_members(pid);
			printf("\n");                        
                        switch(X)
                        {
                                case 0:
                                        printf("*******************************************\n");
					printf("*  Success: Group Members List Retrieved! *\n");			
					printf("*******************************************\n\n");
                                        break;
                                case 1:
                                        printf("********************************************\n");
					printf("*    $&@# Error: Group Not Found! $&@#     *\n");
					printf("********************************************\n\n");
                                        break;
                                case 2:
                                        printf("***************************************************************\n");
					printf("*  $&@# Error: Only Group Owner can view Members List! $&@#   *\n");
					printf("***************************************************************\n\n");
                                        break;
                                case 3:
                                        printf("****************************************************\n");
					printf("*    $&@# Error: Group Has No Subscribers! $&@#    *\n");
					printf("****************************************************\n\n");
                                default:
                                        break;
                        }
			printf("Press Enter To Continue....");		
			getchar();
			choice=0;
			X=0;
		}

		if(choice==7)
		{
			X=api_list_groups(pid);
			printf("\n");     			
                        switch(X)
                        {
                                case 0:
                                        printf("*******************************************\n");
					printf("*  Success: Active Groups List Retrieved! *\n");			
					printf("*******************************************\n\n");
                                        break;
                                case 1:
                                        printf("****************************************************\n");
					printf("*       $&@# Error: There are no Groups! $&@#      *\n");
					printf("****************************************************\n\n");
                                        break;
				default:
					printf("Cant switch\n");	
					break;
			}
			printf("Press Enter To Continue....");		
			getchar();
			choice=0;
			X=0;
		}

		if(choice==8)
		{
			exit(1);
		}

		fflush(stdin);
		fflush(stdout);
	}
	
	return 0;
}

int api_create(int pid)
{
	char g_name[100];
	printf("Enter the Group Name to be Created>>>: ");
	get_name(g_name);
	return create(g_name,pid);	
}


int api_subscribe(int pid)
{
	char g_name[100];	
	printf("Enter the Group Name to Subsribe  >>>: ");
	get_name(g_name);
	return subscribe(g_name,pid);
}

int api_publish(int pid)
{
	char g_name[100];
	char message[256];	
	printf("Enter the Group Name to Publish in>>>: ");
	get_name(g_name);
	printf("Enter A NewMessage to Publish     >>>: ");
	get_name(message);
	return publish(g_name,pid,message);
}

int api_retrieve(int pid)
{
	int ret;
	char g_name[100];
	char *messages;	
	printf("Enter the Group Name to Retrieve  >>>: ");
	get_name(g_name);
	messages=retrieve(g_name, pid, &ret);

	if(ret == 0)
	{
		printf("\nMESSAGES:\n");
		printf("%s",messages);
	}
	return ret;
}

int api_delete(int pid)
{
	char g_name[100];
	printf("Enter the Group Name to Delete    >>>: ");
	get_name(g_name);	

	return delete_(g_name, pid);
}

int api_list_members(int pid)
{
	int ret;
	char g_name[100];
	char *members;
	printf("Enter the Group Name to List      >>>: ");
	get_name(g_name);	
	members=ListMembers(g_name,pid,&ret);

	if(ret == 0)
	{
		printf("\nGROUP %s MEMBERS: \n",g_name);
		printf("%s",members);
	}
	return ret;
}

int api_list_groups(int pid)
{
	int ret;
	char *groups;
	groups=ListGroups(&ret);
	if(ret == 0)
	{
		printf("\nACTIVE GROUPS: \n");
		printf("%s",groups);
	}
	return ret;
}

/* Helper Function */
void get_name(char *name)
{
	char group_name[100];	
	int i=0;
	char c;
	while(i<100)
	{
		c=getchar();
		if(c == '\n')
		{	
			group_name[i]='\0';
			break;
		}
		group_name[i]=c;
		i++;
	}
	strncpy(name,group_name,strlen(group_name)+1);
	strncat(name,"\0",1);
}

