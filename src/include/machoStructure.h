//
// Created by Александр Дремов on 09.04.2021.
//

#ifndef CLANGUAGE_MACHOHEADERSTR_H
#define CLANGUAGE_MACHOHEADERSTR_H

#include <mach/machine.h>
#include <mach/mach.h>
#include <mach-o/reloc.h>
#include <mach-o/nlist.h>
#include <mach-o/loader.h>
#include <cstring>
#include <cstdio>

#ifndef PUBLIC_HEADER
#include "binaryFile.h"
#include "loadCommands.h"
#include "public/FastList.h"
    #endif

#define alignSmall 8
#define alignPage 4096

/*
 * Load commands must be aligned to an 8-byte boundary for 64-bit Mach-O files.
 */
struct loadCommand {
    enum loadCommandType {
        LC_TYPE_SEGMENT,
        LC_TYPE_THREAD,
        LC_TYPE_MAIN,
        LC_TYPE_DYSYMTAB,
        LC_TYPE_SYMTAB
    };
    union {
        unixThreadCommand threadSeg;
        segmentCommand64 generalSeg;
        entryPointCommand entrySeg;
        symtabCommand symtabSeg;
        dysymtabCommand dysymtabSeg;
    };
    FastList<segmentSection> sections;
    FastList<unsigned> payloads;
    loadCommandType type;
    size_t offset;

    void init();

    void dest();

    static loadCommand pageZero();

    static loadCommand code();

    static loadCommand data();

    static loadCommand symtab();

    static loadCommand dysymtab();

    static loadCommand main(unsigned segNum, size_t stackSize = 0);

    static loadCommand thread(unsigned segNum);

    void binWrite(binaryFile *out);

    static loadCommand codeObject();
};

struct machHeader64 {
    mach_header_64 header;
    size_t offset;

    static machHeader64 executable();

    static machHeader64 object();

    void binWrite(binaryFile *out);
};

struct binPayload {
    char *payload;
    size_t size;
    size_t realSize;
    size_t offset;
    bool freeable;
    unsigned align;

    void binWrite(binaryFile *out);

    void dest();
};

#endif //CLANGUAGE_MACHOHEADER_H
