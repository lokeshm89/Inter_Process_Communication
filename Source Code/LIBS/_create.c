#include <lib.h>
#include <unistd.h>
#include <minix/syslib.h>
#define create 	_create

PUBLIC int create(char* group_name, int creator_pid)
{
	/* Return Codes are returned from the kernel 	*/
	/* Return: 0 Success 				*/
	/* Return: 1 Groups Exists 			*/
	/* Return: 2 Groups Maxed			*/

  	message m;
	char *temp;
	int return_code;

  	m.m1_i1=creator_pid;
  	m.m1_i2=strlen(group_name);
  	m.m1_p1=(char *)malloc(strlen(group_name)+1);  
	memset(m.m1_p1,0,strlen(group_name)+1);
  	strncpy(m.m1_p1,group_name,strlen(group_name));
  	strncat(m.m1_p1,"\0",1);
/*    
RESPONSE MESSAGE
	m.m1_p2=(char *)malloc(strlen(group_name)+1);
	memset(m.m1_p2,0,strlen(group_name)+1);
	printf("\nUSER: The Address of m1_p1 after malloc is: %p\n",m.m1_p1);
	printf("USER: The Address of m1_p2 after malloc is: %p\n",m.m1_p2);
	temp=m.m1_p2;
	<syscall>
	printf("%s\n",temp);	
*/
  	_syscall(PM_PROC_NR, GROUP_CREATE, &m);	
	return_code=m.m1_i1;

	/* Clean Up */
	if(m.m1_p1 != NULL)
	{
		free(m.m1_p1);
		m.m1_p1=NULL;
	}
	return return_code;
}

