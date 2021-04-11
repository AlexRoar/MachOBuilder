//
// Created by Александр Дремов on 11.04.2021.
//

#include "machoStructure.h"


/*
 * Load commands must be aligned to an 8-byte boundary for 64-bit Mach-O files.
 */

void loadCommand::init() {
    payloads.init();
    sections.init();
}

void loadCommand::dest() {
    payloads.dest();
    sections.dest();
}

loadCommand loadCommand::pageZero() {
    loadCommand seg = {};
    seg.init();
    seg.sections.init();
    seg.generalSeg.segment.cmd = LC_SEGMENT_64;
    strcpy(seg.generalSeg.segment.segname, SEG_PAGEZERO);
    seg.generalSeg.segment.vmsize = 0x0000000100000000;
    seg.type = LC_TYPE_SEGMENT;
    return seg;
}

loadCommand loadCommand::code() {
    loadCommand seg = {};
    seg.init();
    seg.generalSeg.segment.cmd = LC_SEGMENT_64;
    strcpy(seg.generalSeg.segment.segname, SEG_TEXT);
    seg.generalSeg.segment.vmsize = 0x0000000000004000;
    seg.generalSeg.segment.maxprot = seg.generalSeg.segment.initprot = VM_PROT_READ | VM_PROT_EXECUTE;
    seg.type = LC_TYPE_SEGMENT;
    return seg;
}

loadCommand loadCommand::main(unsigned segNum) {
    loadCommand seg = {};
    seg.init();
    seg.entrySeg.segment.cmd = LC_MAIN;
    seg.entrySeg.segment.cmdsize = sizeof(seg.entrySeg.segment);
    seg.entrySeg.segment.stacksize = 0;
    seg.entrySeg.sectionIndex = segNum;
    seg.type = LC_TYPE_MAIN;
    return seg;
}

loadCommand loadCommand::thread(unsigned segNum) {
    loadCommand seg = {};
    seg.init();
    seg.threadSeg.segment.thread.cmd = LC_UNIXTHREAD;
    seg.threadSeg.segment.thread.cmdsize = sizeof(seg.threadSeg.segment);
    seg.threadSeg.segment.flavor = x86_THREAD_STATE64;
    seg.threadSeg.segment.count = x86_THREAD_STATE64_COUNT;
    seg.threadSeg.sectionIndex = segNum;
    seg.type = LC_TYPE_THREAD;
    return seg;
}

void loadCommand::binWrite(binaryFile *out) {
    offset = out->sizeNow;
    switch (type) {
        case LC_TYPE_SEGMENT: {
            BINFILE_WRITE_STRUCT(generalSeg.segment);
            for (size_t it = this->sections.begin(); it != this->sections.end(); this->sections.nextIterator(&it)) {
                segmentSection *section = nullptr;
                this->sections.get(it, &section);
                section->offset = out->sizeNow;
                section->binWrite(out);
            }
            generalSeg.segment.nsects = this->sections.getSize();
            BINFILE_UPDATE(offset, generalSeg.segment, nsects);
            break;
        }
        case LC_TYPE_THREAD: {
            BINFILE_WRITE_STRUCT(threadSeg.segment);
            break;
        }
        case LC_TYPE_MAIN: {
            BINFILE_WRITE_STRUCT(entrySeg.segment);
            break;
        }
    }
    uint32_t cmdsize = out->sizeNow - offset;
    out->writeZeros(cmdsize % alignSmall); // size divisible by 8;
    generalSeg.segment.cmdsize = out->sizeNow - offset;
    BINFILE_UPDATE(offset, generalSeg.segment, cmdsize);
}

machHeader64 machHeader64::general() {
    machHeader64 header = {};
    header.header.magic = MH_MAGIC_64;
    header.header.cputype = CPU_TYPE_X86_64;
    header.header.cpusubtype = CPU_SUBTYPE_X86_64_ALL | CPU_SUBTYPE_LIB64;
    header.header.filetype = MH_EXECUTE;
    header.header.ncmds = 0; /* to be modified */
    header.header.sizeofcmds = 0; /* to be modified after writing segments */
    header.header.flags = MH_NOUNDEFS;
    return header;
}

void machHeader64::binWrite(binaryFile *out) {
    offset = out->sizeNow;
    BINFILE_WRITE_STRUCT(header);
}

void binPayload::binWrite(binaryFile *out) {
    out->alignZeroes(align);
    offset = out->sizeNow;
    out->write(payload, size);
    out->writeZeros(size % alignSmall);
    realSize = size + size % alignSmall;
}

void binPayload::dest() {
    if (freeable)
        free(payload);
}

