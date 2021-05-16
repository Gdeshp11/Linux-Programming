/* Poll system call example to monitor multiple fd's for IO */

#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>

#define TIMEOUT_SECONDS 10
#define NUM_FD 2 

int main (void)
{
	struct pollfd fds[NUM_FD];

	/* monitor stdin file descriptor for reading */
	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	/* monitor stdout file descriptor for writing */
	fds[1].fd = STDOUT_FILENO;
	fds[1].events = POLLOUT;

  while(1)
  {
    
	int ret = poll(fds, 2, TIMEOUT_SECONDS * 1000);

	if (ret == -1) {
		perror ("\nERROR: Poll failed");
		return -1;
	}

	if (!ret) 
		printf ("\n%d Timeout..", TIMEOUT_SECONDS);
	

	if (fds[0].revents & POLLIN)
		printf ("\nstdin is readable");

	if (fds[1].revents & POLLOUT)
		printf ("\nstdout is writable");
    
   sleep(1);
    
  }

	return 0;

}
