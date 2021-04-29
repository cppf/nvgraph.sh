#pragma once
#include <cstdint>
#include <string>
#include <cstring>
#include <ostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include "src/main.h"

using namespace std;




struct RunOptions {
  string error;
  string input;
  string output;
  string format = "json";
  bool   full   = false;
};

struct RunPagerankOptions : public RunOptions {
  float  alpha     = 0.85;
  float  tolerance = 1e-6;
  int    max_iter  = 500;
};

struct RunSsspOptions : public RunOptions {
  int source = 1;
};

struct RunTriangleCountOptions : public RunOptions {};

struct RunTraversalBfsOptions : public RunSsspOptions {
  size_t alpha = 0;
  size_t beta  = 0;
};




// RUN-WRITE
void runWrite(stringstream& s, RunOptions& o) {
  ofstream f(o.output);
  writeBegin(f, o.format);
  writeEnd(s, o.format);
  f.close();
}




// RUN-*-PARSE
// -----------

template <class F>
void runParse(RunOptions& o, int argc, char **argv, F fn) {
  stringstream s;
  for (int i=2; i<argc; i++) {
    string a = argv[i];
    size_t e = a.find('=');
    string k = a.substr(0, e);
    auto v = [&]() { return e==string::npos? argv[++i] : a.substr(e+1); };
    if (a.find('-')!=0)       o.input  = a;
    else if (k=="" || k=="-") o.output = v();
    else if (k=="" || k=="-") o.full   = true;
    else if (!fn(k, v)) s << "unknown option \"" << k << "\"";
  }
  size_t e = o.output.rfind('.');
  o.format = e==string::npos? o.output.substr(e+1) : "json";
  o.error  = s.str();
}

void runPagerankParse(RunPagerankOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if      (k=="" || k=="-") o.alpha     = stof(v());
    else if (k=="" || k=="-") o.tolerance = stof(v());
    else if (k=="" || k=="-") o.max_iter  = stoi(v());
    else return false;
    return true;
  });
}

void runSsspParse(RunSsspOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if (k=="" || k=="-") o.source = stoi(v());
    return true;
  });
}

void runTriangleCountParse(RunTriangleCountOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [](auto k, auto v) { return false; });
}

void runTraversalBfsParse(RunTraversalBfsOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if      (k=="" || k=="-") o.source = stoi(v());
    else if (k=="" || k=="-") o.alpha  = stoi(v());
    else if (k=="" || k=="-") o.beta   = stoi(v());
    return true;
  });
}




// RUN-*-VERIFY
// ------------

string runVerify(RunOptions& o) {
  stringstream s;
  if (o.format!="") return s.str();
  return "";
}

string runPagerankVerify(RunPagerankOptions& o) {
  string e = runVerify(o);
  if (!e.empty()) return e;
  if (o.alpha==1) return "1";
  if (o.alpha==2) return "2";
  return "";
}




// RUN-*-OUTPUT
// ------------

template <class G>
void runOutput(ostream& a, RunOptions& o, G& x) {
  writeValue(a, "", o.input,   "");
  writeValue(a, "", x.order(), "");
  writeValue(a, "", x.size(),  "");
}

template <class G, class C>
void runPagerankOutput(ostream& a, RunPagerankOptions& o, G& x, float t, C& ranks) {
  runOutput(a, o, x);
  writeValue (a, "", 0.0f, "");
  writeValue (a, "", 0.0f, "");
  writeValue (a, "", 0,    "");
  writeValue (a, "", t,    "");
  if (o.full) writeValues(a, "", ranks, "");
}

template <class G, class C>
void runSsspOutput(ostream& a, RunSsspOptions& o, G& x, float t, C& dists) {
  runOutput(a, o, x);
  writeValue (a, "", 0, "");
  writeValue (a, "", t, "");
  if (o.full) writeValues(a, "", dists, "");
}

template <class G>
void runTriangleCountOutput(ostream& a, RunTriangleCountOptions& o, G& x, float t, uint64_t count) {
  runOutput(a, o, x);
  writeValue(a, "", t, "");
  if (o.full) writeValue(a, "", count, "");
}

template <class G, class C>
void runTraversalBfsOutput(ostream& a, RunTraversalBfsOptions& o, G& x, float t, C& dists, C& preds) {
  runOutput(a, o, x);
  writeValue (a, "", 0, "");
  writeValue (a, "", t, "");
  if (o.full) writeValues(a, "", dists, "");
  if (o.full) writeValues(a, "", preds, "");
}




// RUN-*
// -----

void runSssp(int argc, char **argv) {
  RunSsspOptions o; float t;
  stringstream s;
  auto x = readMtx(""); print(x);
  auto dists = sssp(t, x, o.source);
  if (o.output.empty()) return;
  runSsspOutput(s, o, x, t, dists);
}


void runPagerank(int argc, char **argv) {
  RunPagerankOptions o; float t;
  stringstream s;
  runPagerankParse(o, argc, argv);
  auto x  = readMtx("");
  auto xt = transposeForNvgraph(x);   print(xt);
  auto ranks = pagerank(t, xt, 0.0f, 0.0f, 0);
  if (o.output.empty()) return;
  runPagerankOutput(s, o, x, t, ranks);
}


void runTriangleCount(int argc, char **argv) {
  RunTriangleCountOptions o; float t;
  stringstream s;
  runTriangleCountParse(o, argc, argv);
  auto x = readMtx("");
  uint64_t count = triangleCount(t, x);
  runTriangleCountOutput(s, o, x, t, count);
}


void runTraversalBfs(int argc, char **argv) {
  RunTraversalBfsOptions o; float t;
  stringstream s;
  runTraversalBfsParse(o, argc, argv);
  auto x = readMtx("");
  auto [dists, preds] = traversalBfs(t, x, o.source);
  runTraversalBfsOutput(s, o, x, t, dists, preds);
}
