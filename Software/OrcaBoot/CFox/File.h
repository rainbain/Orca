// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CFox
 * rainbain
 * 8/15/2023
 * Orca Emulator
 *
 * A simple File type. Also adds a simple memory file.
*/

#pragma once

typedef struct CFile CFile;

typedef void (*FILE_SEEK)(CFile *file, size_t address);
typedef size_t (*FILE_READ)(CFile *file, void *data, size_t len);
typedef size_t (*FILE_WRITE)(CFile * file, void *data, size_t len);
typedef bool (*FILE_EOF)(CFile * file);

typedef struct CFile {
    void *super;
    FILE_SEEK seek;
    FILE_READ read;
    FILE_WRITE write;
    FILE_EOF eof;
} CFile;

typedef struct {
    void* memory;
    size_t position;
    size_t size;
} MemoryFile;

extern void OpenRAMFile(void *addr, size_t size, CFile *f, MemoryFile *super);

extern void MemoryFileSeek(CFile *file, size_t address);
extern size_t MemoryFileRead(CFile *file, void *data, size_t len);
extern size_t MemoryFileWrite(CFile * file, void *data, size_t len);
extern bool MemoryFileEOF(CFile * file);
