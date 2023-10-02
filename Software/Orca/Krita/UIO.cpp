// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * UIO
 * rainbain
 * 9/28/2023
 * Orca Emulator
 *
 * Manages memory between the hardware and user space software.
*/

#include "UIO.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iomanip>

#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <poll.h>

using namespace ZynqUSP;

void UIO::ThreadFunction(){
    pollfd poll_fd[2];

    // Message pipe
    poll_fd[0].fd = pipe_fds[0];
    poll_fd[0].events = POLLIN;

    // Interrupt
    poll_fd[1].fd = threadFd;
    poll_fd[1].events = POLLIN;

    while(GetThreadOpen()){
        // Unmask interrults
        uint32_t info;
        ssize_t bytes_read = write(threadFd, &info, sizeof(info));
        if(bytes_read != (ssize_t)sizeof(info)){
            printf("UIO Interrupt Error, write(...) != %lu\n", sizeof(info));
            break;
        }

        int ret = poll(poll_fd, 2, -1);

        if(ret == -1){
            printf("UIO Interrupt Error, pol(...) == -1\n");
            break;
        }

        if(poll_fd[1].revents & POLLIN){
            bytes_read = read(threadFd, &info, sizeof(info));

            if(bytes_read != (ssize_t)sizeof(info)){
                printf("UIO Interrupt Error, read(...) != %lu\n", sizeof(info));
                continue;
            }

            handler(info);
        }

        if(poll_fd[0].revents & POLLIN){
            char cmd;
            bytes_read = read(pipe_fds[0], &cmd, 1);

            if(bytes_read != 1){
                printf("UIO Interrupt Error, read pipe failed!\n");
                continue;
            }

            switch(cmd) {
                case 'X':
                    SetThreadOpen(false);
                    break;
                default:
                    printf("UIO Interrput Error, unknown command '%c'\n", cmd);
                    break;
            }
        }
    }

    SetThreadOpen(false); // In case we reached here by exit, insure our exit is known.
    close(fd);
}

bool UIO::GetThreadOpen(){
    bool tmp;
    threadLock.lock();
    tmp = threadOpen;
    threadLock.unlock();
    return tmp;
}

void UIO::SetThreadOpen(bool val){
    threadLock.lock();
    threadOpen = val;
    threadLock.unlock();
}


UIO::UIO(){
    isOpen = false;
}

UIO::~UIO(){
    if(threadOpen) {
        SetThreadOpen(false);
        write(pipe_fds[1], "X", 1);
        irqThread.join();

        close(pipe_fds[0]);
        close(pipe_fds[1]);
    }
    if(isOpen) {
        munmap(mapping, addressSpace);
        close(fd);
    }
}

void* UIO::GetMemory(size_t offset){
    if(offset > addressSpace){
        printf("UIO get memory failed, 0x%lx is greator than the address space of 0x%lx.\n", offset, addressSpace);
        return 0;
    }

    return (uint8_t*)mapping + offset;
}

bool UIO::Open(std::string path, size_t addressSpace) {
    this->addressSpace = addressSpace;
    fd = open(path.c_str(), O_RDWR);

    if(fd < 0){
        printf("Failed to open UIO at \"%s\"\n", path.c_str());
        return false;
    }

    mapping = mmap(NULL, addressSpace, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);\

    if(mapping == MAP_FAILED) {
        printf("Failed to map UIO memory at \"%s\" with address space 0x%lx\n", path.c_str(), addressSpace);
        close(fd);
        return false;
    }

    isOpen = true;
    return true;
}

void UIO::SetupInterrupts(IRQHandler onInterrupt){
    if(pipe(pipe_fds) == -1){
        printf("UIO, failed to create thread pipes for interrupt!\n");
        return;
    }

    this->threadFd = fd;
    this->handler = onInterrupt;
    SetThreadOpen(true);

    this->irqThread = std::thread(&UIO::ThreadFunction, this);
}



void UIOIndexing::Index() {
    entrys.clear();

    /*
     * Directory listing of /dev/class/uio. Only including paths begining in uio
    */
    std::vector<std::string> uioListings;

    DIR *dir = 0;
    struct dirent *ent;
    if((dir = opendir(uioEnumerationPath.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL){
            std::string entry = ent->d_name;
            if(entry.size() < 4) continue;;
            if(entry[0] != 'u' || entry[1] != 'i' || entry[2] != 'o'){
                continue;
            }
            uioListings.push_back(entry);
        }
        closedir(dir);
    } else {
        printf("UIO indexing failed, failed to get directory listing of \"%s\"\n", uioEnumerationPath.c_str());
        return;
    }

    /*
     * Convert into symbolic link targets.
    */
   char targetPath[1024];

   for(std::string& entry : uioListings) {
        std::string linkPath = uioEnumerationPath + "/" + entry;

        ssize_t len = readlink(linkPath.c_str(), targetPath, sizeof(targetPath) - 1);

        if(len != -1){
            targetPath[len] = 0; // Null terminate
            std::string entryString = targetPath;
            entry = entryString;
        } else{
            printf("Failed to read symbolic link of \"%s\"\n", linkPath.c_str());
        }
   }

   /*
    * Convert into index entrys.
   */
    std::regex pattern("uio(\\d+)");
    for(std::string& entry : uioListings) {
        std::vector<std::string> parts;
        std::stringstream ss(entry);
        std::string part;
        while(std::getline(ss, part, '/')){
            parts.push_back(part);
        }

        if(parts.size() < 3){
            printf("Failed to decode UIO symbolic link: \"%s\"", entry.c_str());
        }

        std::string uioPart = parts.back();
        std::string dtEntryPart = parts[parts.size() - 3];

        std::smatch matches;
        int uioIndex;
        if(std::regex_match(uioPart, matches, pattern)) {
            std::string indexStr = matches[1].str();
            uioIndex = std::stoi(indexStr);
        } else {
            printf("Failed to decode UIO symbolic link, \"%s\" was a unexpected format as part of \"%s\"\n", uioPart.c_str(), entry.c_str());
        }
        entrys[dtEntryPart] = uioIndex;
    }

    printf("Indexed %lu UIO devices.\n", entrys.size());
}

std::string UIOIndexing::GetIndexIndintifyer(uint64_t addr, std::string deviceName){
    std::stringstream nameStream;
    nameStream << std::hex << (addr);
    nameStream << "." << deviceName;
    return nameStream.str();
}

std::string UIOIndexing::GetUIOPath(int id){
    std::stringstream nameStream;
    nameStream << "/dev/uio" << id;
    return nameStream.str();
}


UIOIndexing::UIOIndexing() {
    uioEnumerationPath = "/sys/class/uio";
    Index();
}

bool UIOIndexing::IndexUIO(UIO *uio, uint64_t addr, size_t addressSpace, std::string deviceName){
    std::string nameString = GetIndexIndintifyer(addr, deviceName);

    auto location = entrys.find(nameString);
    if(location == entrys.end()){
        printf("Could not locate UIO entry \"%s\"\n", nameString.c_str());
        return false;
    }

    std::string path = GetUIOPath(location->second);

    return uio->Open(path, addressSpace);
}

std::string UIOIndexing::DumpUIOTable(){
    std::ostringstream formattedStream;

    for(auto& loc : entrys){
        formattedStream << loc.second << ":\n";
        formattedStream << "  Device Module Location: " << GetUIOPath(loc.second) << "\n";
        formattedStream << "  Device Tree Idintifyer: " << loc.first << "\n\n";
    }

    return formattedStream.str();
}