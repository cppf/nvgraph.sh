#pragma once
#include <string>
#include <istream>
#include <sstream>
#include <fstream>
#include "DiGraph.h"

using std::string;
using std::istream;
using std::ifstream;
using std::stringstream;
using std::ios;
using std::getline;




// READ-MTX
// --------

template <class G>
void readMtx(G& a, istream& s) {
  // read rows, cols, size
  string ln;
  int r, c, sz;
  do { getline(s, ln); }
  while (ln[0] == '%');
  stringstream ls(ln);
  ls >> r >> c >> sz;

  // read edges (from, to)
  while (getline(s, ln)) {
    int u, v;
    ls = stringstream(ln);
    if (!(ls >> u >> v)) break;
    a.addEdge(u, v);
  }
}

auto readMtx(istream& s) {
  DiGraph<> a; readMtx(a, s);
  return a;
}


template <class G>
void readMtx(G& a, const char *pth) {
  ifstream f(pth);
  stringstream s;
  s << f.rdbuf();
  f.close();
  readMtx(a, s);
}

auto readMtx(const char *pth) {
  DiGraph<> a; readMtx(a, pth);
  return a;
}
