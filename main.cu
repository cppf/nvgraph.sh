#include <vector>
#include <stdio.h>
#include "src/main.h"

using namespace std;




template <class G>
void runPageRank(G& x, bool all) {
  float t;
  auto ranks = pageRank(t, x);
  printf("[%07.1f ms] pageRank\n", t); if (all) print(ranks);
}


int main(int argc, char **argv) {
  DiGraph<> g;
  DiGraph<int, float, float> h;
  char *file = argv[1];
  bool all = argc > 2;

  printf("Loading graph %s ...\n", file);
  readMtx(file, g);
  print(g);
  transposeForNvgraph(g, h);
  print(h);
  runPageRank(h, all);
  printf("\n");
  return 0;
}
