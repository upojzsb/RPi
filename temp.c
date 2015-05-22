/*
 This program used for showing RPi's CPU or (and) GPU's Temp
 CPU's Temp is saved as a file '/sys/class/thermal/thermal_zone0/temp' ,and this program open this file and read temp from it, transform 'temp' value into float, print it
 GPU's Temp can be return from another program with parameter ,it is '/opt/vc/bin/vcgencmd measure_temp', and this program fork a child process and use 'execve' systemcall replace the child to the program mentioned above.

 UPO-JZSB 2015-05-22 
 2015-05-22 the first version
 */
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CPU_TEMP "/sys/class/thermal/thermal_zone0/temp"
#define BUFF 1024
#define GPU_TEMP_PATH "/opt/vc/bin/vcgencmd"
#define GPU_TEMP_ARGV1 "measure_temp"

void cpu_temp(void);
void gpu_temp(void);

int main(int argc, char ** argv)
{
	if(argc < 2)//Show both CPU and GPU temp
	{
		cpu_temp();
		gpu_temp();
		exit(EXIT_SUCCESS);
	}
	else if(argc == 2)//Show CPU or GPU temp
	{
		if(!strncmp(argv[1] , "cpu", 3))
			cpu_temp();
		else if(!strncmp(argv[1], "gpu", 3))
			gpu_temp();
		else//Error
		{
			fprintf(stderr, "Usage: %s {cpu|gpu} \n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
}

void cpu_temp(void)
{
	int fd;
	char * tmp;
	float raltmp;

	fd = open(CPU_TEMP, O_RDONLY);
	tmp = malloc((size_t)BUFF);

	read(fd, tmp, BUFF);

	raltmp = (float)atof(tmp);
	raltmp /= 1000;

	fprintf(stdout, "CPU temp=%.1f'C\n", raltmp);

	free(tmp);
}

void gpu_temp(void)
{
	if(fork() == 0)
	{
		fprintf(stdout, "GPU ");
		fflush(stdout);

		char * argument[] = {GPU_TEMP_PATH, GPU_TEMP_ARGV1, (char *)NULL};
		
		execve(GPU_TEMP_PATH, argument, (char **)NULL);
	}

	wait((int *)NULL);
}
