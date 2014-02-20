#include <lib.h>
#include <unistd.h>
#include <minix/syslib.h>
#define publish _publish

PUBLIC int publish(char* group_name, int publisher_pid, char* mesg)
{
  	message m;
	int return_code;
	
  	m.m1_i1=publisher_pid;
  	m.m1_i2=strlen(group_name);
	m.m1_i3=strlen(mesg);
	
  	m.m1_p1=(char *)malloc(strlen(group_name)+1);  
	memset(m.m1_p1,0,strlen(group_name)+1);
  	strncpy(m.m1_p1,group_name,strlen(group_name));
  	strncat(m.m1_p1,"\0",1);

	m.m1_p2=(char *)malloc(strlen(mesg)+1);
	memset(m.m1_p2,0,strlen(mesg)+1);
	strncpy(m.m1_p2,mesg,strlen(mesg));
	strncat(m.m1_p2,"\0",1);

  	_syscall(PM_PROC_NR, GROUP_PUBLISH, &m);
	
	return_code=m.m1_i1;
	return return_code;
}

