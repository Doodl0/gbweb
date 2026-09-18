struct memoryBus {
    char memory[0xFFFF];
};

char Memory_ReadByte(struct memoryBus memory, unsigned short address) {
    return memory.memory[address];
}
