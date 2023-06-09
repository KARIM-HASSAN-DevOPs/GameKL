#ifndef MCTS_H
#define MCTS_H


#include "state.h"


struct Node {
    bool expanded;
    unsigned int visits;
    float value;

    struct Node **children;
    uint_fast8_t childrenCount;

    uint8_t depth;
};


struct MCTSOptions {
    uint64_t iterations;
    float uctc;
    uint16_t maxSimDepth;
    bool multiaction;
    bool saveTree;
};


struct MCTSStats {
    uint64_t iterations;
    uint64_t nodes;
    uint64_t treeBytes;
    uint8_t treeDepth;
    uint32_t simulations;
    uint32_t depthOuts;
    uint64_t duration;
};


struct MCTSActionStats {
    uint64_t iterations;
    uint8_t actionCount;
    float value;
    uint64_t visits;
    uint64_t duration;
};


struct MCTSResults {
    struct Action actions[MAX_TURN_ACTIONS];
    uint8_t actionCount;

    struct MCTSStats stats;
    struct MCTSActionStats actionStats[MAX_TURN_ACTIONS];

    struct State state;

    struct Node *tree;
};


void MCTSOptions_default(struct MCTSOptions *options);


void mcts(const struct State *s, struct MCTSResults *r, const struct MCTSOptions *options);


#endif
