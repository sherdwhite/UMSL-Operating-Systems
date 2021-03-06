// Sherd White
// cs4760 Assignment 1
// 10/04/2017

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define PERM (S_IRUSR | S_IWUSR)
#define SIZE 50
#define LENGTH 132

typedef struct {
	int id;
	int index;  //key_t key;
	char data[SIZE][LENGTH];
} shared_memory;

time_t now;
struct tm* tm_info;

int main(int argc, char * argv[]) 
{
	if (argc <= 2)
	{
		fprintf(stderr, "Not enough arguements passed..\n");
		return 1;
	}
	
	int pid = atoi(argv[1]);
	int index = atoi(argv[2]);
	
	int key = 92111;
	int shm_id = shmget(key, sizeof(shared_memory), PERM | IPC_CREAT);
	if (shm_id == -1) {
		perror("Failed to find shared memory segment");
		return 1;
	}
	// printf("My palin segment id is %d\n", shm_id);

	// attach shared memory segment
	shared_memory* ptr = (shared_memory*)shmat(shm_id, NULL, 0);
	// shmat(segment_id, NULL, SHM_RDONLY) to attach to read only memory
	if (ptr == (void*)-1) {
		perror("Failed to attach existing shared memory segment");
		return 1;
	}
	// printf("My palin ptr address is %x\n", ptr);
	// printf("Palin: The id is %d, the index is %d.\n", ptr->id, ptr->index);

	// int i = 0;
	// Testing array of strings for data.
	// for(i = 0; i < 50; i++){
		// printf("%s", ptr->data[i]);
	// }

	// Start from leftmost and rightmost corners of str
	int l = 0;
	int r = strlen(ptr->data[index]) - 1;
	int palin = 1;
	// Keep comparing characters while they are same
	while (r > l)
	{
			if (ptr->data[index][l] != ptr->data[index][r]) {
				// printf("%s Not Palindrome\n", word);
				palin = 0;
				break;
			}
			l++;
			r--;
	}
	// if (palin == 1) {
		// printf("%s Palindrome\n", word);
	// }

	/* Critical section */
	time_t cur_time;
    char* cur_t_string;
    cur_time = time(NULL);
    if (cur_time == ((time_t)-1))
    {
        fprintf(stderr, "Failure to get the current time.\n");
    }
    cur_t_string = ctime(&cur_time); //convert to local time format
    if (cur_t_string == NULL)
    {
        fprintf(stderr, "Failure to convert the current time.\n");
    }
    printf("PID %d entered Critical Section at: %s", pid, cur_t_string);
	// sleep for random amount of time (between 0 and 2 seconds);
	srand(time(NULL));
	int random = rand() % 3;
	sleep(random);
	// Write palindromes and non-palindromes to their files.
	FILE *file;
	char indx[2];
	char cpid[2];
	sprintf(indx, "%d", index);
	sprintf(cpid, "%d", pid);
	if (palin == 1) {
		// write to palin.out
		file = fopen("palin.out", "a");
		fputs(cpid, file);
		fputs(" ", file);
		fputs(indx, file);
		fputs(" ", file);
		fputs(ptr->data[index], file);
		fputs("\n", file);
	}
	else {
		// write to nopalin.out
		file = fopen("nopalin.out", "a");
		fputs(cpid, file);
		fputs(" ", file);
		fputs(indx, file);
		fputs(" ", file);
		fputs(ptr->data[index], file);
		fputs("\n", file);
	}
	fclose(file);
	
	// sleep for random amount of time (between 0 and 2 seconds);
	random = rand() % 3;
	sleep(random);
	// execute code to exit from critical section;
    cur_time = time(NULL);
    if (cur_time == ((time_t)-1))
    {
        fprintf(stderr, "Failure to get the current time.\n");
    }
    cur_t_string = ctime(&cur_time); //convert to local time format
    if (cur_t_string == NULL)
    {
        fprintf(stderr, "Failure to convert the current time.\n");
    }
    printf("PID %d exited Critical Section at: %s", pid, cur_t_string);

	
	// detach from memory segment
	int detach = shmdt(ptr);
	if (detach == -1){
		perror("Failed to detach shared memory segment");
		return 1;
	}

    return 0;
}