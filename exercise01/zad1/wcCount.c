#include "wcCount.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

blocks *createBlock(int maxBlocks) {
  if (maxBlocks <= 0) {
    return NULL;
  }

  blocks *array = (blocks *)calloc(1, sizeof(blocks));
  if (array == NULL) {
    return NULL;
  }
  array->blockArray = (void **)calloc(maxBlocks, sizeof(void *));
  if (array->blockArray == NULL) {
    free(array);
    return NULL;
  }
  array->maxBlocks = maxBlocks;
  array->blocksNumber = 0;
  return array;
}

int readFileContentToBlock(char *fileName, void **block) {

  FILE *file = fopen(fileName, "r");
  long fileSize;

  if (!file) {
    return FILE_NOT_FOUND;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return FAILURE;
  }

  fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (fileSize < 0) {
    fclose(file);
    return FAILURE;
  }

  *block = calloc(fileSize + 1, sizeof(char));

  if (!block) {
    fclose(file);
    return ALLOCATION_MEMORY_ERROR;
  }

  fread(*block, sizeof(char), fileSize, file);

  fclose(file);

  return SUCCESS;
}

int countWords(char *fileName, struct blocks *blocks) {
  if (!blocks) {
    return FAILURE;
  }

  char tmpfile[] = "/tmp/tmpXXXXXX";
  char command[100];
  int status;

  mkstemp(tmpfile);

  snprintf(command, 100, "wc '%s' > '%s'", fileName, tmpfile);

  status = system(command);

  if (status != 0) {
    return FAILURE;
  }

  if (readFileContentToBlock(
          tmpfile, &blocks->blockArray[blocks->blocksNumber]) != SUCCESS) {
    return FAILURE;
  }

  blocks->blocksNumber++;

  if (remove(tmpfile) != 0) {
    return FILE_REMOVE_ERROR;
  }

  return SUCCESS;
}

void *getBlockContent(struct blocks *blocks, int index) {
  if (!blocks) {
    return NULL;
  }

  if (index < 0 || index >= blocks->blocksNumber) {
    return NULL;
  }
  return blocks->blockArray[index];
}

int shiftBlockArray(struct blocks *blocks, int index) {
  if (!blocks) {
    return FAILURE;
  }

  if (index < 0 || index >= blocks->blocksNumber) {
    return FAILURE;
  }

  for (int i = index; i < blocks->blocksNumber - 1; i++) {
    blocks->blockArray[i] = blocks->blockArray[i + 1];
  }
  blocks->blocksNumber--;
  return SUCCESS;
}

int removeBlock(struct blocks *blocks, int index) {
  if (!blocks) {
    return FAILURE;
  }

  if (index < 0 || index >= blocks->blocksNumber) {
    return FAILURE;
  }
  free(blocks->blockArray[index]);
  shiftBlockArray(blocks, index);
  return SUCCESS;
}

void freeBlockArray(struct blocks *blocks) {
  if (!blocks) {
    return;
  }

  for (int i = 0; i < blocks->blocksNumber; i++) {
    free(blocks->blockArray[i]);
  }
  free(blocks->blockArray);
  blocks->blockArray = NULL;
  blocks->blocksNumber = 0;
  blocks->maxBlocks = 0;
}