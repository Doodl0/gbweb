struct memoryBus {
    unsigned char memory[0xFFFF];
};

unsigned char Memory_ReadByte(struct memoryBus memory, unsigned short address);
