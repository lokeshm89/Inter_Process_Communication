#include <lib.h>
#include <unistd.h>
#include <minix/syslib.h>
#define subscribe _subscribe

PUBLIC int subscribe(char* group_name, int sub_pid)
{
  	message m;
  	int return_code;

	/* Fill Transport */
	m.m1_i1=sub_pid;
        m.m1_i2=strlen(group_name);

  	m.m1_p1=(char *)malloc(strlen(group_name)+1);
        memset(m.m1_p1,0,strlen(group_name)+1);
        strncpy(m.m1_p1,group_name,strlen(group_name));
        strncat(m.m1_p1,"\0",1);

	_syscall(PM_PROC_NR, GROUP_SUBSCRIBE, &m);

	return_code=m.m1_i1;
	if(m.m1_p1 != NULL)
	{
		free(m.m1_p1);
		m.m1_p1=NULL;
	}
	return return_code;
}

