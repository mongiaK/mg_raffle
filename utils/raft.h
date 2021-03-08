#ifndef __METHOD__
#define __METHOD__

#include <stdio.h>
#include <stdint.h>

enum RaftState {
    RS_LEADER = 1,
    RS_CANDIDATE = 2,
    RS_FOLLOWER = 3
};

static const uint32_t rs_heartbeat_timeout = 300;   //单位 ms
static const uint32_t rs_election_timeout = 150;    //单位 ms

struct Raft {
    void (*heartbeat_thread)();
};

#endif
