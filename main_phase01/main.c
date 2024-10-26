/*
 * ============================================
 * file: main.c
 * @Author John Malliotakis (jmal@csd.uoc.gr)
 * @Version 24/10/2023
 *
 * @e-mail hy240@csd.uoc.gr
 *
 * @brief Main function
 *        for CS240 Project Phase 1,
 *        Winter Semester 2024-2025
 * @see   Compile using supplied Makefile by running: make
 * ============================================
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "elections.h"

/* Maximum input line size */
#define MAX_LINE 1024

/* 
 * Uncomment the following line to
 * enable debugging prints
 * or comment to disable it
 */
/* #define DEBUG */
#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else
#define DPRINT(...)
#endif /* DEBUG */


void announce_elections(void)
{
	int i;
	/*initialize Districts*/
	for(i = 1; i <= 56; i++)
	{
		Districts[i].did = i;
		Districts[i].seats = 0;
		Districts[i].allotted = 0;
		Districts[i].blanks = 0;
		Districts[i].voids = 0;
		Districts[i].stations = NULL;
		Districts[i].candidates = NULL;
	}
	/*initialize parties*/
	for(i = 0; i < 5; i++)
	{
		Parties[i].pid = i;
		Parties[i].nelected = 0;
		Parties[i].elected = NULL;
	}
	/*initialize parliament*/
	Parliament.members = NULL;
}

int create_disticts(int did, int seats)
{
	/*inserts new district*/
	if(did < 1 || did > 56)
	{
		printf("FAILED\n");
		return 1;
	}
	Districts[did].seats = seats;

	/*prints the array*/
	int i;
	for(i = 0; i < did; i++)
	{
		printf("<%d>, ", did);
	}
	printf("\nDONE\n");
	return 0;
}





/*
 * Globals:
 * you may add some here for certain events
 * (such as D and P)
 */
struct district Districts[56];
struct party Parties[5];
struct parliament Parliament;

void destroy_structures(void)
{
	/*
	 * TODO: For a bonus
	 * empty all structures
	 * and free all memory associated
	 * with list nodes here
	 */
}

int main(int argc, char *argv[])
{
	FILE *event_file;
	char line_buffer[MAX_LINE];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	event_file = fopen(argv[1], "r");
	if (!event_file) {
		perror("fopen error for event file open");
		exit(EXIT_FAILURE);
	}

	while (fgets(line_buffer, MAX_LINE, event_file)) {
		char *trimmed_line;
		char event;
		int did, sid, vid, cid, pid, seats;
		int ret = 0;
		/*
		 * First trim any whitespace
		 * leading the line.
		 */
		trimmed_line = line_buffer;
		while (trimmed_line && isspace(*trimmed_line))
			trimmed_line++;
		if (!trimmed_line)
			continue;
		/* 
		 * Find the event,
		 * or comment starting with #
		 */
		if (sscanf(trimmed_line, "%c", &event) != 1) {
			fprintf(stderr, "Could not parse event type out of input line:\n\t%s",
					trimmed_line);
			fclose(event_file);
			exit(EXIT_FAILURE);
		}

		switch (event) {
			/* Comment, ignore this line */
			case '#':
				break;
			case 'A':
				printf("A\n");
				announce_elections();
				break;
			case 'D':
				if (sscanf(trimmed_line, "D %d %d", &did, &seats) != 2) {
					fprintf(stderr, "Event D parsing error\n");
					ret = 1;
					break;
				}
				printf("D %d %d\n", did, seats);
				ret = create_district(did, seats);
				break;
			case 'S':
				if (sscanf(trimmed_line, "S %d %d", &sid, &did) != 2) {
					fprintf(stderr, "Event S parsing error\n");
					ret = 1;
					break;
				}
				printf("S %d %d\n", sid, did);
				ret = create_station(sid, did);
				break;
			case 'P':
				if (sscanf(trimmed_line, "P %d", &pid) != 1) {
					fprintf(stderr, "Event P parsing error\n");
					ret = 1;
					break;
				}
				printf("P %d\n", pid);
				create_party(pid);
				break;
			case 'C':
				if (sscanf(trimmed_line, "C %d %d %d", &cid, &did, &pid) != 3) {
					fprintf(stderr, "Event C parsing error\n");
					ret = 1;
					break;
				}
				printf("C %d %d %d\n", cid, did, pid);
				ret = register_candidate(cid, did, pid);
				break;
			case 'R':
				if (sscanf(trimmed_line, "R %d %d %d", &vid, &did, &sid) != 3) {
					fprintf(stderr, "Event R parsing error\n");
					ret = 1;
					break;
				}
				printf("R %d %d %d\n", vid, did, sid);
				ret = register_voter(vid, did, sid);
				break;
			case 'U':
				if (sscanf(trimmed_line, "U %d", &vid) != 1) {
					fprintf(stderr, "Event U parsing error\n");
					ret = 1;
					break;
				}
				printf("U %d\n", vid);
				ret = unregister_voter(vid);
				break;
			case 'E':
				printf("E\n");
				delete_empty_stations();
				break;
			case 'V':
				if (sscanf(trimmed_line, "V %d %d %d", &vid, &sid, &cid) != 3) {
					fprintf(stderr, "Event V parsing error\n");
					ret = 1;
					break;
				}
				printf("V %d %d %d\n", vid, sid, cid);
				ret = vote(vid, sid, cid);
				break;
			case 'M':
				if (sscanf(trimmed_line, "M %d", &did) != 1) {
					fprintf(stderr, "Event M parsing error\n");
					ret = 1;
					break;
				}
				printf("M %d\n", did);
				count_votes(did);
			case 'G':
				printf("G\n");
				form_government();
				break;
			case 'N':
			printf("N\n");
				form_parliament();
				break;
			case 'I':
				if (sscanf(trimmed_line, "I %d", &did) != 1) {
					fprintf(stderr, "Event I parsing error\n");
					ret = 1;
					break;
				}
				printf("I %d\n", did);
				print_district(did);
			case 'J':
				if (sscanf(trimmed_line, "J %d %d", &sid, &did) != 2) {
					fprintf(stderr, "Event J parsing error\n");
					ret = 1;
					break;
				}
				printf("J %d %d\n", sid, did);
				print_station(sid, did);
				break;
			case 'K':
				if (sscanf(trimmed_line, "K %d", &pid) != 1) {
					fprintf(stderr, "Event I parsing error\n");
					ret = 1;
					break;
				}
				printf("K %d\n", pid);
				print_party(pid);
				break;
			case 'L':
				printf("L\n");
				print_parliament();
				break;
			default:
				fprintf(stderr, "WARNING: Unrecognized event %c. Continuing...\n",
						event);
				ret = 1;
				break;
		}
		fprintf(stderr, "Event %c %s.\n", event,
				ret ? "failed" : "succeeded");
	}
	fclose(event_file);
	destroy_structures();
	return 0;
}
