#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define TABSIZE 6543
#define LINESIZE 10 /* entries per line */
#define GROUPSIZE 5 /* lines per group */
#define PAGESIZE 10 /* groups per page */

static void signal_handler(int);
struct sigaction sigact;

long int primes = 0, pairs = 0;
int lpos = 1, gpos = 1, pgrp = 1, pgno = 1;
unsigned int runtime = 10;

char copyright[] = "prime Copyright 1990, 1998 - Thomas D. Dial";

unsigned long int exlst();
void line(unsigned long int j, char star);
void title();

unsigned char list[TABSIZE];
int llist = 0;              /* number of entries valid in list */
unsigned long limit = 0;    /* integer square root of number being tested */
unsigned long j = 0;        /* current candidate */
unsigned d = 0;             /* current divisor */
unsigned rem = 0;           /* remainder on division */
unsigned long last = 0;     /* last prime discovered */
unsigned long startp = 0;   /* last prime before start of output */
unsigned long spairs = 0;   /* pair count supplied at startup */
unsigned long sprimes = 0;  /* prime count supplied at startup */
int lower = 1;              /* flags for testing */
int upper = 1;
char * end;		    /* end of strtoul scan */
int base;		    /* base for input to strtoul */

void setlimit(unsigned long int j)
/*
	figure approximate integer square root of j
*/
{
 while (limit * limit < j + 1)
	limit++;
}

unsigned long int exlst()
/*
	Maintain the list of differences
*/

{
 if (lower) {
	if (llist < (TABSIZE - 1))
		list[++llist] = j - last - 1;
	last = j - 1;
	}

 if (upper) {
	if (llist < TABSIZE) {
		list[++llist] = j - last + 1;
		if (llist == TABSIZE) {
			if (startp > 0) {
				j = (startp > j) ? ((startp+1)/6)*6 : j;
				pairs = spairs;
				primes = sprimes;
				}
			}
	}
	last = j + 1;
	}

}

void printprime(unsigned long int j)
/*
	Format and display the new prime(s).
*/
{
 char star;

 if (upper && lower) {
	pairs++;
	star = '*';
	}
 else
	star = ' ';

 if (lower) {
	primes++;
	line(j - 1, star);
	}

 if (upper) {
	primes++;
	line(j + 1, star);
	}
}

void line(unsigned long int j, char star)
/*
	Take care of print formatting.
*/
{
 if (j > startp) {
	printf("% 10lu%c", j, star);
	if (++lpos > LINESIZE) {
		printf("\n");
		lpos = 1;
		if (++gpos > GROUPSIZE) {
			printf("\n");
			gpos = 1;
			if (++pgrp > PAGESIZE) {
				pgrp = 1;
				printf("  Pairs - %ld (%-5.2f%%); total primes - %ld (%-5.2f%%).\n",
				  pairs, (100.0 * pairs)/primes, primes, (100.0 * primes)/j);
				title();
				}
			}
		}
	}
}

void title()
{
	printf("\fTable of Prime Integers - Page %d\n\n", pgno++);
}


void findp() {
 int i;
 j = alarm(runtime);
 for (j=6;;j+=6) {

 /*
    to limit output, supply a limiting condition (e.g., pgno < 100)
    as the "while" condition above.
 */

	lower = upper = 1;
	setlimit(j);
	d = list[0] + list[1];
	for (i=2; (d+=list[i]) <= limit; i++) {
		rem = j % d;

		if (rem == 1) {
			lower = 0;
			if (!upper)
				break;
			}
		if (rem == d - 1) {
			upper = 0;
			if (!lower)
				break;
			}
		}
	if (lower || upper) {
		printprime(j);
		exlst();
		}
	}
}

static void signal_handler(int sig)
{
 fprintf(stderr, "Exiting after %d seconds at alarm signal.\n", runtime);
 exit(0);
}

int main(int argc,char **argv)
{
 int arg;

 primes = 2;
 for (arg = 1; arg < argc; arg++) {
	if (argv[arg][0] != '-')
		arg++;
	else
		switch (argv[arg++][1]) {
			case 'p':
				pgno = atoi(argv[arg]);
				fprintf(stderr, "starting on page number %d\n", pgno);
				break;
			case 'd':
				spairs = atol(argv[arg]);
				fprintf(stderr, "starting count %ld pairs\n", spairs);
				break;
			case 't':
				sprimes = atol(argv[arg]);
				fprintf(stderr, "starting count %ld primes\n", sprimes);
				break;
			case 'l':
				startp = strtoul(argv[arg], &end, base);
				fprintf(stderr, "starting after %lu\n", startp);
				break;
			default:
				fprintf(stderr, "unknown flag \"%s\"\n", argv[arg]);
			}
	}

 sigact.sa_handler = signal_handler;
 sigemptyset(&sigact.sa_mask);
 sigact.sa_flags = 0;
 sigaction(SIGALRM, &sigact, (struct sigaction *)NULL);

 list[0] = 2;
 list[1] = 1;
 list[2] = 2;
 llist = 1;

 last = 3;
 title();
 if (startp == 0) { line(2,' '); line(3,' '); }
 findp();

 return 0;
}
