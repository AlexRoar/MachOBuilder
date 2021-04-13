//
// Created by Александр Дремов on 10.04.2021.
//

#ifndef machoFileBin_GUARD
#define machoFileBin_GUARD
#include "machoStructure.h"
#include "public/FastList.h"

struct MachoFileBin {
    machHeader64 header;
    FastList<loadCommand> loadCommands;
    FastList<binPayload> payload;

    void init();

    void dest();

    static MachoFileBin *New();

    static void simpleExe(binaryFile& binary, const char* code, size_t size);

    void Delete();

    void binWrite(binaryFile *out);

    void postprocess(binaryFile *out);

    void threadSectionLink(binaryFile *out);

    void mainSectionLink(binaryFile *out);

    void vmRemap(binaryFile *out);

    void payloadsProcess(binaryFile *out);

    void fileOffsetsRemap(binaryFile *out);

    segmentSection *getSectionByIndex(size_t sectionNum, loadCommand** lc= nullptr);
};

#endif //machoFileBin_GUARD
