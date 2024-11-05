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
#define DEBUG 
#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else
#define DPRINT(...)
#endif /* DEBUG */

void vote_candidate(int cid, int district_id);
struct candidate **sort_array_decreasing(struct candidate *array[], int size);
void merge_array_to_list(struct candidate *array[], int array_size, struct candidate *list, int max_party);
/*
 * Globals:
 * you may add some here for certain events
 * (such as D and P)
 */
struct district Districts[57];
struct party Parties[5];
struct parliament Parliament;


void announce_elections(void)
{
	int i;
	/*initialize Districts*/
	for(i = 0; i <= 56; i++)
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
	for(i = 1; i <= did; i++)
	{
		printf("<%d>, ", i);
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
	return 0;

}

void create_party(int pid){
	/*inserts new party*/
	if(pid < 0 || pid > 5)
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
		printf("<%d>, ", i);
	}
	printf("\nDONE\n");
}

int register_candidate(int cid, int did, int pid){
	/*inserts new candidate*/
	if(did < 1 || did > 56 || pid < 0 || pid > 5 || cid < 2)
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

	current->registered++;

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
	for (i = 0; i < 56; i++) {
        current_station = Districts[i].stations;
        while (current_station != NULL) {
            struct voter *current_voter = current_station->voters;
            struct voter *prev_voter = NULL;

            while (current_voter != NULL) {
                if (current_voter->vid == vid) {
                    if (prev_voter == NULL) {
                        current_station->voters = current_voter->next;
                    } else {
                        prev_voter->next = current_voter->next;
                    }
                    free(current_voter);
                    esc = 1;
                    break;
                }
                prev_voter = current_voter;
                current_voter = current_voter->next;
            }
            if (esc) break;
            current_station = current_station->next;
        }
        if (esc) break;
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
	printf("\nDONE\n");
	return 0;
}

void delete_empty_stations()
{
	/*removes empty stations*/
	int i, esc = 0;
	 for (i = 1; i <= 56; i++) { 
        struct station *current = Districts[i].stations;
        struct station *prev = NULL;

        while (current != NULL) {
            if (current->registered == 0) { 
				/*prints stations*/
                printf("\n\t<%d> <%d>", current->sid, i); 
                if (prev == NULL) {
                    Districts[i].stations = current->next;
                } else {
                    prev->next = current->next;
                }
                struct station *temp = current;
                current = current->next;
                free(temp);
            } else {
                prev = current;
                current = current->next;
            }
        }
    }

	
	printf("\nDONE\n");
}

int vote(int vid, int sid, int cid)
{
	/*adds vote to voter*/
	if(vid < 0 || sid < 0 || cid < 0 )
	{
		printf("\nFAILEDD\n");
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

	int votes_party[5][57] = {{0}};
	int seats_per_party[5][57] = {{0}};
	int magnitude[57] = {0};
	int total_votes = 0;

	struct candidate *current_candidate = Districts[did].candidates;
	/*counts total votes and votes per party*/
	while(current_candidate != NULL)
	{
		total_votes += current_candidate->votes;
		votes_party[current_candidate->pid][did] += current_candidate->votes;
		current_candidate = current_candidate->next;
	}

	int valid_votes = total_votes - (Districts[did].voids + Districts[did].blanks);
	valid_votes = valid_votes <= 0 ? 0 : valid_votes;
	magnitude[did] = valid_votes / Districts[did].seats;
	int i;

	/*counts the seats per party*/
	for(i = 0; i < 5; i++)
	{
		if(magnitude[did] > 0)
		{
			seats_per_party[i][did] = votes_party[i][did] / magnitude[did];
		}
		else
		{
			seats_per_party[i][did] = 0;
		}
	}
	
	/*distributes seats to candidates*/
	printf("\n\tSeats = ");

	for(i = 0; i < 5; i++)
	{	
		current_candidate = Districts[did].candidates;
		if(current_candidate == NULL)
		{
			printf("\n\tFAILED");
		}
		int j ;
		int prev = 0;
		for(j = 0; j < seats_per_party[i][did]; j++)
		{
			while(current_candidate != NULL){
				
				if(current_candidate->pid == i && current_candidate->elected != 1)
				{
					current_candidate->elected = 1;
					break;
				}
				current_candidate = current_candidate->next;
			}
			if(current_candidate != NULL)
			{
				
				/* Add the elected candidate to the party*/
				struct candidate *elected_candidate = (struct candidate *)malloc(sizeof(struct candidate));
				if (elected_candidate == NULL) {
					printf("FAILED\n");
					return;
				}
				
				elected_candidate->cid = current_candidate->cid;
				elected_candidate->pid = current_candidate->pid;
				elected_candidate->votes = current_candidate->votes;
				elected_candidate->elected = 1;

				elected_candidate->next = NULL;
				if(Parties[i].elected == NULL)
				{
					Parties[i].elected = elected_candidate;
				}
				else
				{
					struct candidate *current_elected_candidate = Parties[i].elected;
					while(current_elected_candidate != NULL)
					{
						if(current_elected_candidate->next == NULL)
						{
							current_elected_candidate->next = elected_candidate;
							break;
						}
						current_elected_candidate = current_elected_candidate->next;
					}
				}
				Parties[i].nelected++;
				Districts[did].allotted++;
				
				/*prints candidates*/
				printf("\n\t\t<%d><%d><%d>", elected_candidate->cid, i, elected_candidate->votes);
			}
		}
	}
		
	printf("\nDONE\n");
}

void form_government()
{
	/*forms government*/
	int i;
	int max_seats = 0;
	int max_party = 0;
	for(i = 0; i < 5; i++)
	{
		if(Parties[i].nelected > max_seats)
		{
			max_seats = Parties[i].nelected;
			max_party = i;
		}
	}
	printf("\n\tParty = <%d>", max_party);
	int remaining_seats;
	/*add the candidates to the remaining seats to the party */
	for(i = 1; i <= 56; i++)
	{
		remaining_seats = Districts[i].seats - Districts[i].allotted;

		struct candidate *current_candidate = Districts[i].candidates;
		while(remaining_seats > 0 && current_candidate != NULL)
		{
			if(current_candidate->pid == max_party)
			{	
				if(!current_candidate->elected){
					current_candidate->elected = 1;
					Parties[max_party].nelected++;
					Districts[i].allotted++;
					remaining_seats--;
					/* Add candidate to the party*/
					struct candidate *elected_candidate = (struct candidate *)malloc(sizeof(struct candidate));
					if (elected_candidate == NULL) {
						printf("FAILED\n");
						return;
					}
					current_candidate->elected = 1;
					elected_candidate->cid = current_candidate->cid;
					elected_candidate->pid = current_candidate->pid;
					elected_candidate->votes = current_candidate->votes;
					elected_candidate->elected = 1;
					elected_candidate->next = NULL;

					struct candidate *current_elected_candidate = Parties[max_party].elected;

					while(current_elected_candidate->next != NULL)
					{
						current_elected_candidate = current_elected_candidate->next;
					}
					current_elected_candidate->next = elected_candidate;
				}
			}
			current_candidate = current_candidate->next;
		}
	}
	/*find the most voted and non elected candidate/s if needed*/
	int j = 0;
	struct candidate *top_voted_non_elected[remaining_seats];
	if(remaining_seats)
	{
		for(i = 1; i <= 56; i++)
		{
			struct candidate *current_candidate = Districts[i].candidates;
			while(current_candidate != NULL)
			{
				if(j < remaining_seats){
					if(current_candidate->elected != 1)
					{
						top_voted_non_elected[j]->cid = current_candidate->cid;
						top_voted_non_elected[j]->pid = current_candidate->pid;
						top_voted_non_elected[j]->votes = current_candidate->votes;
						top_voted_non_elected[j]->next = NULL;
						top_voted_non_elected[j]->elected = 1;
						current_candidate->elected = 1;
						j++;
					}
				}
				else if(j == remaining_seats)
				{
					struct candidate **sorted_array = sort_array_decreasing(top_voted_non_elected, remaining_seats);
					for (int k = 0; k < remaining_seats; k++) {
						top_voted_non_elected[k] = sorted_array[k];
					}
					struct candidate *last_element = top_voted_non_elected[remaining_seats - 1];
					int k = remaining_seats - 2;

					while (k >= 0 && top_voted_non_elected[k]->votes < last_element->votes) {
						top_voted_non_elected[k + 1]->elected = 0;
						top_voted_non_elected[k + 1] = top_voted_non_elected[k];
						top_voted_non_elected[k]->elected = 1;
						k--;
					}
					top_voted_non_elected[k + 1] = last_element;
				}
				current_candidate = current_candidate->next;
			}
		}
		struct candidate *candidate_elected_array = Parties[max_party].elected;
		merge_array_to_list(top_voted_non_elected, remaining_seats, candidate_elected_array, max_party);
	}
	/*prints candidates*/
	printf("\n\tSeats = ");
	for(i = 1; i <= 56; i++){
		struct candidate *current_candidate = Districts[i].candidates;
		while(current_candidate != NULL){
			struct candidate *current_party_candidate = Parties[max_party].elected;
				while(current_party_candidate != NULL){
				if(current_candidate->cid == current_party_candidate->cid){
					printf("\n\t\t<%d> <%d> <%d> <%d>\n", i, current_candidate->cid , current_candidate->votes, current_candidate->pid);
				}
				current_party_candidate = current_party_candidate->next;
			}
			current_candidate = current_candidate->next;
		}
	}
	printf("\nDONE\n");
}

void merge_array_to_list(struct candidate *array[], int array_size, struct candidate *list, int max_party) {
    struct candidate *merged_head = NULL;
    struct candidate **last_ptr = &merged_head;

    int i = 0;
    while (i < array_size && list != NULL) {
        if (array[i]->votes >= list->votes) {
            *last_ptr = array[i];
            array[i] = array[i]->next;
            i++;
        } else {
            *last_ptr = list;
            list = list->next;
        }
        last_ptr = &(*last_ptr)->next;
    }

    while (i < array_size) {
        *last_ptr = array[i];
        array[i] = array[i]->next;
        last_ptr = &(*last_ptr)->next;
        i++;
    }

    while (list != NULL) {
        *last_ptr = list;
        list = list->next;
        last_ptr = &(*last_ptr)->next;
    }

    *last_ptr = NULL;
    Parties[max_party].elected = merged_head; 
}

struct candidate **sort_array_decreasing(struct candidate *array[], int size)
{
	/*sorts array*/
	int i, j;
	for(i = 0; i < size; i++)
	{
		for(j = i + 1; j < size; j++)
		{
			if(array[i]->votes < array[j]->votes)
			{
				struct candidate *temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}
	return array;
}

void form_parliament()
{
	/*forms parliament*/
	struct candidate *current_candidate = NULL;
	int i;
	for(i = 0; i <5; i++)
	{
		current_candidate = Parties[i].elected;
		while(current_candidate != NULL)
		{
			/* Add candidate to the parliament */
			struct candidate *elected_candidate = (struct candidate *)malloc(sizeof(struct candidate));
			if (elected_candidate == NULL) {
				printf("FAILED\n");
				return;
			}
			elected_candidate->cid = current_candidate->cid;
			elected_candidate->pid = current_candidate->pid;
			elected_candidate->votes = current_candidate->votes;
			elected_candidate->elected = 1;
			elected_candidate->next = NULL;

			if (Parliament.members == NULL) {
				Parliament.members = elected_candidate;
			} else {
				struct candidate *current_elected_candidate = Parliament.members;
				while (current_elected_candidate->next != NULL) {
					current_elected_candidate = current_elected_candidate->next;
				}
				current_elected_candidate->next = elected_candidate;
			}
			current_candidate = current_candidate->next;
		}
	}
	/*prints candidates*/
	printf("\n\tMembers = ");
	struct candidate *current = Parliament.members;
	while(current != NULL)
	{
		printf("\n\t\t<%d> <%d> <%d>\n", current->cid, current->pid, current->votes);
		current = current->next;
	}
	printf("\nDONE\n");
}

void print_district(int did)
{
	/*prints district*/
	if(did < 1 || did > 56)
	{
		printf("\nFAILED\n");
		return;
	}
	printf("\n\tSeats = <%d>", Districts[did].seats);
	printf("\n\tBlanks = <%d>", Districts[did].blanks);
	printf("\n\tVoids = <%d>", Districts[did].voids);
	struct candidate *current_candidate = Districts[did].candidates;
	printf("\n\tCandidates = ");
	while (current_candidate != NULL)
	{
		printf("\n\t\t<%d> <%d> <%d>\n", current_candidate->cid, current_candidate->pid, current_candidate->votes);
		current_candidate = current_candidate->next;
	}
	printf("\n\tStations = ");
	struct station *current_station = Districts[did].stations;
	while(current_station != NULL)
	{
		printf("<%d>, ", current_station->sid);
		current_station = current_station->next;
	}
	
	printf("\nDONE\n");
}

void print_station(int sid, int did)
{
	/*prints station*/
	if(did < 1 || did > 56)
	{
		printf("\nFAILED\n");
		return;
	}
	struct station *current_station = Districts[did].stations;
	while(current_station != NULL)
	{
		if(current_station->sid == sid)
		{
			printf("\n\tRegistered = <%d>", current_station->registered);
			printf("\n\tVoters = ");
			struct voter *current_voter = current_station->voters;
			while(current_voter != NULL)
			{
				printf("\n\t\t<%d> <%d>\n", current_voter->vid, current_voter->voted);
				current_voter = current_voter->next;
			}
			break;
		}
		current_station = current_station->next;
	}
	printf("\nDONE\n");
}

void print_party(int pid)
{
	/*prints party*/
	if(pid < 0 || pid > 5)
	{
		printf("\nFAILED\n");
		return;
	}
	printf("\n\tElected = ");
	struct candidate *current_candidate = Parties[pid].elected;
	while(current_candidate != NULL)
	{
		if(current_candidate->elected == 1)
		{
			printf("\n\t\t<%d> <%d>\n", current_candidate->cid, current_candidate->votes);
		}
		current_candidate = current_candidate->next;
	}
	printf("\nDONE\n");
}

void print_parliament()
{
	/*prints parliament*/
	printf("\n\tMembers = ");
	struct candidate *current = Parliament.members;
	while(current != NULL)
	{
		printf("\n\t\t<%d> <%d> <%d>\n", current->cid, current->pid, current->votes);
		current = current->next;
	}
	printf("\nDONE\n");
}



void destroy_structures(void)
{
	/*
	 * TODO: For a bonus
	 * empty all structures
	 * and free all memory associated
	 * with list nodes here
	 */

	/* Free Districts */
	int i;
	for(i = 0; i <= 56; i++)
	{
		struct station *current_station = Districts[i].stations;
		while(current_station != NULL)
		{
			struct voter *current_voter = current_station->voters;
			while(current_voter != NULL)
			{
				struct voter *temp = current_voter;
				current_voter = current_voter->next;
				free(temp);
			}
			struct station *temp = current_station;
			free(temp);

			current_station = current_station->next;

		}
		struct candidate *current_candidate = Districts[i].candidates;
		while(current_candidate != NULL)
		{
			struct candidate *temp = current_candidate;
			current_candidate = current_candidate->next;
			free(temp);
		}
	}
	/*free Parties*/
	for(i = 0; i < 5; i++)
	{
		struct candidate *current_candidate = Parties[i].elected;
		while(current_candidate != NULL)
		{
			struct candidate *temp = current_candidate;
			current_candidate = current_candidate->next;
			free(temp);
		}
	}

	/*free Parliament*/
	struct candidate *current_member = Parliament.members;
	while(current_member != NULL)
	{
		struct candidate *temp = current_member;
		free(temp);
		current_member = current_member->next;

	}
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
				break;
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
				break;
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
