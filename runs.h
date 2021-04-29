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
};

struct RunPagerankOptions : public RunOptions {
  float  alpha = 0.85;
  float  tolerance = 1e-6;
  int    max_iter = 500;
};

struct RunSsspOptions : public RunOptions {
  int source = 1;
};

typedef RunOptions RunTriangleCountOptions;
typedef RunSsspOptions RunTraversalBfsOptions;




// RUN-ERROR
bool runError(string& e) {
  if (e.empty()) return false;
  cerr << "error: " << e << '\n';
  return true;
}


// RUN-WRITE
void runWrite(stringstream& s, RunOptions& o) {
  ofstream f(o.output);
  writeBegin(f, o.format);
  f << s.rdbuf();
  writeEnd(s, o.format);
  f.close();
}




// RUN-*-PARSE
// -----------

template <class F>
void runParse(RunOptions& o, int argc, char **argv, F fn) {
  stringstream estream;
  for (int i=2; i<argc; i++) {
    string a = argv[i];
    size_t e = a.find('=');
    string k = a.substr(0, e);
    auto v = [&]() { return e==string::npos? argv[++i] : a.substr(e+1); };
    if (a.find('-')!=0)                o.input = a;
    else if (k=="-o" || k=="--output") o.output = v();
    else if (k=="-f" || k=="--format") o.format = v();
    else if (!fn(k, v)) estream << "unknown option \"" << k << "\"";
  }
  o.error = estream.str();
}

void runPagerankParse(RunPagerankOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if      (k=="-a" || k=="--alpha")     o.alpha     = stof(v());
    else if (k=="-t" || k=="--tolerance") o.tolerance = stof(v());
    else if (k=="-i" || k=="--max_iter")  o.max_iter  = stoi(v());
    else return false;
    return true;
  });
}

void runSsspParse(RunSsspOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if (k=="-s" || k=="--source") o.source = stoi(v());
    else return false;
    return true;
  });
}

void runTriangleCountParse(RunTriangleCountOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [](auto k, auto v) { return false; });
}

void runTraversalBfsParse(RunTraversalBfsOptions& o, int argc, char **argv) {
  runSsspParse(o, argc, argv);
}




// RUN-*-VERIFY
// ------------

string runVerify(RunOptions& o) {
  stringstream s;
  if (!o.error.empty()) return o.error;
  if (o.input.empty()) return "no input file given";
  if (!(o.format=="json" || o.format=="yaml")) {
    s << "unknown format \"" << o.format << "\"";
    return s.str();
  }
  return "";
}

string runPagerankVerify(RunPagerankOptions& o) {
  string e = runVerify(o);
  if (!e.empty()) return e;
  if (o.alpha<0 || o.alpha>1) return "alpha must be between 0 and 1";
  if (o.tolerance<1e-10 || o.tolerance>1) return "tolerance must be between 1e-10 and 1";
  return "";
}

template <class G>
string runSsspVerify(RunSsspOptions& o, G& x) {
  stringstream s;
  string e = runVerify(o);
  if (!e.empty()) return e;
  if (!x.hasVertex(o.source)) {
    s << "source vertex \"" << o.source << "\" not in graph";
    return s.str();
  }
  return "";
}

template <class G>
string runTraversalBfsVerify(RunTraversalBfsOptions& o, G& x) {
  return runSsspVerify(o, x);
}




// RUN-*-OUTPUT
// ------------

template <class G>
void runOutput(ostream& a, RunOptions& o, G& x) {
  writeValue(a, "input", o.input,   o.format);
  writeValue(a, "order", x.order(), o.format);
  writeValue(a, "size",  x.size(),  o.format);
}

template <class G, class C>
void runPagerankOutput(ostream& a, RunPagerankOptions& o, G& x, float t, C& ranks) {
  runOutput(a, o, x);
  writeValue (a, "alpha",     o.alpha,     o.format);
  writeValue (a, "tolerance", o.tolerance, o.format);
  writeValue (a, "max_iter",  o.max_iter,  o.format);
  writeValue (a, "time_ms",   t,           o.format);
  writeValues(a, "ranks",     ranks,       o.format);
}

template <class G, class C>
void runSsspOutput(ostream& a, RunSsspOptions& o, G& x, float t, C& dists) {
  runOutput(a, o, x);
  writeValue (a, "source",    o.source, o.format);
  writeValue (a, "time_ms",   t,        o.format);
  writeValues(a, "distances", dists,    o.format);
}

template <class G>
void runTriangleCountOutput(ostream& a, RunTriangleCountOptions& o, G& x, float t, uint64_t count) {
  runOutput(a, o, x);
  writeValue(a, "time_ms", t,     o.format);
  writeValue(a, "count",   count, o.format);
}

template <class G, class C>
void runTraversalBfsOutput(ostream& a, RunTraversalBfsOptions& o, G& x, float t, C& dists, C& preds) {
  runOutput(a, o, x);
  writeValue (a, "source",       o.source, o.format);
  writeValue (a, "time_ms",      t,        o.format);
  writeValues(a, "distances",    dists,    o.format);
  writeValues(a, "predecessors", preds,    o.format);
}




// RUN-*
// -----

void runSssp(int argc, char **argv) {
  RunSsspOptions o; float t;
  stringstream s; string e;
  runSsspParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x = readMtx(o.input.c_str()); print(x);
  e = runSsspVerify(o, x);
  if (runError(e)) return;
  auto dists = sssp(t, x, o.source);
  printf("[%.1f ms] nvgraphSssp\n", t);
  if (o.output.empty()) return;
  runSsspOutput(s, o, x, t, dists);
  runWrite(s, o);
}


void runPagerank(int argc, char **argv) {
  RunPagerankOptions o; float t;
  stringstream s; string e;
  runPagerankParse(o, argc, argv);
  runPagerankVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x  = readMtx(o.input.c_str()); print(x);
  auto xt = transposeForNvgraph(x);   print(xt);
  auto ranks = pagerank(t, xt, o.alpha, o.tolerance, o.max_iter);
  printf("[%.1f ms] nvgraphPagerank\n", t);
  if (o.output.empty()) return;
  runPagerankOutput(s, o, x, t, ranks);
  runWrite(s, o);
}


void runTriangleCount(int argc, char **argv) {
  RunTriangleCountOptions o; float t;
  stringstream s; string e;
  runTriangleCountParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x = readMtx(o.input.c_str()); print(x);
  uint64_t count = triangleCount(t, x);
  printf("[%.1f ms] nvgraphTriangleCount\n", t);
  runTriangleCountOutput(s, o, x, t, count);
  runWrite(s, o);
}


void runTraversalBfs(int argc, char **argv) {
  RunTraversalBfsOptions o; float t;
  stringstream s; string e;
  runTraversalBfsParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x = readMtx(o.input.c_str()); print(x);
  e = runTraversalBfsVerify(o, x);
  if (runError(e)) return;
  auto [dists, preds] = traversalBfs(t, x, o.source);
  printf("[%.1f ms] nvgraphTraversalBfs\n", t);
  if (o.output.empty()) return;
  runTraversalBfsOutput(s, o, x, t, dists, preds);
  runWrite(s, o);
}
