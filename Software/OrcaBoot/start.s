// start.s

.section .text
.global _start

// Entry point
_start:
    // Set the stack pointer to 0x80000000
    mov sp, #0x80000000

    // Call the main function
    bl main

    // Exit
    b .
    