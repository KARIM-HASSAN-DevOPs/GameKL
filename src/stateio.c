#include "layout.h"
#include "state.h"

#include <stdio.h>
#include <string.h>


const char RESOURCE_CHARS[4] = "rybg";


void State_derive(struct State *state);


void Action_toString(const struct Action *action, char string[]) {
    switch (action->type) {
        case START_PLACE: {
            int node = action->data & 0b11111;
            int dir = action->data >> 6;
            int edge = CORNER_ADJACENT_EDGES[node][dir];

            sprintf(string, "s%02d%02d", node, edge);
            break;
        }

        case TRADE: {
            int in1 = (action->data >> 0)  & 0b11;
            int in2 = (action->data >> 2)  & 0b11;
            int in3 = (action->data >> 4)  & 0b11;
            int out = (action->data >> 6)  & 0b11;

            sprintf(string, "t%c%c%c%c",
                RESOURCE_CHARS[in1], RESOURCE_CHARS[in2],
                RESOURCE_CHARS[in3], RESOURCE_CHARS[out]);
            break;
        }

        case BRANCH: {
            sprintf(string, "b%02d", action->data);
            break;
        }

        case NODE: {
            sprintf(string, "n%02d", action->data);
            break;
        }

        case END: {
            string[0] = 'e';
            string[1] = '\0';
            break;
        }
    }
}


void Action_fromString(struct Action *action, const char string[]) {
    // Zero the memory, since the struct's members don't take up all
    // its space. This is not strictly necessary, but it's needed for
    // memcmp to work reliably on emscripten. I don't expect this
    // function to be called in performance-sensitive contexts, else
    // we could attach it to some test flag.
    memset(action, 0, sizeof(struct Action));
    switch (string[0]) {
        case 's': {
            int node;
            int edge;
            sscanf(string, "s%02d%02d", &node, &edge);

            int data = node & 0b11111;
            if (data > NUM_CORNERS) {
                data = 0;
            }
            for (enum Direction dir = 0; dir < 4; dir++) {
                if (CORNER_ADJACENT_EDGES[node][dir] == edge) {
                    data |= dir << 6;
                    break;
                }
            }

            action->type = START_PLACE;
            action->data = data;

            break;
        }

        case 't': {
            char in1c, in2c, in3c, outc;
            sscanf(string, "t%c%c%c%c", &in1c, &in2c, &in3c, &outc);

            int in1=0, in2=0, in3=0, out=0;
            for (int i = 0; i < NUM_RESOURCES; i++) {
                if (RESOURCE_CHARS[i] == in1c) in1 = i;
                if (RESOURCE_CHARS[i] == in2c) in2 = i;
                if (RESOURCE_CHARS[i] == in3c) in3 = i;
                if (RESOURCE_CHARS[i] == outc) out = i;
            }

            int data = 0;
            data |= in1 << 0;
            data |= in2 << 2;
            data |= in3 << 4;
            data |= out << 6;

            action->type = TRADE;
            action->data = data;

            break;
        }

        case 'b': {
            int data;
            sscanf(string, "b%02d", &data);

            action->type = BRANCH;
            action->data = data;
            if (action->data > NUM_EDGES) {
                action->data = 0;
            }

            break;
        }

        case 'n': {
            int data;
            sscanf(string, "n%02d", &data);

            action->type = NODE;
            action->data = data;
            if (action->data > NUM_CORNERS) {
                action->data = 0;
            }

            break;
        }

        case 'e': {
            action->type = END;
            action->data = 0;
        }
    }
}


bool validStateString(const char string[]) {
    int s = 0;
    for (int i = 0; i < NUM_SQUARES; i++) {
        switch(string[s++]) {
            case 'r':
            case 'y':
            case 'b':
            case 'g':
            case 'v':
                break;
            default:
                fprintf(stderr, "invalid square resource at %d\n", s - 1);
                return false;
        }
        switch(string[s++]) {
            case '0':
            case '1':
            case '2':
            case '3':
                break;
            default:
                fprintf(stderr, "invalid square limit at %d\n", s - 1);
                return false;
        }
    }

    for (int i = 0; i < NUM_CORNERS + NUM_EDGES; i++) {
        switch(string[s++]) {
            case '0':
            case '1':
            case '2':
                break;
            default:
                fprintf(stderr, "invalid node/corner at %d\n", s - 1);
                return false;
        }
    }

    // Turn
    switch(string[s++]) {
        case '1':
        case '2':
            break;
        default:
            fprintf(stderr, "invalid turn at %d\n", s - 1);
            return false;
    }

    // Resources
    for (int i = 0; i < NUM_PLAYERS * NUM_RESOURCES * 2; i++) {
        char c = string[s++];
        if (c < '0' || (c > '9' && (c < 'a' || c > 'f'))) {
            fprintf(stderr, "invalid resource amount at %d\n", s - 1);
            return false;
        }
    }

    // Trade status
    switch(string[s++]) {
        case '0':
        case '1':
            break;
        default:
            fprintf(stderr, "invalid trade status at %d\n", s - 1);
            return false;
    }

    return true;
}


void State_toString(const struct State *state, char string[]) {
    int s = 0;
    char c;

    // Squares
    for (int i = 0; i < NUM_SQUARES; i++) {
        if (state->squares[i].limit == 0) {
            string[s++] = 'v';
            string[s++] = '0';
            continue;
        }

        const char RESOURCE_CHARS[4] = "rybg";
        string[s++] = RESOURCE_CHARS[state->squares[i].resource];
        string[s++] = '0' + state->squares[i].limit;
    }

    // Nodes
    for (int i = 0; i < NUM_CORNERS; i++) {
        c = '0';
        if ((1llu << i) & state->nodes[PLAYER_1]) {
            c = '1';
        }
        if ((1llu << i) & state->nodes[PLAYER_2]) {
            if (c == '1') {
                c = '?';
            } else {
                c = '2';
            }
        }
        string[s++] = c;
    }

    // Branches
    for (int i = 0; i < NUM_EDGES; i++) {
        char c = '0';
        if ((1llu << i) & state->branches[PLAYER_1]) {
            c = '1';
        }
        if ((1llu << i) & state->branches[PLAYER_2]) {
            if (c == '1') {
                c = '?';
            } else {
                c = '2';
            }
        }
        string[s++] = c;
    }

    // Turn
    string[s++] = '1' + state->turn;

    // Resources
    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        int blue = state->resources[player][BLUE];
        if (blue > 0xff) blue = 0xff;
        int green = state->resources[player][GREEN];
        if (green > 0xff) green = 0xff;
        int red = state->resources[player][RED];
        if (red > 0xff) red = 0xff;
        int yellow = state->resources[player][YELLOW];
        if (yellow > 0xff) yellow = 0xff;

        s += sprintf(&string[s], "%02x%02x%02x%02x", blue, green, red, yellow);
    }

    // Trade status
    string[s++] += '0' + state->tradeDone;

    string[s++] = '\0';
}


void State_fromString(struct State *state, const char string[]) {
    State_randomStart(state);

    int s = 0;

    // Squares
    for (int i = 0; i < NUM_SQUARES; i++) {
        enum Resource resource;
        switch(string[s++]) {
            case 'r':
            case 'v':
                resource = RED;
                break;
            case 'y':
                resource = YELLOW;
                break;
            case 'b':
                resource = BLUE;
                break;
            case 'g':
                resource = GREEN;
                break;
            default:
                resource = RED;
        }
        int limit = string[s++] - '0';
        state->squares[i].resource = resource;
        state->squares[i].limit = limit;
        state->squares[i].remaining = limit;
        state->squares[i].captor = PLAYER_NONE;
    }

    // Nodes
    for (int i = 0; i < NUM_CORNERS; i++) {
        enum Player player = PLAYER_NONE;
        switch(string[s++]) {
            case '1':
                player = PLAYER_1;
                break;
            case '2':
                player = PLAYER_2;
                break;
            default: {}
        }

        if (player != PLAYER_NONE) {
            state->nodes[player] |= (1llu << i);
        }
    }

    // Branches
    for (int i = 0; i < NUM_EDGES; i++) {
        enum Player player = PLAYER_NONE;
        switch(string[s++]) {
            case '1':
                player = PLAYER_1;
                break;
            case '2':
                player = PLAYER_2;
                break;
            default: {}
        }

        if (player != PLAYER_NONE) {
            state->branches[player] |= (1llu << i);
        }
    }

    // Turn
    state->turn = string[s++] - '1';

    // Resources
    for (enum Player player = 0; player < NUM_PLAYERS; player++) {
        unsigned int blue, green, red, yellow;
        sscanf(&string[s], "%02x%02x%02x%02x",
            &blue, &green, &red, &yellow
        );
        s += 2*4;
        state->resources[player][BLUE] = blue;
        state->resources[player][GREEN] = green;
        state->resources[player][RED] = red;
        state->resources[player][YELLOW] = yellow;
    }

    // Trade status
    state->tradeDone = string[s] - '0';

    // Derive everythine else
    State_derive(state);
}


void State_print(const struct State *state) {
    char nc[NUM_CORNERS];
    for (int i = 0; i < NUM_CORNERS; i++) {
        char c = '.';
        if ((1llu << i) & state->nodes[PLAYER_1]) {
            c = 'X';
        }
        if ((1llu << i) & state->nodes[PLAYER_2]) {
            if (c == 'X') {
                c = '?';
            } else {
                c = 'O';
            }
        }
        nc[i] = c;
    }

    char ec[NUM_EDGES];
    for (int i = 0; i < NUM_EDGES; i++) {
        char c = ' ';
        if ((1llu << i) & state->branches[PLAYER_1]) {
            c = 'x';
        }
        if ((1llu << i) & state->branches[PLAYER_2]) {
            if (c == 'x') {
                c = '?';
            } else {
                c = 'o';
            }
        }
        ec[i] = c;
    }

    const char RESOURCE_CHARS[] = {'R', 'Y', 'B', 'G'};
    char rc[NUM_SQUARES];
    char lc[NUM_SQUARES];
    for (int i = 0; i < NUM_SQUARES; i++) {
        rc[i] = RESOURCE_CHARS[state->squares[i].resource];
        lc[i] = '0' + state->squares[i].limit;
        if (lc[i] == '0') {
            rc[i] = ' ';
            lc[i] = ' ';
        }
    }

    fprintf(stderr, "      %c%c%c%c\n", nc[0], ec[0], ec[0], nc[1]);
    fprintf(stderr, "      %c%c%c%c\n", ec[1], rc[0], lc[0], ec[2]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", nc[2], ec[3], ec[3], nc[3], ec[4], ec[4], nc[4], ec[5], ec[5], nc[5]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", ec[6], rc[1], lc[1], ec[7], rc[2], lc[2], ec[8], rc[3], lc[3], ec[9]);
    fprintf(stderr, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", nc[6], ec[10], ec[10], nc[7], ec[11], ec[11], nc[8], ec[12], ec[12], nc[9], ec[13], ec[13], nc[10], ec[14], ec[14], nc[11]);
    fprintf(stderr, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", ec[15], rc[4], lc[4], ec[16], rc[5], lc[5], ec[17], rc[6], lc[6], ec[18], rc[7], lc[7], ec[19], rc[8], lc[8], ec[20]);
    fprintf(stderr, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", nc[12], ec[21], ec[21], nc[13], ec[22], ec[22], nc[14], ec[23], ec[23], nc[15], ec[24], ec[24], nc[16], ec[25], ec[25], nc[17]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", ec[26], rc[9], lc[9], ec[27], rc[10], lc[10], ec[28], rc[11], lc[11], ec[29]);
    fprintf(stderr, "   %c%c%c%c%c%c%c%c%c%c\n", nc[18], ec[30], ec[30], nc[19], ec[31], ec[31], nc[20], ec[32], ec[32], nc[21]);
    fprintf(stderr, "      %c%c%c%c\n", ec[33], rc[12], lc[12], ec[34]);
    fprintf(stderr, "      %c%c%c%c\n", nc[22], ec[35], ec[35], nc[23]);
}


void State_printDetail(const struct State *state) {
	State_print(state);
	fprintf(stderr, "Turn:\t%d\n", state->turn);
    fprintf(stderr, "Action count: %d\t", state->actionCount);
    fprintf(stderr, "Trade done: %d\t", state->tradeDone);
    fprintf(stderr, "\n");
    fprintf(stderr, "Largest network size: %d\t", state->largestNetworkSize);
    fprintf(stderr, "Largest network player: %d\n", state->largestNetworkPlayer);

	for (enum Player player = 0; player < NUM_PLAYERS; player++) {
		fprintf(stderr, "Player %d:", player);
		fprintf(stderr, "\tScore:\t%d", state->score[player]);
		fprintf(stderr, "\tNode count:\t%d", popcount(state->nodes[player]));
        #ifdef __EMSCRIPTEN__
		fprintf(stderr, "\tCaptured:\t%x", state->captured[player]);
		fprintf(stderr, "\tBlocked:\t%llx\n", state->blocked[player]);
        #else
		fprintf(stderr, "\tCaptured:\t%lx", state->captured[player]);
		fprintf(stderr, "\tBlocked:\t%lx\n", state->blocked[player]);
        #endif

        fprintf(stderr, "\tResources: %dg %dy %db %dr",
            state->resources[player][GREEN],
            state->resources[player][YELLOW],
            state->resources[player][BLUE],
            state->resources[player][RED]);

		fprintf(stderr, "\n");
	}

    char stateString[STATE_STRING_SIZE];
    State_toString(state ,stateString);
    fprintf(stderr, "%s\n", stateString);
}
