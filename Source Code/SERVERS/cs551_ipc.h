/* Function prototypes. */
struct mproc;
struct stat;
struct mem_map;

/* CS551 IPC System Calls*/
_PROTOTYPE( int do_group_create, 	(void)      	);
_PROTOTYPE( int do_group_subscribe,     (void)         	);
_PROTOTYPE( int do_group_publish, 	(void)        	);
_PROTOTYPE( int do_group_retrieve,      (void)         	);
_PROTOTYPE( int do_group_delete, 	(void)         	);
_PROTOTYPE( int do_list_members,        (void)         	);
_PROTOTYPE( int do_list_groups, 	(void)         	);

/* Helper Functions */
_PROTOTYPE( int do_init,		(void)                                  );	
_PROTOTYPE( int do_group_exists,        (char* group_name)                      );
_PROTOTYPE( int do_getname, (char* user_side,  char* ker_side, int size) 	);
_PROTOTYPE( int do_return_message, (char* user_side, char* ker_side, int size)	);
_PROTOTYPE( int do_find_first_empty,	(void)                                  );
