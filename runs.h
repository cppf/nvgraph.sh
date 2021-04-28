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

struct RunTraversalBfsOptions : public RunOptions {
  int source = 1;
};

struct RunTriangleCountOptions : public RunOptions {};




// RUN-ERROR
bool runError(string& e) {
  if (e.empty()) return false;
  cerr << "error: " << e << '\n';
  return true;
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

void runTraversalBfsParse(RunTraversalBfsOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [&](auto k, auto v) {
    if (k=="-s" || k=="--source") o.source = stoi(v());
    else return false;
    return true;
  });
}

void runTriangleCountParse(RunTriangleCountOptions& o, int argc, char **argv) {
  runParse(o, argc, argv, [](auto k, auto v) { return false; });
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
string runTraversalBfsVerify(RunTraversalBfsOptions& o, G& x) {
  stringstream s;
  string e = runVerify(o);
  if (!e.empty()) return e;
  if (!x.hasVertex(o.source)) {
    s << "source vertex \"" << o.source << "\" not in graph";
    return s.str();
  }
  return "";
}




// RUN-*-OUTPUT
// ------------

template <class G, class F>
void runOutput(RunOptions& o, G& x, F fn) {
  stringstream s;
  writeBegin(s, o.format);
  writeValue(s, "input", o.input,   o.format);
  writeValue(s, "order", x.order(), o.format);
  writeValue(s, "size",  x.size(),  o.format);
  fn(s);
  writeEnd(s, o.format);
  ofstream f(o.output);
  f << s.rdbuf();
  f.close();
}

template <class G, class C>
void runPagerankOutput(RunPagerankOptions& o, G& x, float t, C& ranks) {
  runOutput(o, x, [&](auto& s) {
    writeValue (s, "alpha",     o.alpha,     o.format);
    writeValue (s, "tolerance", o.tolerance, o.format);
    writeValue (s, "max_iter",  o.max_iter,  o.format);
    writeValue (s, "time_ms",   t,           o.format);
    writeValues(s, "ranks",     ranks,       o.format);
  });
}

template <class G, class C>
void runTraversalBfsOutput(RunTraversalBfsOptions& o, G& x, float t, C& dists, C& preds) {
  runOutput(o, x, [&](auto& s) {
    writeValue (s, "source",       o.source, o.format);
    writeValues(s, "distances",    dists,    o.format);
    writeValues(s, "predecessors", preds,    o.format);
  });
}

template <class G>
void runTriangleCountOutput(RunTriangleCountOptions& o, G& x, float t, uint64_t count) {
  runOutput(o, x, [&](stringstream& s) {
    writeValue(s, "count", count, o.format);
  });
}




// RUN-*
// -----

void runPagerank(int argc, char **argv) {
  RunPagerankOptions o; string e; float t;
  runPagerankParse(o, argc, argv);
  runPagerankVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x  = readMtx(o.input.c_str()); print(x);
  auto xt = transposeForNvgraph(x);   print(xt);
  auto ranks = pagerank(t, xt, o.alpha, o.tolerance, o.max_iter);
  printf("[%.1f ms] nvgraphPagerank\n", t);
  if (!o.output.empty()) runPagerankOutput(o, x, t, ranks);
}


void runTraversalBfs(int argc, char **argv) {
  RunTraversalBfsOptions o; string e; float t;
  runTraversalBfsParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x = readMtx(o.input.c_str()); print(x);
  e = runTraversalBfsVerify(o, x);
  if (runError(e)) return;
  auto [dists, preds] = traversalBfs(t, x, o.source);
  printf("[%.1f ms] nvgraphTraversalBfs\n", t);
  if (!o.output.empty()) runTraversalBfsOutput(o, x, t, dists, preds);
}


void runTriangleCount(int argc, char **argv) {
  RunTriangleCountOptions o; string e; float t;
  runTriangleCountParse(o, argc, argv);
  e = runVerify(o);
  if (runError(e)) return;
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x = readMtx(o.input.c_str()); print(x);
  uint64_t count = triangleCount(t, x);
  printf("[%.1f ms] nvgraphTriangleCount\n", t);
  if (!o.output.empty()) runTriangleCountOutput(o, x, t, count);
}
