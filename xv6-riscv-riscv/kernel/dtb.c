#include "types.h"
void printf (char*, ...);

uint64 dtb_pa;

int strncmp (const char*, const char*, uint);




// DTB magic number - every valid DTB starts with this
// it's how we know we're reading a device tree
#define FDT_MAGIC       0xd00dfeed

// There are token types inside the DTB structure
// Think of them like HTML tags - they mark beginning and end of nodes
#define FDT_BEGIN_NODE  1       //  Start of a nodes - like <div>
#define FDT_END_NODE     2       //  end of a nodes - like <div>
#define FDT_PROP                3        //  a property inside a node -like an attribute
#define FDT_NOP                  4        // do nothing, skip
#define FDT_END                   9        //   end of the entire tree

// DTB is big-endian but RISCV is little endian
 //  This function flips the bytes so we read numbers correctly
 //  Think of it like reading a word written backwards
static uint32
bswap32(uint32 x) {
    return ((x & 0xff000000) >> 24) |
               ((x & 0x00ff0000) >>  8)  |
               ((x & 0x0000ff00) <<  8 ) |
               ((x & 0x000000ff) << 24);
}

// Simple string starts with check
// Returns 1 if s starts with prefix


// Main function - parse DTB and return memory size in byte
uint64
dtb_memsize(void)
{
   uint64 dtb = 0x87000000;
    if(dtb == 0){
        printf("dtb is 0\n");
    
         return 0; }
    uint32 *fdt = (uint32 *)dtb;

    // First check the magic number
    // if this isn't 0xd00dfeed something is wrong
    if (bswap32(fdt[0]) != FDT_MAGIC){
        printf("bad magic: %x\n " , bswap32(fdt[0]));
        return 0;
    }

    printf("magic ok, parsing ...\n");


    // read offsets from the header
    // these tell us where the structure and strings sections start
    uint32 off_struct = bswap32(fdt[2]); // offset to structure section
    uint32 off_strings = bswap32(fdt[3]); // offset to string section

    //get pointer to each section
    uint32 *p = (uint32 *)((char *) dtb_pa + off_struct);
    char     *strings    = (char     *)((char*)dtb_pa + off_strings);

    int in_memory = 0;  

    while (1)
    {
        uint32 token = bswap32(*p++);
        if(token == FDT_BEGIN_NODE){
            //starts of a new node
            //the node name follows immediatly as a string
            char *name = (char *)p;
            printf("node: %s\n", name);

            //check if this is a memory node
            //memory nodes are named "memory@xxxxxxx"
            in_memory= (strncmp(name, "memory@", 7)== 0);
            //advance past the name string
            //names are padded to 4 byte allignment
            int len = 0;
            while (name[len]) len++; // find end of string
            p += (len +4) /4; //skip past it (4 byte alligned)
        }
         else if (token == FDT_END_NODE) {
            // left the node 
            in_memory = 0;
        }
        else if (token == FDT_PROP){
            // A property -has a length, a name of offset, then data
            uint32 len          = bswap32(*p++);  //how many bytes of data
            uint32 nameoff =  bswap32(*p++); //offset into strings section
            char    *propname = strings + nameoff;  // actual property name

            // Are we inside a memory node AND is this the "reg" property?
            // "regs" holds base address and size of the memory region
            if(in_memory && strncmp(propname, "reg", 3) == 0){
                printf("found reg! p[0]=%x p[1]=%x p[2]=%x p[3]=%x\n", 
                bswap32(p[0]), bswap32(p[1]), bswap32(p[2]), bswap32(p[3]));
                // reg format: <addr_hi addr_lo size_hi_size_lo>
                //Each is a 32-bit big-endian value
                // We combine hi and lo into 64bit values
                uint64 size = ((uint64)bswap32(p[2]) << 32)
                                   | (uint64) bswap32(p[3]);

                 if(size == 0)
                       size = (uint64)bswap32(p[0]);
                 return size; //found it return memory size
            }

            // not what we want skip pass this property's data
            p+= (len +3) /  4;
        } else if (token == FDT_NOP){
            // do nothing just skip
        } else {
            // End of the entire tree stop
            break;
        }
        
    }
    return 0; // didnt find memory node
}