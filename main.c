#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "sysInfo.h"

//FILE pointer initializer
FILE *lAdj;
FILE *s1Temp;
FILE *s2Temp;
FILE *s1Limit;
FILE *s2Limit;
FILE *tar1;
FILE *tar2;

int main() {
	//The limit adjuster. This is the only method to control the fan speed as far as I know, so that is why I made this two-mode system
	lAdj = fopen("/sys/devices/platform/temperatures/limit_adjust", "w");
	//Check for g4-fandriver config files
	if ((tar1 = fopen("/etc/g4-fandriver/target1.conf", "r")) == NULL){
		printf("use the install script like I said. or make the file /etc/g4-fandriver/target1.conf and put something like 50\n");
		exit(1);
	}
	if ((tar2 = fopen("/etc/g4-fandriver/target2.conf", "r")) == NULL){
		printf("use the install script like I said. or make the file /etc/g4-fandriver/target2.conf and put something like 48\n");
		exit(1);
	}
	//Load more files
	if ((s1Temp = fopen("/sys/devices/platform/temperatures/sensor1_temperature", "r")) == NULL) {
		printf("Error! Missing file /sys/devices/platform/temperatures/sensor1_temperature!\nPlease report this error message to an issue on Github to help contribute to this project's support!\n");
		printSysInfo();
		exit(1);
	}
	if ((s2Temp = fopen("/sys/devices/platform/temperatures/sensor2_temperature", "r")) == NULL) {
		printf("Error! Missing file /sys/devices/platform/temperatures/sensor2_temperature!\nPlease report this error message to an issue on Github to help contribute to this project's support!\n");
		printSysInfo();
		exit(1);
	}
	if ((s1Limit = fopen("/sys/devices/platform/temperatures/sensor1_limit", "r")) == NULL) {
		printf("Error! Missing file /sys/devices/platform/temperatures/sensor1_limit!\nPlease report this error message to an issue on Github to help contribute to this project's support!\n");
		printSysInfo();
		exit(1);
	}
	if ((s2Limit = fopen("/sys/devices/platform/temperatures/sensor2_limit", "r")) == NULL) {
		printf("Error! Missing file /sys/devices/platform/temperatures/sensor2_limit!\nPlease report this error message to an issue on Github to help contribute to this project's support!\n");
		printSysInfo();
		exit(1);
	}
	//Load the target config files into targetTemp1 and targetTemp2
	int targetTemp1;
	int targetTemp2;
	fscanf(tar1, "%d", &targetTemp1);
	fscanf(tar2, "%d", &targetTemp2);
	printf("Driver loaded successfully!\n");
	printf("Begining driver loop.\n");
	/*
	 * Modes:
	 * 0: Quiet mode
	 * Quiet mode offsets the target by 5, making it easier to reach
	 * 1: Cool mode
	 * Cool mode offsets the target by -2 making it harder to reach 
	 */
	int mode = 0; //Default mode is quiet mode.
	int offset = 5; //The offset for quiet mode is 5.
	while(true) {
		//Print the mode (For debugging)
		//printf("Mode: %d\n", mode);
		//printf("hi\n");
		//Sleep two seconds to reduce CPU load
		sleep(2);
		//True target calculations (the config target plus the offset)
		int target1 = targetTemp1 + offset;
		int target2 = targetTemp2 + offset;
		//Reinitialize variables
		int temp1;
		int temp2;
		int cT1;
		int cT2;
		//Close the files
		fclose(s1Temp);
		fclose(s2Temp);
		fclose(s1Limit);
		fclose(s2Limit);
		//Reopen them so that the stream is refreshed
		s1Limit = fopen("/sys/devices/platform/temperatures/sensor1_limit", "r");
		s2Limit = fopen("/sys/devices/platform/temperatures/sensor2_limit", "r");
		s1Temp = fopen("/sys/devices/platform/temperatures/sensor1_temperature", "r");
		s2Temp = fopen("/sys/devices/platform/temperatures/sensor2_temperature", "r");
		//Load values from our files into our variables
		fscanf(s1Temp, "%d", &temp1);
		fscanf(s2Temp, "%d", &temp2);
		fscanf(s1Limit, "%d", &cT1);
		fscanf(s2Limit, "%d", &cT2);
		//Offset and mode  updaters. Changes won't take effect until next loop cycle
		//printf("Target 1: %d Target 2: %d\n", targetTemp1, targetTemp2);
		if (targetTemp1 - temp1 <= 0) {
			mode = 1;
			offset = -2;
		}
		if (targetTemp2 - temp2 <= 0) {
			mode = 1;
			offset = -2;
		}
		
		if (targetTemp1 - temp1 >= 4) {
			mode = 0;
			offset = 5;
		}
		if (targetTemp2 - temp2 >= 4) {
			mode = 0;
			offset = 5;
		}
		//Set the limit adjuster to target minus 4
		if (mode == 0) {
			if (target2 < target1) {
				int tc = target2 - 4;
				int t = tc - temp2;
				fprintf(lAdj, "%d", t);
			}
			if (target1 < target2) {
				int tc = target1 - 4;
				int t = tc - temp1;
				fprintf(lAdj, "%d", t);
			}
		}
		if (mode == 1) {
			if (target2 > target1) {
				int tc = target2 - 4;
				int t = tc - temp2;
				fprintf(lAdj, "%d", t);
			}
			if (target1 > target2) {
				int tc = target1 - 4;
				int t = tc - temp1;
				fprintf(lAdj, "%d", t);
			}
		}
		fclose(lAdj);
		lAdj = fopen("/sys/devices/platform/temperatures/limit_adjust", "w");
		
	}
}