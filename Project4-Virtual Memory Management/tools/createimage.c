#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMAGE_FILE "./image"
#define ARGS "[--extended] [--vm] <bootblock> <executable-file> ..."

#define SECTOR_SIZE 512
#define OS_SIZE_LOC 2
#define BOOT_LOADER_SIG_OFFSET 0x1fe
#define BOOT_LOADER_SIG_1 0x55
#define BOOT_LOADER_SIG_2 0xaa
#define BOOT_MEM_LOC 0x7c00
#define OS_MEM_LOC 0x1000

/* structure to store command line options */
static struct
{
    int vm;
    int extended;
} options;

/* prototypes of local functions */
static void create_image(int nfiles, char *files[]);
static void error(char *fmt, ...);
static void read_ehdr(Elf64_Ehdr *ehdr, FILE *fp);
static void read_phdr(Elf64_Phdr *phdr, FILE *fp, int ph,
                      Elf64_Ehdr ehdr);
static void write_segment(Elf64_Ehdr ehdr, Elf64_Phdr phdr, FILE *fp,
                          FILE *img, int *nbytes, int *first);
static void write_os_size(int nbytes, FILE *img);
static void write_user_thread_segment(Elf64_Ehdr ehdr, Elf64_Phdr phdr, FILE *fp,
                          FILE *img, int *nbytes, int *first);
                          
int main(int argc, char **argv)
{
	char *progname = argv[0];
	options.vm = 0;
	options.extended = 0;
	
	while((argc > 1) && (argv[1][0] == '-') && (argv[1][1] == '-')){
		char *option = &argv[1][2];
		if (strcmp(option, "vm") == 0) {
			options.vm = 1;
		}
		else if(strcmp(option, "extended") == 0) {
			options.extended = 1;
		} 
		else{
			error("%s: invalid option\nusage: %s %s\n", progname, progname, ARGS);
		}
		argc--;
		argv++;
	}
	if(options.vm == 1){
		error("%s: option --vm not implemented\n", progname);
	}
	if(argc < 3){
        	/* at least 3 args (createimage bootblock kernel) */
        	error("usage: %s %s\n", progname, ARGS);
	}
	
	create_image(argc-1, argv+1);
	return 0;
}

static void create_image(int nfiles, char *files[])
{
	FILE *fp, *img;
	Elf64_Ehdr ehdr;
	Elf64_Phdr phdr;
	int i, ph;
	int first = 1, nbytes = 0;
	
	/* create the image file */
	img = fopen(IMAGE_FILE,"wb+");
	for(i=0; i<nfiles; i++){
		/* open input file */
		fp = fopen(files[i],"rb+");
		/* read ELF header */
		read_ehdr(&ehdr, fp);
		
		if(options.extended)
		printf("0x%04lx: %s\n", ehdr.e_entry, files[i]);
		
		/* read each program header */
		for(ph=0; ph<ehdr.e_phnum; ph++){
			read_phdr(&phdr, fp, ph, ehdr);
			if(options.extended){
			printf("\tsegment %d:\n", ph);
			printf("\t\toffset 0x%04lx\tvaddr 0x%04lx\n",phdr.p_offset, phdr.p_vaddr);
			printf("\t\tfilesz 0x%04lx\tmemsz 0x%04lx\n",phdr.p_filesz, phdr.p_memsz);
			}
			/* write segment to the image */
			write_segment(ehdr, phdr, fp, img, &nbytes, &first);
		}
		fclose(fp);
	}
	write_os_size(nbytes, img);
	fclose(img);
}

static void read_ehdr(Elf64_Ehdr *ehdr, FILE *fp)
{
	fread(ehdr,1,64,fp);
	rewind(fp);
}

static void read_phdr(Elf64_Phdr *phdr, FILE *fp, int ph,
                      Elf64_Ehdr ehdr)
{
	fseek(fp, ehdr.e_phoff + ph * ehdr.e_phentsize, 0);
	fread(phdr,1,ehdr.e_phentsize,fp);
	rewind(fp);
}

static void write_segment(Elf64_Ehdr ehdr, Elf64_Phdr phdr, FILE *fp,
                          FILE *img, int *nbytes, int *first)
{
	char *segment = (char*)malloc(phdr.p_memsz);
	unsigned int write_offset; 
	unsigned int padding_size;
	
	/* read segment from elf */
	memset(segment,0,phdr.p_memsz);
	fseek(fp,phdr.p_offset,0);
	fread(segment,1,phdr.p_filesz,fp);
	rewind(fp);
	
	/* calculate offset in image file */
	if(*first){
		write_offset = 0;
		*first = 0;
	}
	else{
		write_offset = SECTOR_SIZE + *nbytes;
		if(phdr.p_memsz % SECTOR_SIZE)
			*nbytes = *nbytes + ((phdr.p_memsz / SECTOR_SIZE)+1) * SECTOR_SIZE;
		else
			*nbytes = *nbytes + phdr.p_memsz;
	}
	
	/* write segment to image */
	fseek(img,write_offset,0);
	fwrite(segment,1,phdr.p_memsz,img);
	free(segment);
	if(options.extended)
	printf("\t\twriting 0x%04lx bytes\n",phdr.p_memsz);
	
	/* pad 0 by sector alignment */
	if(phdr.p_memsz % SECTOR_SIZE){
		padding_size = SECTOR_SIZE - (phdr.p_memsz % SECTOR_SIZE);
		char *padding = (char*)malloc(padding_size);
		memset(padding,0,padding_size);
		fwrite(padding,1,padding_size,img);
		free(padding);
	}
	if(options.extended)
	printf("\t\tpadding up to 0x%04x\n",(*nbytes)+SECTOR_SIZE);
	rewind(img);
}
 
static void write_os_size(int nbytes, FILE *img)
{
	unsigned short *size = (unsigned short *)malloc(sizeof(unsigned short));
	*size = nbytes / SECTOR_SIZE;
	fseek(img, (BOOT_LOADER_SIG_OFFSET - OS_SIZE_LOC), 0);
	fwrite(size,1,2,img);
	rewind(img);
	printf("size of kernel is %u sectors\n",*size); 
}

/* print an error message and exit */
static void error(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	if(errno != 0){
		perror(NULL);
	}
	exit(EXIT_FAILURE);
}
