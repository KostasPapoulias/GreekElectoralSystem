/*
 * ============================================
 * file: voting.h
 * @Author Myron Tsatsarakis (myrontsa@csd.uoc.gr)
 * @Version 26/11/2024
 *
 * @e-mail hy240@csd.uoc.gr
 *
 * @brief Structure and function declarations
 *        for CS240 Project Phase 2,
 *        Winter Semester 2024-2025
 * ============================================
 */

#ifndef VOTING_H
#define VOTING_H

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

/*** Districts Array  ***/

#define PARTIES_SZ 5
typedef struct District District;
struct District {
    int did;
    int seats;
    int blanks;
    int invalids;
    int partyVotes[PARTIES_SZ];
};
#define DISTRICTS_SZ 56
typedef struct DistrictsArray DistrictsArray;
struct DistrictsArray {
    District data[DISTRICTS_SZ];
};

/*** Voters Tree, Stations Hashtable  ***/

typedef struct Voter Voter;
struct Voter {
    int vid;
    bool voted;
    Voter* parent;
    Voter* lc;
    Voter* rc;
};
typedef struct VotersTree VotersTree;
struct VotersTree {
    Voter* root;
};
typedef struct Station Station;
struct Station {
    int sid;
    int did;
    int registered;
    VotersTree voters;
    Station* next;
};
typedef struct StationsList StationsList;
struct StationsList {
    Station* begin;
};
typedef struct StationsHashTable StationsHashTable;
struct StationsHashTable {
    StationsList* data;
    int capacity;
};
extern int ParsedMaxStationsCount;
extern int ParsedMaxSid;

/*** Candidates Binary Search Tree, Parties Array  ***/

typedef struct Candidate Candidate;
struct Candidate {
    int cid;
    int pid;
    int did;
    int votes;
    bool isElected;
    Candidate* lc;
    Candidate* rc;
};
typedef struct CandidatesBST CandidatesBST;
struct CandidatesBST {
    Candidate* root;
};
typedef struct Party Party;
struct Party {
    int pid;
    int electedCount;
    CandidatesBST candidates;
};
typedef struct PartiesArray PartiesArray;
struct PartiesArray {
    Party data[PARTIES_SZ];
};

/*** Parliament List (sorted descending)  ***/

typedef struct ElectedCandidate ElectedCandidate;
struct ElectedCandidate {
    int cid;
    int pid;
    int did;
    ElectedCandidate* next;
};
typedef struct ParliamentList ParliamentList;
struct ParliamentList {
    ElectedCandidate* begin;
};

/*** Bonus Candidates Binary Search Tree with Sentinel  ***/

typedef struct BonusCandidatesBST BonusCandidatesBST;
struct BonusCandidatesBST {
    Candidate* root;
    Candidate* sentinel;
};

// Enable in Makefile
#ifdef DEBUG_PRINTS_ENABLED
#define DebugPrint(...) printf(__VA_ARGS__);
#else
#define DebugPrint(...)
#endif

void EventAnnounceElections();
void EventCreateDistrict(int did, int seats);
void EventCreateStation(int sid, int did);
void EventRegisterCandidate(int cid, int did, int pid);
void EventRegisterVoter(int vid, int sid);
void EventVote(int vid, int sid, int cid, int pid);
void EventCountVotes(int did);
void EventFormGovernment();
void EventFormParliament();
void EventPrintDistrict(int did);
void EventPrintStation(int sid);
void EventPrintParty(int pid);
void EventPrintParliament();
void EventBonusAnnounceElections();
void EventBonusUnregisterVoter(int vid, int sid);
void EventBonusRegisterCandidate(int cid, int did, int pid);
void EventBonusSplitCandidates();
void EventBonusFreeMemory();

#endif
