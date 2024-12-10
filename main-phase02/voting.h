#ifndef VOTING_H
#define VOTING_H

#include <stdbool.h>

#define DISTRICTS_SZ 56
#define PARTIES_SZ 5

typedef struct District District;
typedef struct Station Station;
typedef struct Voter Voter;
typedef struct Party Party;
typedef struct Candidate Candidate;
typedef struct ElectedCandidate ElectedCandidate;

struct District {
    int did;
    int seats;
    int blanks;
    int invalids;
    int partyVotes[PARTIES_SZ];
};

struct Station {
    int sid;
    int did;
    int registered;
    Voter* voters;
    Station* next;
};

struct Voter {
    int vid;
    bool voted;
    Voter* parent;
    Voter* lc;
    Voter* rc;
};

struct Party {
    int pid;
    int electedCount;
    Candidate* candidates;
};

struct Candidate {
    int cid;
    int did;
    int pid;
    int votes;
    bool isElected;
    Candidate* lc;
    Candidate* rc;
};

struct ElectedCandidate {
    int cid;
    int did;
    int pid;
    ElectedCandidate* next;
};

extern District Districts[DISTRICTS_SZ];
extern Station** StationsHT;
extern Party Parties[PARTIES_SZ];
extern ElectedCandidate* Parliament;

extern const int DefaultDid;
extern const int BlankDid;
extern const int InvalidDid;

extern int MaxStationsCount;
extern int MaxSid;
int findCandidateDistrict(Candidate* root, int cid);
Voter* insertVoter(Voter* root, Voter* newVoter);
void initializeElected(Candidate* root, Candidate** elected, int* count, int partyElected, int did);
void updateElected(Candidate* root, Candidate** elected, int partyElected, int did);
void printCandidates(Candidate* root);
void printVoters(Voter* root);
void VoteForCandidate(Candidate* root, int cid);
Voter* findVoter(Voter* root, int vid);
void unregisterVoter(Voter* root, int vid);

void EventAnnounceElections(int maxStationCount, int maxSid);
void EventCreateDistrict(int did, int seats);
void EventCreateStation(int sid, int did);
void EventRegisterCandidate(int cid, int pid, int did);
void EventRegisterVoter(int vid, int sid);
void EventVote(int vid, int sid, int cid, int pid);
void EventCountVotes(int did);
void EventFormGovernment();
void EventFormParliament();
void addElectedCandidatesToParliament(Candidate* root);
void EventPrintDistrict(int did);
void EventPrintStation(int sid);
void EventPrintParty(int pid);
void EventPrintParliament();
void EventBonusUnregisterVoter(int vid, int sid);
void EventBonusFreeMemory();

#endif 