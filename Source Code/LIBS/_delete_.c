#include <lib.h>
#include <unistd.h>
#include <minix/syslib.h>
#define delete_ _delete_

PUBLIC int delete_(char* group_name, int pid)
{
  	message m;
	int return_code;

  	m.m1_i1=pid;
  	m.m1_i2=strlen(group_name);
	
  	m.m1_p1=(char *)malloc(strlen(group_name)+1);  
	memset(m.m1_p1,0,strlen(group_name)+1);
  	strncpy(m.m1_p1,group_name,strlen(group_name));
  	strncat(m.m1_p1,"\0",1);

  	_syscall(PM_PROC_NR, GROUP_DELETE, &m);
	return_code=m.m1_i1;
	
	return return_code;
}

