/* 
    Assignment 1: Comments redacted
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include <string.h>

typedef struct {
        int pid, status, ruid, euid, rgid, egid;
        time_t time;
        double stime, utime;
    } cLog;

void childForkFailed(int);
int bico(int, int);
int factorial(int);
void processChild(int, int, int, int, int);
void printExitStats(cLog[], time_t, struct rusage);
void waitForChildren(cLog[]);

static time_t *t;
static double *st, *ut;
static int *rid, *eid, *rgid, *egid;


int main() {
    pid_t c1,c2,c3,c4;
    time_t t0;
    struct rusage ru;
    cLog child[4];
    if (getrusage(RUSAGE_SELF, &ru) == -1) perror("Parent getrusage failed");
    t = mmap(NULL, sizeof *t, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    ut = mmap(NULL, sizeof *ut, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    st = mmap(NULL, sizeof *st, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	rid = mmap(NULL, sizeof *rid, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	eid = mmap(NULL, sizeof *eid, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	rgid = mmap(NULL, sizeof *rgid, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	egid = mmap(NULL, sizeof *egid, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    t0 = time(NULL);


    if ((c1 = fork()) < 0)
        childForkFailed(1);
    else if (c1 == 0)
		processChild(1,-1,-1,-1,-1);
    
    sleep(2); // To allow initial welcome message to be observed

    if ((c2 = fork()) < 0)
        childForkFailed(2);
	else if (c2 == 0) 
    	processChild(2, 2, 0, 10, 0);

    if ((c3 = fork()) < 0)
        childForkFailed(3);
	else if (c3 == 0)  
        processChild(3, 3, 1, 9, 2);

    if ((c4 = fork()) < 0)
        childForkFailed(4);
	else if (c4 == 0)
    	processChild(4,-1,-1,-1,-1);
	
    waitForChildren(child);
    printExitStats(child, t0, ru);

    return 0;
}

void waitForChildren(cLog child[]) {
	pid_t pid;
	int status;

    int childCount = 0;
    while ((pid = wait(&status)) > 0) {
        child[childCount].pid = (int)pid;
        child[childCount].ruid = *rid;
        child[childCount].euid = *eid;
        child[childCount].rgid = *rgid;
        child[childCount].egid = *egid;
        child[childCount].status = status;
        child[childCount].time = *t;
        child[childCount].utime = *ut;
        child[childCount].stime = *st;
        childCount++;
    }
}

void printExitStats(cLog child[], time_t t0, struct rusage ru) {

	printf("\nListing exit statistics of processes...\n");
    sleep(3);
    int i;
    for (i = 0; i < 4; i++) {
        printf("Exit status of Child %d (PID: %d) was %d at %s"
        	"\tSystem CPU time: %6.1f microseconds | User CPU time: %6.1f microseconds. \n"
        	"\tReal UID: %d. Effective UID: %-5d | Real GID: %d.  Effective GID: %d\n\n",
         i+1, child[i].pid, child[i].status, ctime(&child[i].time), child[i].stime, child[i].utime, child[i].ruid, child[i].euid, child[i].rgid, child[i].egid);

        sleep(3);
    }
    sleep(3);
    int z; for (z = 0; z < 5000000; z++) { int x; x+=z; } //usertime loop
    if (getrusage(RUSAGE_SELF, &ru) == -1) perror("Parent getrusage failed");
    time_t t1 = time(NULL);
    printf("Exit status of Parent  (PID: %d) was %d at %s"
        	"\tSystem CPU time: %6.1f microseconds | User CPU time: %6.1f microseconds. \n"
        	"\tReal UID: %d. Effective UID: %-5d | Real GID: %d.  Effective GID: %d\n\n",
         getpid(), 0, ctime(&t1), (double)ru.ru_stime.tv_usec, (double)ru.ru_utime.tv_usec, getuid(), geteuid(), getgid(), getegid());

    

    char username[32];
	cuserid(username);
	if (username == NULL || strlen(username) == 0) perror("username grab fail");
    printf("Username: %s\n", username);
    printf("Total Execution Time: %ld seconds\n", (long) (t1-t0));
    printf("Total CPU Time: %f seconds\n", (double)clock()/CLOCKS_PER_SEC);
}

void processChild (int childNumber, int fromN, int toK, int maxN, int customSleep) {

	struct rusage rusage;
    int n, k, myPID = getpid();

    if (childNumber == 1) {
    	printf("\n(n (n-2)) binomial coefficient computations of integers n=2, 3, 10, start now!\n");
    }

    if (childNumber == 2 || childNumber == 3) {
	    sleep(customSleep);
	    for (n = fromN, k = toK; n <= maxN; n+= 2, k+=2) {
	        printf("Child %d (PID: %d) produced the binomial coefficient %2d for integer n = %2d and integer k = %d.\n", childNumber, myPID, bico(n,k), n, k);
	        sleep(3);
	    }
	    sleep(customSleep);
	}

	if (childNumber == 4) {
    	sleep(17);
        printf("\nListing directory segments in long format...\n");
        sleep(3);
    }

    if (getrusage(RUSAGE_SELF, &rusage) == -1) perror("rusage failed");
    time(t);
	int z; for (z = 0; z < 5000000; z++) { int x; x+=z; } //usertime loop
	if (getrusage(RUSAGE_SELF, &rusage) == -1) perror("rusage failed");
	*st = (double)rusage.ru_stime.tv_usec;
    *ut = (double)rusage.ru_utime.tv_usec;

    *rid = getuid();
    *eid = geteuid();
    *rgid = getgid();
    *egid = getegid();

    if (childNumber == 4) {
    	execlp("ls", "ls", "-l", (char *) NULL);
        perror("Execlp of 'ls' in child 4 has failed"); 
        exit(1);
    }

    exit(0);
}

int factorial(int n) {
    int result = 1, i;
    for (i = 1; i<=n; i++) {
        result = result*i;
    }

    return result;
}
int bico(int n, int k) {   
    return factorial(n)/(factorial(k) * factorial(n-k));
}

void childForkFailed(int childnumber) {
    printf("Child %d failed to fork! | ", childnumber);
    perror("");
    exit(1);
}
