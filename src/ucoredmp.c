#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ucoredmp.h"

#define EI_CLASS_32BIT (1)
#define EI_CLASS_64BIT (2)

#define EI_DATA_LITTLEENDIAN (1)
#define EI_DATA_BIGENDIAN (2)

#define EI_VERSION (6)

#define EI_OSABI_STANDALONE (255)

#define ET_TYPE_CORE (4)

#define ET_MACHINE_ARM (0x28)

#define ELF_VERSION (1)

typedef struct elf_header_t {
  // 0x00
  uint8_t magic[4];  // magic number. Always [0x75, 0x45, 0x4C, 0x46]
  uint8_t bitFormat;
  uint8_t endianess;
  uint8_t eiVersion;
  uint8_t osabi;
  uint8_t pad1[8];

  // 0x10
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  size_t entryPoint;
  size_t programHeaderOffset;
  size_t sectionHeaderOffset;
  uint32_t flags;
  uint16_t elfHeaderSize;
  uint16_t programHeaderEntrySize;
  uint16_t numProgramHeaderEntries;
  uint16_t sectionHeaderEntrySize;
  uint16_t numSectionHeaderEntries;
  uint16_t sectionHeaderNamesEntryIdx;
} elf_header_t;

static_assert((sizeof(size_t) == 4) || (sizeof(size_t) == 8),
              "Only supports 32-bit and 64-bit arch");

static uint8_t get_host_endianess();
static void ucoredmp_init_header(elf_header_t* header);
static void ucoredmp_init_header_magic(elf_header_t* header);

int ucoredmp_write() { 
    FILE* fp;
    elf_header_t header;
    ucoredmp_init_header(&header);
    fp = fopen("dump.dmp", "wb");
    fwrite(&header, sizeof(uint8_t), sizeof(header), fp);
    fclose(fp);
    fp = 0;
    return 0;
}

static void ucoredmp_init_header(elf_header_t* header) {
  if (!header) return;

  ucoredmp_init_header_magic(header);

  if (sizeof(size_t) == 4) {
    header->bitFormat = EI_CLASS_32BIT;
  } else {
    header->bitFormat = EI_CLASS_64BIT;
  }

  header->endianess = get_host_endianess();

  header->eiVersion = ELF_VERSION;
  header->osabi = EI_OSABI_STANDALONE;

  header->type = ET_TYPE_CORE;
  header->machine = ET_MACHINE_ARM;
  header->version = ELF_VERSION;
  header->entryPoint = 0;
  header->programHeaderOffset = 0;
  header->sectionHeaderOffset = 0;
  header->flags = 0;
  header->elfHeaderSize = sizeof(elf_header_t);
  header->programHeaderEntrySize = 0;
  header->numProgramHeaderEntries = 0;
  header->sectionHeaderEntrySize = 0;
  header->numSectionHeaderEntries = 0;
  header->sectionHeaderNamesEntryIdx = 0;
}

static void ucoredmp_init_header_magic(elf_header_t* header) {
  if (!header) return;

  header->magic[0] = 0x7F;
  header->magic[1] = 'E';
  header->magic[2] = 'L';
  header->magic[3] = 'F';
}

static uint8_t get_host_endianess() {
    static uint32_t i = 1;
    uint8_t* byte = (uint8_t*)&i;
    if (byte[0] == 1) {
        return EI_DATA_LITTLEENDIAN;
    } else {
        return EI_DATA_BIGENDIAN;
    }
}