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
#include <math.h>

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
	for(i = 1; i <= 5; i++)
	{
		Parties[i].pid = i;
		Parties[i].nelected = 0;
		Parties[i].elected = NULL;
	}
	/*initialize parliament*/
	Parliament.members = NULL;
}

int create_district(int did, int seats)
{
	/*inserts new district*/
	if(did < 1 || did > 56)
	{
		printf("\nFAILED\n");
		return 1;
	}
	Districts[did].seats = seats;

	/*prints the array*/
	printf("\n\tDistricts = ");
	int i;
	for(i = 0; i < did; i++)
	{
		printf("<%d>, ", did);
	}
	printf("\nDONE\n");
	return 0;
}

int create_station(int sid, int did)
{
	/*inserts new station*/
	if (did < 1 || did > 56) {
        printf("\nFAILED\n");
        return 1;
    }
	struct station *new_station = (struct station *)malloc(sizeof(struct station));
	if(new_station == NULL)
	{
		printf("FAILED\n");
		return 1;
	}
	new_station->sid = sid;
	new_station->registered = 0;
	new_station->voters = NULL;
	new_station->vsentinel = NULL;
	new_station->next = NULL;
	if(Districts[did].stations == NULL)
	{
		Districts[did].stations = new_station;
	}
	else
	{
		struct station *current = Districts[did].stations;
		while(current != NULL)
		{
			if(current->next == NULL)
			{
				current->next = new_station;
				break;
			}
			current = current->next;
		}
	}
	/*prints stations*/
	printf("\n\tStations = ");
	struct station *current = Districts[did].stations;
	while(current != NULL)
	{
		printf("<%d>, ", current->sid);
		current = current->next;
	}
	printf("\nDONE\n");

}

void create_party(int pid){
	/*inserts new party*/
	if(pid < 1 || pid > 5)
	{
		printf("\nFAILED\n");
		return;
	}
	Parties[pid].pid = pid;
	Parties[pid].nelected = 0;
	Parties[pid].elected = NULL;

	/*prints parties*/
	printf("\n\tParties = ");
	int i;
	for(i = 0; i < pid; i++)
	{
		printf("<%d>, ", pid);
	}
	printf("\nDONE\n");
}

int register_candidate(int cid, int did, int pid){
	/*inserts new candidate*/
	if(did < 1 || did > 56 || pid < 1 || pid > 5 || cid < 2)
	{
		printf("\nFAILED\n");
		return 1;
	}
	
	struct candidate *new_candidate = (struct candidate *)malloc(sizeof(struct candidate));
	if(new_candidate == NULL)
	{
		printf("FAILED\n");
		return 1;
	}
	new_candidate->cid = cid;
	new_candidate->pid = pid;
	new_candidate->votes = 0;
	new_candidate->elected = 0;
	new_candidate->next = NULL;
	new_candidate->prev = NULL;
	if(Districts[did].candidates == NULL)
	{
		Districts[did].candidates = new_candidate;
	}
	else
	{
		struct candidate *current = Districts[did].candidates;
		while(current != NULL)
		{
			if(current->next == NULL)
			{
				current->next = new_candidate;
				new_candidate->prev = current;
				break;
			}
			current = current->next;
		}
	}
	/*prints candidates*/
	printf("\n\tCandidates = ");
	struct candidate *current = Districts[did].candidates;
	while(current != NULL)
	{
		printf("<%d>, ", current->cid);
		current = current->next;
	}
	printf("\nDONE\n");
	return 0;
}

int register_voter(int vid, int did, int sid)
{
	/*inserts new voter*/
	if(did < 1 || did > 56)
	{
		printf("\nFAILED\n");
		return 1;
	}
	struct voter *new_voter = (struct voter *)malloc(sizeof(struct voter));
	if(new_voter == NULL)
	{
		printf("FAILED\n");
		return 1;
	}
	new_voter->vid = vid;
	new_voter->voted = 0;
	new_voter->next = NULL;
	if(Districts[did].stations == NULL)
	{
		printf("FAILED\n");
		return 1;
	}
	struct station *current = Districts[did].stations;
	while(current != NULL)
	{
		if(current->sid == sid)
		{
			if(current->voters == NULL)
			{
				current->voters = new_voter;
			}
			else
			{
				struct voter *voter_current = current->voters;
				while(voter_current != NULL)
				{
					if(voter_current->next == NULL)
					{
						voter_current->next = new_voter;
						break;
					}
					voter_current = voter_current->next;
				}
			}
			break;
		}
		current = current->next;
	}

	/*prints voters*/
	printf("\n\tVoters = ");
	struct voter *voter_current = current->voters;
	while(voter_current != NULL)
	{
		printf("<%d>, ", voter_current->vid);
		voter_current = voter_current->next;
	}
	printf("\nDONE\n");
	return 0;
}

int unregister_voter(int vid)
{
	/*removes voter*/
	if(vid < 1)
	{
		printf("\nFAILED\n");
		return 1;
	}
	int i, esc = 0;
	struct station *current_station;
	for(i = 1; i <= 56; i++)
	{
		current_station = Districts[i].stations;
		while(current_station != NULL)
		{
			struct voter *current_voter = current_station->voters;
			struct voter *prev_voter = current_voter;
			while(current_voter != NULL)
			{
				if(current_voter->vid == vid)
				{
					if(current_voter->next == NULL)
					{
						prev_voter->next = NULL;
						free(current_voter);
						esc = 1;
						break;
					}
					else
					{
						prev_voter->next = current_voter->next;
						free(current_voter);	
						esc = 1;					
						break;
					}
				}
			}
			if(esc == 1)
			{
				break;
			}
			current_voter = current_voter;
			current_station = current_station->next;
		}
	}
	if(esc == 0)
	{
		printf("\nFAILED\n");
		return 1;
	}
	current_station->registered--;

	/*prints voters*/
	printf("\n\t<%d> <%d>", i, current_station->sid);
	printf("\n\tVoters = ");
	struct voter *voter_current = current_station->voters;
	while(voter_current != NULL)
	{
		printf("<%d>, ", voter_current->vid);
		voter_current = voter_current->next;
	}
	return 0;
}

void delete_empty_stations()
{
	/*removes empty stations*/
	int i, esc = 0;
	for(i = 1; i <= 56; i++)
	{
		struct station *current = Districts[i].stations;
		struct station *prev = current;
		while(current != NULL)
		{
			if(current->registered == 0)
			{
				if(current->next == NULL)
				{
					prev->next = NULL;
					free(current);
					esc = 1;
					break;
				}
				else
				{
					prev->next = current->next;
					free(current);
					esc = 1;
					break;
				}
			}
			
			prev = current;
			current = current->next;
		
		}
		if(esc == 1)
		{
			break;
		}
	}

	/*prints stations*/
	for(i = 1; i <= 56; i++)
	{
		struct station *current = Districts[i].stations;
		while(current != NULL)
		{
			printf("\n\t<%d> <%d>, ", current->sid, i);
			current = current->next;
		}
	}
	printf("\nDONE\n");
}

int vote(int vid, int sid, int cid)
{
	/*adds vote to voter*/
	if(vid < 0 || sid < 0 || cid < 0 )
	{
		printf("\nFAILED\n");
		return 1;
	}
	int i, esc = 0;
	struct station *current_station;
	for(i = 1; i <= 56; i++)
	{
		current_station = Districts[i].stations;
		while(current_station != NULL)
		{
			if(current_station->sid == sid)
			{
				struct voter *current_voter = current_station->voters;
				while(current_voter != NULL)
				{
					if(current_voter->vid == vid)
					{
						if(cid == 0 || cid == 1)
						{
							if(cid == 0)
							{
								Districts[i].blanks++;
							}
							else
							{
								Districts[i].voids++;
							}
							esc = 1;
							break;
						}
						else
						{
							vote_candidate(cid, i);
						}
						current_voter->voted = 1;
						esc = 1;
						break;
					}
					current_voter = current_voter->next;
				}
				break;
			}
			current_station = current_station->next;
		}
		if(esc == 1)
		{
			break;
		}
	}
	if(esc == 0)
	{
		printf("\nFAILED\n");
		return 1;
	}
	/*adds vote to candidate*/
	struct candidate *current_candidate = Districts[i].candidates;
	while(current_candidate != NULL)
	{
		if(current_candidate->cid == cid)
		{
			current_candidate->votes++;
			break;
		}
		current_candidate = current_candidate->next;
	}

	/*prints candidates*/
	printf("\n\tDistrict = <%d>", i);
	printf("\n\tCandidates = ");
	struct candidate *candidate_current = Districts[i].candidates;
	while(candidate_current != NULL)
	{
		printf("(<%d> <%d>), ", candidate_current->cid, candidate_current->votes);
		candidate_current = candidate_current->next;
	}
	printf("\n\tBlanks = <%d>", Districts[i].blanks);
	printf("\n\tVoids = <%d>", Districts[i].voids);
	printf("\nDONE\n");
	return 0;
}

void vote_candidate(int cid, int i)
{
	/*adds vote to candidate*/
	struct candidate *current_candidate = Districts[i].candidates;
	while(current_candidate != NULL)
	{
		if(current_candidate->cid == cid)
		{
			current_candidate->votes++;
			/*swaps candidates*/
			if(current_candidate->prev != NULL)
			{
				while (current_candidate->prev != NULL && current_candidate->votes > current_candidate->prev->votes) {
					struct candidate *temp = current_candidate->prev;
					current_candidate->prev = temp->prev;
					temp->next = current_candidate->next;
					if (temp->next != NULL) {
						temp->next->prev = temp;
					}
					current_candidate->next = temp;
					temp->prev = current_candidate;
					if (current_candidate->prev != NULL) {
						current_candidate->prev->next = current_candidate;
					} else {
						Districts[i].candidates = current_candidate;
					}
				}
			}
			break;
		}
		current_candidate = current_candidate->next;
	}
}

void count_votes(int did)
{
	/*counts votes*/
	if(did < 1 || did > 56)
	{
		printf("\nFAILED\n");
		return;
	}

	int votes_party[5] = {0};
	int seats_per_party[5] = {0};
	int magnitude[56] = {0};
	int total_votes = 0;

	struct candidate *current_candidate = Districts[did].candidates;
	/*counts total votes and votes per party*/
	while(current_candidate != NULL)
	{
		total_votes += current_candidate->votes;
		votes_party[current_candidate->pid] += current_candidate->votes;
		current_candidate = current_candidate->next;
	}

	int valid_votes = total_votes - (Districts[did].voids + Districts[did].blanks);
	int i;
	/*counts the seats per party*/
	for(i = 1; i <= 5; i++)
	{
		seats_per_party[i] = (int)floor((double)votes_party[i] / magnitude[did]);
	}
	
	current_candidate = Districts[did].candidates;
	/*distributes seats to candidates*/
	for(i = 1; i <= 5; i++)
	{
		int j;
		for(j = 0; j < seats_per_party[i]; j++)
		{
			if(current_candidate == NULL)
			{
				break;
			}
			current_candidate->elected = 1;
			Parties[i].nelected++;
			current_candidate = current_candidate->next;
			/* Add the elected candidate to the party*/
			struct candidate *elected_candidate = (struct candidate *)malloc(sizeof(struct candidate));
			if (elected_candidate == NULL) {
				printf("FAILED\n");
				return;
			}
			*elected_candidate = *current_candidate;
			elected_candidate->next = Parties[i].elected;
			Parties[i].elected = elected_candidate;
		}
	}
	/*prints candidates*/
	printf("/n/tSeats = ");
	for(i = 1; i <= 5; i++)
	{
		struct candidate *current_candidate = Parties[i].elected;
		while(current_candidate != NULL)
		{
			printf("(<%d> <%d> <%d>), ", current_candidate->cid, i, current_candidate->votes);
			current_candidate = current_candidate->next;
		}
	}
	
	printf("\nDONE\n");
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
