/* Name: Robert Osborne
*  WSUID:H796Q978
*  Assignment 2
*  Description: Write a program that creates a child process that generates a sequence of numbers to shared memory, from which the parent outputs the result.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>



int main(int argc, char **argv)
{

	if (argc != 2) /*error if more than 1 number input is received */
	{
		fprintf(stderr, "Usage: %s <integer value>\n", argv[0]);
		return 1;
	}
	char *ptr2;/*used to check if non-numbers are used*/
	ptr2 = argv[1];
	char CheckInput = *ptr2; /*used to check if non-numbers are used*/
	if (CheckInput =='+')
	{
		ptr2++;
		CheckInput = *ptr2;	
	}
	while (CheckInput != '\0' && ('0' <= CheckInput && CheckInput <= '9')) /*checks if all input is a number*/
	{
		ptr2++;
		CheckInput = *ptr2;
	}
	if (CheckInput != '\0') /*non-number character was found*/
	{
		fprintf(stderr, "non-integer character used, exiting.\n");
		return 1;
	}

	int check = atoi(argv[1]); /*error if negative number or a number of 500000 is received */

	if(check <= 0 || check >= 500000)
	{
		printf("Negative or overly large number entered, exiting.\n");
		return 1;
	}
	const int SIZE = 4096;
	const char *name = "test";
	int shm_fd;
	void *ptr; /*where the string will be stored*/

	int status; /*For when checking for parent to wait */
	pid_t pid;
	pid = fork();

	if(pid == -1) /*Error in forking */
	{
		fprintf(stderr, "Error in Forking.\n");
		return 1;
	}
	
	else if(pid == 0)
	{
		int x;/*to hold the number that is being inputed into the formula */
		int c;/* Counter */
		x = check;
		char temp[8];

		shm_fd=shm_open(name, O_CREAT | O_RDWR, 0666);
		ftruncate(shm_fd, SIZE);
		ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		while (x > 1) /* Runs through up to 10 times, also stops if x hits 1. */
		{	
			sprintf(ptr, "%d, ", x);
			sprintf(temp, "%d, ", x);
			ptr += strlen(temp); /*moves the pointer to the next byte in memory*/
			c++;	
			if(x%2 == 0)/*if x is even */
			{
				x = x/2;
	
			}
			else /*if odd */
			{
				x = 3*x+1;
			}
			if (c %10 == 0) /*new line every ten outputs*/
			{
				sprintf(ptr, "\n");
				ptr += 1;
			}
		}
		sprintf(ptr, "%d", x); /*the final number, no "n"*/
		sprintf(temp, "%d", x);
		ptr+= strlen(temp);
		
	}
	else
	{
		pid = wait(&status); /*wait till child is done */
		shm_fd = shm_open(name, O_RDONLY, 0666);
		ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
		printf("%s",(char *)ptr); /*print what was stored in shared memory*/
		printf("\n\n");

		shm_unlink(name);
		return 0;
	}
	return 0;
}
