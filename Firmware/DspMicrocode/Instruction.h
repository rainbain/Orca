#pragma once

#include <stdint.h>

/*
 * Design Notes:
 * All DSPs must finish before going to the next operation.
 * Accumulator Port A is shared by other DSPs.
 * If another DSP is reading it dont use it.
*/

//
// Part A
//

// Selects a input from the vector 0-4
#define PTA_VECTOR(addr)      ((addr << 12) | (0<<11))

// Selects the accumulator. Addressed as: A0 A1 A2 A3 B0 B1 B2 B3...
#define PTA_ACCUMULATOR(addr) ((addr << 12) | (1<<11))

//
// Part B
//

// Selects a input from the vector 0-4
#define PTB_VECTOR(addr) ((addr<<9) | (0 << 5))

// Select a input of the matrix
#define PTB_MATRIX(col, row) ((row<<9) | (col << 7) | (1 << 5))

// Only addresses local acculator data
#define PTB_ACCUMULATOR(addr) ((addr << 7) | (2 << 5))

// Operation

#define PT_OPERATION(op) (op<<2)

// Output
// Selects the DSPs own output accumulator byte, 0 1 2 or 3
#define PTU_ACCUMULATOR(addr) (addr << 0)

#define INSTRUCTION(a, b, op, out) (a | b | op | out)

// Operations

#define OP_FLOOR 0
#define OP_INT   1
#define OP_FLOAT 2
#define OP_SQRT  3
#define OP_MULT  4
#define OP_ADD   5
#define OP_SUB   6
#define OP_READY 7