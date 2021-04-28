#include <vector>
#include <string>
#include <stdio.h>
#include "src/main.h"
#include "runs.h"

using namespace std;




int main(int argc, char **argv) {
  string cmd = argv[1];
  if (cmd=="pagerank") runPagerank(argc, argv);
  if (cmd=="traversal-bfs" || cmd=="bfs") runTraversalBfs(argc, argv);
  printf("\n");
  return 0;
}
