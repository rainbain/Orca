// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CFox
 * rainbain
 * 8/15/2023
 * Orca Emulator
 *
 * A simple File type
*/

#include "CFox.h"

void OpenRAMFile(void *addr, size_t size, CFile *f, MemoryFile *super){
    super->memory = addr;
    super->position = 0;
    super->size = size;

    f->super = super;
    f->seek = MemoryFileSeek;
    f->read = MemoryFileRead;
    f->write = MemoryFileWrite;
    f->eof = MemoryFileEOF;
}

void MemoryFileSeek(CFile *file, size_t address){
    MemoryFile *super = (MemoryFile*)file->super;
    super->position = address;
}

size_t MemoryFileRead(CFile *file, void *data, size_t len){
    MemoryFile *super = (MemoryFile*)file->super;
    
    size_t actionSize = len;
    if(super->position + len > super->size){
        actionSize = super->size - super->position;
    }

    memcpy(data, super->memory+super->position, actionSize);

    super->position += actionSize;

    return actionSize;
}

size_t MemoryFileWrite(CFile * file, void *data, size_t len){
    MemoryFile *super = (MemoryFile*)file->super;

    size_t actionSize = len;
    if(super->position + len > super->size){
        actionSize = super->size - super->position;
    }

    memcpy(super->memory+super->position, data, actionSize);

    super->position += actionSize;

    return actionSize;
}

bool MemoryFileEOF(CFile * file){
    MemoryFile *super = (MemoryFile*)file->super;

    if(super->position >= super->size) return true;
    return false;
}