#include <sys/utsname.h>
#include <stdio.h>
#include "sysInfo.h"

int printSysInfo() {
	struct utsname uname_pointer;
	uname(&uname_pointer);
	printf("System name: %s\n", uname_pointer.sysname);
	printf("Nodename: %s\n", uname_pointer.nodename);
	printf("Release: %s\n", uname_pointer.release);
	printf("Version: %s\n", uname_pointer.version);
	printf("CPU Info: %s\n", uname_pointer.machine);
}