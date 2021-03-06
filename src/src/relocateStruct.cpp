//
// Created by Александр Дремов on 17.04.2021.
//
#include <cstring>
#include "machoBinFile.h"
#include "relocateStruct.h"
#include "binaryFile.h"

void relocatePayload::binWrite(binaryFile *out) {
    relocationInfo tmp = {};
    for (size_t i = info.begin(); i != info.end(); info.nextIterator(&i)) {
        info.get(i, &tmp);
        BINFILE_WRITE_STRUCT(tmp.info);
    }
}

binPayload relocatePayload::bufferWrite() {
    auto buf = (char *) calloc(info.getSize(), sizeof(relocation_info));
    char *now = buf;
    relocationInfo tmp = {};
    for (size_t i = info.begin(); i != info.end(); info.nextIterator(&i), now += sizeof(tmp.info)) {
        info.get(i, &tmp);
        memcpy(now, (char *) (&(tmp.info)), sizeof(tmp.info));
    }
    return {buf, info.getSize() * sizeof(relocation_info), 0, 0, true, 1};
}

void relocatePayload::init() {
    info.init();
}

void relocatePayload::dest() {
    info.dest();
}

void relocatePayload::addReloc(const char* name, int32_t r_address, uint32_t r_symbolnum, uint32_t r_pcrel, uint32_t r_length,
                               uint32_t r_extern, uint32_t r_type) {
    relocation_info tmp = {r_address, r_symbolnum, r_pcrel, r_length, r_extern, r_type};
    info.pushBack({tmp, name});
}

void relocatePayload::addReloc(const relocationInfo &other) {
    info.pushBack(other);
}
