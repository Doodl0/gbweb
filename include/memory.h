struct memoryBus {
    unsigned char memory[0xFFFF];
};

char Memory_ReadByte(struct memoryBus memory, unsigned short address);
