#pragma once
#include <sys/stat.h>




bool existsFile(const char *pth) {
  struct stat s;
  return stat(pth, &s) == 0;
}
