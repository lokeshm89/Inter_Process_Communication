#include <lib.h>
#include <unistd.h>
#include <minix/syslib.h>
#define ListMembers _ListMembers

PUBLIC char* ListMembers(char* group_name, int pid, int* ret)
{
  	message m;
	char *members;

  	m.m1_i1=pid;
  	m.m1_i2=strlen(group_name);

  	m.m1_p1=(char *)malloc(strlen(group_name)+1);  
	memset(m.m1_p1,0,strlen(group_name)+1);
  	strncpy(m.m1_p1,group_name,strlen(group_name));
  	strncat(m.m1_p1,"\0",1);

	/* Will hold the Retrurn String From Kernel */
	m.m1_p2=(char *)malloc(sizeof(char)*1024);
	memset(m.m1_p2,0,sizeof(char)*1024);

	/* Set the Place Holder */
	members=m.m1_p2;

  	_syscall(PM_PROC_NR, LIST_MEMBERS, &m);

	*ret=m.m1_i1;
	return members;
}

