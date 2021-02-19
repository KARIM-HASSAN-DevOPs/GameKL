#include "lookups.h"


const int SQUARE_ADJACENT_SQUARES[NUM_SQUARES][4] = {
    {-1,  2, -1, -1}, //  0
    {-1,  5,  2, -1}, //  1
    { 0,  6,  3,  1}, //  2
    {-1,  7,  2, -1}, //  3
    { 5, -1, -1, -1}, //  4
    { 1,  9,  6,  5}, //  5
    { 2, 10,  7,  5}, //  6
    { 3, 11,  8,  6}, //  7
    {-1, -1,  7, -1}, //  8
    { 5, -1, 10, -1}, //  9
    { 6, 12, 11,  9}, // 10
    { 7, -1, -1, 10}, // 11
    {10, -1, -1, -1}, // 12
};


const uint_fast64_t SQUARE_ADJACENT_BRANCHES[NUM_SQUARES][4] = {
    { 0,  4,  2,  1}, //  0
    { 3, 11,  7,  6}, //  1
    { 4, 12,  8,  7}, //  2
    { 5, 13,  9,  8}, //  3
    {10, 21, 16, 15}, //  4
    {11, 22, 17, 16}, //  5
    {12, 23, 18, 17}, //  6
    {13, 24, 19, 18}, //  7
    {14, 25, 20, 19}, //  8
    {22, 30, 27, 26}, //  9
    {23, 31, 28, 27}, // 10
    {24, 32, 29, 28}, // 11
    {31, 35, 34, 33}, // 12
};


const int EDGE_ADJACENT_SQUARES[NUM_EDGES][2] = {
     {0, -1}, //  0
     {0, -1}, //  1
     {0, -1}, //  2
     {1, -1}, //  3
      {0, 2}, //  4
     {3, -1}, //  5
     {1, -1}, //  6
      {1, 2}, //  7
      {2, 3}, //  8
     {3, -1}, //  9
     {4, -1}, // 10
      {1, 5}, // 11
      {2, 6}, // 12
      {3, 7}, // 13
     {8, -1}, // 14
     {4, -1}, // 15
      {4, 5}, // 16
      {5, 6}, // 17
      {6, 7}, // 18
      {7, 8}, // 19
     {8, -1}, // 20
     {4, -1}, // 21
      {5, 9}, // 22
     {6, 10}, // 23
     {7, 11}, // 24
     {8, -1}, // 25
     {9, -1}, // 26
     {9, 10}, // 27
    {10, 11}, // 28
    {11, -1}, // 29
     {9, -1}, // 30
    {10, 12}, // 31
    {11, -1}, // 32
    {12, -1}, // 33
    {12, -1}, // 34
    {12, -1}  // 35
};

const uint_fast64_t EDGE_ADJACENT_EDGES[NUM_EDGES] = {
	0b000000000000000000000000000000000110, //  0
	0b000000000000000000000000000000011001, //  1
	0b000000000000000000000000000100110001, //  2
	0b000000000000000000000000000011010010, //  3
	0b000000000000000000000000000010101110, //  4
	0b000000000000000000000000001100010100, //  5
	0b000000000000000000010000110000001000, //  6
	0b000000000000000000000001100000011010, //  7
	0b000000000000000000000011000000110100, //  8
	0b000000000000000010000110000000100000, //  9
	0b000000000000000000011000100001000000, // 10
	0b000000000000000000010001010011000000, // 11
	0b000000000000000000100010100110000000, // 12
	0b000000000000000001000101001100000000, // 13
	0b000000000000000110000010001000000000, // 14
	0b000000000000001000000000010000000000, // 15
	0b000000000000011000000000110001000000, // 16
	0b000000000000110000000001100010000000, // 17
	0b000000000001100000000011000100000000, // 18
	0b000000000011000000000110001000000000, // 19
	0b000000000010000000000100000000000000, // 20
	0b000000000100010000011000000000000000, // 21
	0b000000000100101000110000000000000000, // 22
	0b000000001001010000100000000000000000, // 23
	0b000000010010100011000000000000000000, // 24
	0b000000100001000110000000000000000000, // 25
	0b000001000000011000000000000000000000, // 26
	0b000011000000110000100000000000000000, // 27
	0b000110000001100001000000000000000000, // 28
	0b000100010011000000000000000000000000, // 29
	0b001010001100000000000000000000000000, // 30
	0b001101011000000000000000000000000000, // 31
	0b010010110000000000000000000000000000, // 32
	0b100011000000000000000000000000000000, // 33
	0b100110000000000000000000000000000000, // 34
	0b011000000000000000000000000000000000  // 35
};


const uint_fast32_t EDGE_ADJACENT_CORNERS[NUM_EDGES] = {
	0b000000000000000000000011, //  0
	0b000000000000000000001001, //  1
	0b000000000000000000010010, //  2
	0b000000000000000000001100, //  3
	0b000000000000000000011000, //  4
	0b000000000000000000110000, //  5
	0b000000000000000010000100, //  6
	0b000000000000000100001000, //  7
	0b000000000000001000010000, //  8
	0b000000000000010000100000, //  9
	0b000000000000000011000000, // 10
	0b000000000000000110000000, // 11
	0b000000000000001100000000, // 12
	0b000000000000011000000000, // 13
	0b000000000000110000000000, // 14
	0b000000000001000001000000, // 15
	0b000000000010000010000000, // 16
	0b000000000100000100000000, // 17
	0b000000001000001000000000, // 18
	0b000000010000010000000000, // 19
	0b000000100000100000000000, // 20
	0b000000000011000000000000, // 21
	0b000000000110000000000000, // 22
	0b000000001100000000000000, // 23
	0b000000011000000000000000, // 24
	0b000000110000000000000000, // 25
	0b000001000010000000000000, // 26
	0b000010000100000000000000, // 27
	0b000100001000000000000000, // 28
	0b001000010000000000000000, // 29
	0b000011000000000000000000, // 30
	0b000110000000000000000000, // 31
	0b001100000000000000000000, // 32
	0b010010000000000000000000, // 33
	0b100100000000000000000000, // 34
	0b110000000000000000000000  // 35
};


//const int SQUARE_NODES[NUM_SQUARES] = {
// Use bitboards
//}