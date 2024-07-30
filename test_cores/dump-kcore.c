#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define field(buf, offset, typ) (*(typ*)((char*) buf + offset))
const uint64_t PHDR_SIZE = 0x38;

struct output_file
{
    int fd;
    uint64_t header_start, next_header;
    uint64_t next_offset;
};

struct ram_mapping
{
    int num;
    struct {
        uint64_t start, end;
        uint64_t output_file_offset;
    } ents[64];
};

uint64_t min(uint64_t a, uint64_t b)
{
    return a < b ? a : b;
}

uint64_t round_up(uint64_t a, uint64_t v)
{
    if (v == 0 || v == 1)
        return a;
    a -= 1;
    a /= v;
    a += 1;
    a *= v;
    return a;
}

void error(char *desc)
{
    fprintf(stderr, "Error: %s\n", desc);
    exit(-1);
}


void read_exactly(int infd, off_t offset, size_t size, char *data)
{
    if (lseek(infd,  offset,  SEEK_SET) != offset)
        error("Seeking input");

    while (size) {
        int ret = read(infd, data, size);
        if (ret < 0) {
            error("read failed");
        } else if (ret == 0) {
            error("file too small");
        } else {
            size -= ret;
            data += ret;
        }
    }
}

void write_exactly(int outfd, off_t offset, size_t size, char *data)
{
    if (lseek(outfd, offset, SEEK_SET) != offset)
        error("Seeking output");

    while (size) {
        int ret = write(outfd, data, size);
            
        if (ret < 0) {
            error("write failed");
        }

        data += ret;
        size -= ret;
    }
}

void copy_file_data(int infd, off_t in_offset, size_t size, int outfd, off_t out_offset)
{
    if (lseek(infd, in_offset, SEEK_SET) != in_offset)
        error("Seeking input2");
        
    char buf[4096 * 20];
    
    while (size) {
        int rret = read(infd, buf, min(size, sizeof(buf)));
        if (rret < 0) {
            error("read failed");
        } else if (rret == 0) {
            error("file too small");
        }

        write_exactly(outfd, out_offset, rret, buf);
        size -= rret;
        out_offset += rret;
    }    
}


void get_ram_mappings(struct ram_mapping *ram_map)
{
    FILE *fd = fopen("/proc/iomem", "r");
    if (!fd)
        error("Opening ram file");

    char line[256];
    int i = 0;
    while (fgets(line, sizeof(line), fd)) {
        if (strstr(line, "System RAM")) {
            sscanf(line, "%lx-%lx",
                   &ram_map->ents[i].start,
                   &ram_map->ents[i].end);
            
            printf("Physical RAM %lx-%lx\n",
                   ram_map->ents[i].start,
                   ram_map->ents[i].end);
            i++;

        }
    }

    ram_map->num = i;
    
    fclose(fd);
}


void dump_physical_ram(struct ram_mapping *ram_map,
                       int infd,
                       uint64_t in_phys_offset,
                       struct output_file *out)
{
    for (int i = 0; i < ram_map->num; i++) {
        uint64_t size     = ram_map->ents[i].end   - ram_map->ents[i].start;
        uint64_t in_start = ram_map->ents[i].start + in_phys_offset;
        
        out->next_offset = round_up(out->next_offset, 4096);
        copy_file_data(infd, in_start, size, out->fd, out->next_offset);
        ram_map->ents[i].output_file_offset = out->next_offset;
        out->next_offset += size;
    }
}

void output_mapping(struct output_file *out,
                    uint64_t vaddr,
                    uint64_t paddr,
                    uint64_t file_off,
                    uint64_t size)
{
    char prog_header[PHDR_SIZE];
    field(prog_header, 0x00, uint32_t) = 1; /* PT_LOAD */
    field(prog_header, 0x04, uint32_t) = 7; /* RWX */
    field(prog_header, 0x08, uint64_t) = file_off;
    field(prog_header, 0x10, uint64_t) = vaddr;
    field(prog_header, 0x18, uint64_t) = paddr;
    field(prog_header, 0x20, uint64_t) = size; /* file size */
    field(prog_header, 0x28, uint64_t) = size; /* memsize */
    field(prog_header, 0x30, uint64_t) = 0x1000; /* align */
    
    write_exactly(out->fd, out->next_header, sizeof(prog_header), prog_header);
    out->next_header += PHDR_SIZE;
}

void process_load_phdr(uint64_t phdr_paddr,
                       uint64_t phdr_vaddr,
                       uint64_t size,
                       struct ram_mapping *ram_map,
                       struct output_file *out) {
    //      SP               SP+S
    ///      |----------------|
    ///    |####|
    //     S    E
    for (int i = 0; i < ram_map->num; i++) {
        int64_t start_pa, end_pa;
        if (ram_map->ents[i].start > phdr_paddr)
            start_pa = ram_map->ents[i].start;
        else
            start_pa = phdr_paddr;

        if (ram_map->ents[i].end < phdr_paddr + size)
            end_pa = ram_map->ents[i].end;
        else
            end_pa = phdr_paddr + size;

        if (start_pa < end_pa) {
            uint64_t mapping_offset = start_pa - phdr_paddr;
            uint64_t ram_offset     = start_pa - ram_map->ents[i].start;
            uint64_t size           = end_pa - start_pa;

            //printf("%#lx %#lx %#lx\n", ram_offset, ram_map->ents[i].start, phdr_paddr);
            
            output_mapping(out,
                           /* vaddr= */   phdr_vaddr + mapping_offset,
                           /* paddr= */   phdr_paddr + mapping_offset,
                           /* fileoff= */ ram_map->ents[i].output_file_offset + ram_offset,
                           /* size = */ size
                           );
        }
    }
}

int main(int argc, char** argv)
{
    struct ram_mapping ram;
    get_ram_mappings(&ram);
    
    int infd = open("/proc/kcore", O_RDONLY);
    if (infd < 0)
        error("open input file failed");

    char elf_header[64];
    read_exactly(infd, 0, sizeof(elf_header), elf_header);

    if (field(elf_header, 0, uint32_t) != 0x464c457f)
        error("Invalid Elf");

    printf("Type: %#x\n",  field(elf_header, 0x10, uint16_t));
    printf("Entry: %#x\n", field(elf_header, 0x18, uint64_t));

    uint64_t phdr_start = field(elf_header, 0x20, uint64_t);
    uint64_t phdr_esize = field(elf_header, 0x36, uint16_t);
    uint64_t phdr_num   = field(elf_header, 0x38, uint16_t);

    if (phdr_esize != PHDR_SIZE)
        error("Invalid program header size");
    
    uint64_t shdr_start = field(elf_header, 0x28, uint64_t);
    uint64_t shdr_esize = field(elf_header, 0x3A, uint16_t);
    uint64_t shdr_num   = field(elf_header, 0x3C, uint16_t);

    /* prepare output file */
    
    int outfd = open("out.kcore", O_WRONLY | O_CREAT | O_TRUNC);
    if (outfd < 0)
        error("open output file failed");

    struct output_file out;
    out.fd = outfd;
    out.header_start = sizeof(elf_header);
    out.next_header = out.header_start;
    out.next_offset = sizeof(elf_header) + 5*4096;

    /* copying the program headers */

    uint64_t infile_phys_offset = 0;
    uint64_t total_bytes = 0;
    
    for (int phdr = 0; phdr < phdr_num; phdr++) {
        char prog_header[PHDR_SIZE];
        read_exactly(infd,
                     phdr_start + phdr * PHDR_SIZE,
                     PHDR_SIZE,
                     prog_header);

        uint32_t mapping_type = field(prog_header, 0x00, uint32_t);
        uint64_t file_start   = field(prog_header, 0x08, uint64_t);
        uint64_t file_size    = field(prog_header, 0x20, uint64_t);
        uint64_t mem_start    = field(prog_header, 0x10, uint64_t);
        uint64_t mem_size     = field(prog_header, 0x28, uint64_t);
        uint64_t pa_start     = field(prog_header, 0x18, uint64_t);
        uint64_t align        = field(prog_header, 0x30, uint64_t);
        
        if (mapping_type == 1) { /* PT_LOAD */
            if (pa_start == (uint64_t) -1) {
                /* vmalloc / vmemmap,
                   it would be great to get these
                   but the dump doesn't give us any way to actually figure out
                   what parts of this range are mapped */
                continue;
            }
            
            if (infile_phys_offset == 0 && pa_start != (uint64_t) -1) {
                /* initalise data for load segments */
                /* file_start = [] + pa_start */
                infile_phys_offset = file_start - pa_start;
                dump_physical_ram(&ram,
                                  infd,
                                  infile_phys_offset,
                                  &out);
            }

            process_load_phdr(pa_start,
                              mem_start,
                              mem_size,
                              &ram,
                              &out);
        } else {
            /* other */
            out.next_offset = round_up(out.next_offset, align);
            copy_file_data(infd,
                           file_start,
                           file_size,
                           out.fd,
                           out.next_offset);
            
            field(prog_header, 0x08, uint64_t) = out.next_offset;
            out.next_offset += file_size;

            write_exactly(outfd,
                          out.next_header,
                          PHDR_SIZE,
                          prog_header);
            out.next_header += PHDR_SIZE;
        }
        
        /* compute the physical offset */
        total_bytes += mem_size;
    }


    /* write output header */
    field(elf_header, 0x20, uint64_t) = out.header_start; /* program header */
    field(elf_header, 0x38, uint16_t) = (out.next_header - out.header_start) / PHDR_SIZE; /* program header num */
    field(elf_header, 0x28, uint64_t) = 0; /* section header */
    field(elf_header, 0x3c, uint16_t) = 0; /* section header num */
    write_exactly(out.fd, 0, sizeof(elf_header), elf_header);

    
    printf("%ld bytes total\n", total_bytes);
}
