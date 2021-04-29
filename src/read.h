#pragma once
#include <string>
#include <istream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <type_traits>
#include "_support.h"
#include "DiGraph.h"

using std::string;
using std::istream;
using std::ifstream;
using std::stringstream;
using std::ios;
using std::getline;
using std::equal;
using std::is_arithmetic;




// READ-MTX
// --------

template <class G>
void readMtx(G& a, istream& s) {
  using E = typename G::TEdge;
  // read rows, cols, size
  string ln;
  bool sym = false;
  while (1) {
    getline(s, ln);
    if (ln.find('%')!=0) break;
    if (ln.find("%%")!=0) continue;
    sym = ln.rfind("asymmetric")==string::npos;
  }
  int r, c, sz;
  stringstream ls(ln);
  ls >> r >> c >> sz;

  // read edges (from, to)
  while (getline(s, ln)) {
    int u, v; E w = E();
    stringstream ls(ln);
    if (!(ls >> u >> v)) break;
    if (is_arithmetic<E>::value) ls >> w;
    a.addEdge(u, v, w);
    if (sym) a.addEdge(v, u, w);
  }
}

auto readMtx(istream& s) {
  DiGraph<int, NONE, float> a; readMtx(a, s);
  return a;
}


template <class G>
void readMtx(G& a, const char *pth) {
  ifstream f(pth);
  string s0; stringstream s(s0);
  s << f.rdbuf();
  f.close();
  readMtx(a, s);
}

auto readMtx(const char *pth) {
  DiGraph<int, NONE, float> a; readMtx(a, pth);
  return a;
}
