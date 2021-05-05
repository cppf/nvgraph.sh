#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <sstream>
#include <fstream>
#include "_support.h"
#include "stringify.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::ostream;
using std::ostringstream;
using std::ofstream;




// WRITE
// -----

template <class T>
void write(ostream& a, T *x, int N) {
  a << "{";
  for (int i=0; i<N; i++)
    a << " " << x[i];
  a << " }";
}

template <class T>
void write(ostream& a, vector<T>& x) {
  write(a, x.data(), x.size());
}

template <class K, class T>
void write(ostream& a, unordered_map<K, T>& x) {
  a << "{\n";
  for (auto& p : x)
    a << "  " << p.first << " => " << p.second << "\n";
  a << "}";
}




template <class T>
void write(ostream& a, T *x, int R, int C) {
  a << "{\n";
  for (int r=0; r<R; r++) {
    for (int c=0; c<C; c++)
      a << "  " << GET2D(x, r, c, C);
    a << "\n";
  }
  a << "}";
}

template <class T>
void write(ostream& a, vector<vector<T>>& x) {
  a << "{\n";
  for (auto& v : x) {
    a << "  "; write(a, v);
  }
  a << "}";
}




template <class G>
void write(ostream& a, G& x, bool all=false) {
  a << "order: " << x.order() << " size: " << x.size();
  if (!all) { a << " {}"; return; }
  a << " {\n";
  for (auto u : x.vertices()) {
    a << "  " << u << " ->";
    for (auto v : x.edges(u))
      a << " " << v;
    a << "\n";
  }
  a << "}";
}




// WRITE-FILE
// ----------

void writeFile(const char *pth, string d) {
  ofstream f(pth);
  f << d;
  f.close();
}




// WRITE-MTX
// ---------

template <class G>
void writeMtx(ostream& a, G& x) {
  a << "%%MatrixMarket matrix coordinate real general\n";
  a << x.order() << " " << x.order() << " " << x.size() << "\n";
  for (auto u : x.vertices()) {
    for (auto v : x.edges(u))
      a << u << " " << v << " " << x.edgeData(u) << "\n";
  }
}

template <class G>
void writeMtx(const char *pth, const G& x) {
  string s0; ostringstream s(s0);
  writeMtx(s, x);
  writeFile(pth, s.str());
}




// WRITE-BEGIN
// -----------

void writeBeginYaml(ostream& a) {}

void writeBeginJson(ostream& a) {
  a << "{\n";
}

void writeBegin(ostream& a, string fmt) {
  if (fmt=="yaml") writeBeginYaml(a);
  else writeBeginJson(a);
}




// WRITE-END
// ---------

void writeEndYaml(ostream& a) {}

void writeEndJson(ostream& a) {
  a.seekp(-2, a.cur);
  a << "\n}\n";
}

void writeEnd(ostream& a, string fmt) {
  if (fmt=="yaml") writeEndYaml(a);
  else writeEndJson(a);
}




// WRITE-VALUE
// -----------

template <class T>
void writeValueYaml(ostream& a, string key, T v) {
  if (!key.empty()) a << key << ": ";
  a << stringify(v) << (key.empty()? "\n" : ",\n");
}

template <class T>
void writeValueJson(ostream& a, string key, T v) {
  if (!key.empty()) a << "  \"" << key << "\": ";
  a << stringify(v) << (key.empty()? "\n" : ",\n");
}

template <class T>
void writeValue(ostream& a, string key, T v, string fmt) {
  if (fmt=="yaml") writeValueYaml(a, key, v);
  else writeValueJson(a, key, v);
}




// WRITE-VALUES
// ------------

template <class I>
void writeValuesYaml(ostream& a, string key, I&& vs) {
  if (!key.empty()) a << key << ":\n";
  for (auto v : vs)
    a << "- " << stringify(v) << '\n';
}

template <class I>
void writeValuesJson(ostream& a, string key, I&& vs) {
  if (!key.empty()) a << "  \"" << key << "\": ";
  string pre = key.empty()? "  " : "    ";
  a << "[\n";
  auto b = a.tellp();
  for (auto v : vs)
    a << pre << stringify(v) << ",\n";
  if (a.tellp()!=b) a.seekp(-2, a.cur);
  a << '\n' << pre.substr(2) << (key.empty()? "]\n" : "],\n");
}

template <class I>
void writeValues(ostream& a, string key, I&& vs, string fmt) {
  if (fmt=="yaml") writeValuesYaml(a, key, vs);
  else writeValuesJson(a, key, vs);
}
