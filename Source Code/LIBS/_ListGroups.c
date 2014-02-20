#include <lib.h>
#include <unistd.h>
#include <minix/syslib.h>
#define ListGroups _ListGroups

PUBLIC char* ListGroups(int* ret)
{
  	message m;
	char *groups;

	m.m1_p2=(char *)malloc(sizeof(char)*1024);
	memset(m.m1_p2,0,sizeof(char)*1024);

	groups=m.m1_p2;
  	_syscall(PM_PROC_NR, LIST_GROUPS, &m);
	
	*ret=m.m1_i1;
	return groups;
}

