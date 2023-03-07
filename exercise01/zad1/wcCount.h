enum status_codes {
  SUCCESS = 0,
  FAILURE = -1,
  FILE_NOT_FOUND = -2,
  FILE_READ_ERROR = -3,
  FILE_REMOVE_ERROR = -4,
  ALLOCATION_MEMORY_ERROR = -5,
  BLOCK_ADD_ERROR = -6,
};

typedef struct blocks {
  void **blockArray;
  int maxBlocks;
  int blocksNumber;
} blocks;

blocks *createBlock(int maxBlocks);
int countWords(char *fileName, struct blocks *blocks);
void *getBlockContent(struct blocks *blocks, int index);
int readFileContentToBlock(char *fileName, void **blocks);
int removeBlock(struct blocks *blocks, int index);
void freeBlockArray(struct blocks *blocks);
int shiftBlockArray(struct blocks *blocks, int index);
