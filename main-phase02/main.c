/*
 * ============================================
 * file: main.c
 * @Author Myron Tsatsarakis (myrontsa@csd.uoc.gr)
 * @Version 26/11/2024
 *
 * @e-mail hy240@csd.uoc.gr
 *
 * @brief Main function
 *        for CS240 Project Phase 2,
 *        Winter Semester 2024-2025
 * ============================================
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "voting.h"

/*** Parsing Utilities ***/

void TrimLine(char* line) {
    char* start = line;
    // Trim leading whitespace
    while (*start && isspace((unsigned char)*start)) start += 1;
    if (*start == '\0') {    // whitespace-only string case
        *line = '\0';
        return;
    }
    // Trim trailing whitespace
    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end -= 1;
    *(end + 1) = '\0';
    // Copy the trimmed content to the original buffer (memmove used for overlapping content)
    memmove(line, start, strlen(start));
}

bool IsEmptyLine(const char* line) {
    if (line == NULL || *line == '\0') return true;
    // check for non-whitespace characters
    for (const char* p = line; *p != '\0'; ++p)
        if (!isspace((unsigned char)*p)) return false;
    return true;
}

void ReportOutOfBounds(const char* line, unsigned lineIdx, unsigned arg, unsigned minLimit, unsigned maxLimit) {
    if ((minLimit <= arg) && (arg < maxLimit)) return;
    printf("On line %u: %s\n", lineIdx, line);
    printf("argument %u out of bounds [%u, %u)\n", arg, minLimit, maxLimit);
    exit(EXIT_FAILURE);
}

void ReportInvalidParse(const char* line, unsigned lineIdx, int actual, int expected) {
    if (actual == expected) return;
    printf("On line %u: %s\n", lineIdx, line);
    printf("sscanf argument assignment error: expected %d, assigned %d\n", expected, actual);
    exit(EXIT_FAILURE);
}

void EventParsedArgumentInitialization(const char* line, unsigned lineIdx) {
    const char* nameAddr = &(line[2]);
    char name[256];
    int charsRead;
    int count = sscanf(nameAddr, " %s%n", name, &charsRead);
    const char* valAddr = nameAddr + charsRead;
    ReportInvalidParse(line, lineIdx, count, 1);
    /*     if (strcmp(name, "Primes") == 0) {
            for (unsigned i = 0; i < PARSED_PRIMES_SZ; ++i) {
                unsigned num;
                int count = sscanf(valAddr, "%u", &num);
                ReportInvalidParse(line, lineIdx, count, 1);
                ParsedPrimes[i] = num;
            }
            DebugPrint("BB Primes ");
            for (unsigned i = 0; i < PARSED_PRIMES_SZ; ++i) DebugPrint("%u ", ParsedPrimes[i]);
            DebugPrint("\n");
            return;
        } */
    if (strcmp(name, "MaxStationsCount") == 0) {
        unsigned num;
        int count = sscanf(valAddr, "%u", &num);
        ReportInvalidParse(line, lineIdx, count, 1);
        ParsedMaxStationsCount = num;
        DebugPrint("BB MaxStationsCount %u\n", ParsedMaxStationsCount);
        return;
    }
    if (strcmp(name, "MaxSid") == 0) {
        unsigned num;
        int count = sscanf(valAddr, "%u", &num);
        ReportInvalidParse(line, lineIdx, count, 1);
        ParsedMaxSid = num;
        DebugPrint("BB MaxSid %u\n", ParsedMaxSid);
        return;
    }
    printf("On line %u: %s\n", lineIdx, line);
    printf("Invalid Parsed Argument (Primes, MaxStationsCount, MaxSid)\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }
    FILE* inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        printf("fopen error (Invalid path?)\n");
        exit(EXIT_FAILURE);
    }
#define LINE_SZ 4096
    char line[LINE_SZ] = { 0 };
    for (unsigned lineIdx = 1; fgets(line, LINE_SZ, inputFile) != NULL; ++lineIdx) {    // Read next line from input file
        line[strcspn(line, "\r\n")] = '\0';
        TrimLine(line);
        if (IsEmptyLine(line)) {
            continue;    // Skip empty lines
        }
        if (line[0] == '#') {
            continue;    // Skip comment lines
        }
        if (line[0] == 'A') {
            EventAnnounceElections();
            continue;
        }
        if (line[0] == 'D') {
            const char* args = &(line[1]);
            unsigned did, seats;
            int count = sscanf(args, "%u %u", &did, &seats);
            ReportInvalidParse(line, lineIdx, count, 2);
            ReportOutOfBounds(line, lineIdx, did, 0, DISTRICTS_SZ);
            EventCreateDistrict(did, seats);
            continue;
        }
        if (line[0] == 'S') {
            const char* args = &(line[1]);
            unsigned sid, did;
            int count = sscanf(args, "%u %u", &sid, &did);
            ReportInvalidParse(line, lineIdx, count, 2);
            ReportOutOfBounds(line, lineIdx, did, 0, DISTRICTS_SZ);
            EventCreateStation(sid, did);
            continue;
        }
        if (line[0] == 'C') {
            const char* args = &(line[1]);
            unsigned cid, pid, did;
            int count = sscanf(args, "%u %u %u", &cid, &pid, &did);
            ReportInvalidParse(line, lineIdx, count, 3);
            ReportOutOfBounds(line, lineIdx, pid, 0, PARTIES_SZ);
            ReportOutOfBounds(line, lineIdx, did, 0, DISTRICTS_SZ);
            EventRegisterCandidate(cid, pid, did);
            continue;
        }
        if (line[0] == 'R') {
            const char* args = &(line[1]);
            unsigned vid, sid;
            int count = sscanf(args, "%u %u", &vid, &sid);
            ReportInvalidParse(line, lineIdx, count, 2);
            EventRegisterVoter(vid, sid);
            continue;
        }
        if (line[0] == 'V') {
            const char* args = &(line[1]);
            unsigned vid, sid, cid, pid;
            int count = sscanf(args, "%u %u %u %u", &vid, &sid, &cid, &pid);
            ReportInvalidParse(line, lineIdx, count, 4);
            ReportOutOfBounds(line, lineIdx, pid, 0, PARTIES_SZ);
            EventVote(vid, sid, cid, pid);
            continue;
        }
        if (line[0] == 'M') {
            const char* args = &(line[1]);
            unsigned did;
            int count = sscanf(args, "%u", &did);
            ReportInvalidParse(line, lineIdx, count, 1);
            ReportOutOfBounds(line, lineIdx, did, 0, DISTRICTS_SZ);
            EventCountVotes(did);
            continue;
        }
        if (line[0] == 'G') {
            EventFormGovernment();
            continue;
        }
        if (line[0] == 'N') {
            EventFormParliament();
            continue;
        }
        if (line[0] == 'I') {
            const char* args = &(line[1]);
            unsigned did;
            int count = sscanf(args, "%u", &did);
            ReportInvalidParse(line, lineIdx, count, 1);
            ReportOutOfBounds(line, lineIdx, did, 0, DISTRICTS_SZ);
            EventPrintDistrict(did);
            continue;
        }
        if (line[0] == 'J') {
            const char* args = &(line[1]);
            unsigned sid;
            int count = sscanf(args, "%u", &sid);
            ReportInvalidParse(line, lineIdx, count, 1);
            EventPrintStation(sid);
            continue;
        }
        if (line[0] == 'K') {
            const char* args = &(line[1]);
            unsigned pid;
            int count = sscanf(args, "%u", &pid);
            ReportInvalidParse(line, lineIdx, count, 1);
            ReportOutOfBounds(line, lineIdx, pid, 0, PARTIES_SZ);
            EventPrintParty(pid);
            continue;
        }
        if (line[0] == 'L') {
            EventPrintParliament();
            continue;
        }
        if (line[0] == 'B' && line[1] == 'A') {
            EventBonusAnnounceElections();
            continue;
        }
        if (line[0] == 'B' && line[1] == 'U') {
            const char* args = &(line[2]);
            unsigned vid, sid;
            int count = sscanf(args, "%u %u", &vid, &sid);
            ReportInvalidParse(line, lineIdx, count, 2);
            EventBonusUnregisterVoter(vid, sid);
            continue;
        }
        if (line[0] == 'B' && line[1] == 'C') {
            const char* args = &(line[2]);
            unsigned cid, pid, did;
            int count = sscanf(args, "%u %u %u", &cid, &pid, &did);
            ReportInvalidParse(line, lineIdx, count, 3);
            ReportOutOfBounds(line, lineIdx, pid, 0, PARTIES_SZ);
            ReportOutOfBounds(line, lineIdx, did, 0, DISTRICTS_SZ);
            EventBonusRegisterCandidate(cid, pid, did);
            continue;
        }
        if (line[0] == 'B' && line[1] == 'W') {
            EventBonusSplitCandidates();
            continue;
        }
        if (line[0] == 'B' && line[1] == 'F') {
            EventBonusFreeMemory();
            continue;
        }
        if (line[0] == 'B' && line[1] == 'B') {
            EventParsedArgumentInitialization(line, lineIdx);
            continue;
        }
        printf("Invalid line %u: %s\n", lineIdx, line);
        exit(EXIT_FAILURE);
    }
    int err = fclose(inputFile);
    if (err != 0) {
        printf("fclose error\n");
        exit(EXIT_FAILURE);
    }
}
