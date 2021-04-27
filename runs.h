#pragma once
#include <string>
#include <cstring>
#include <ostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "src/main.h"

using namespace std;




struct RunPageRankOptions {
  string error;
  string input;
  string output;
  string format = "json";
  float  alpha = 0.85;
  float  tolerance = 1e-6;
  int    max_iter = 500;
};




// RUN-PAGERANK
// ------------

void runPageRankParse(RunPageRankOptions& o, int argc, char **argv) {
  stringstream estream;
  for (int i=2; i<argc; i++) {
    string a = argv[i];
    size_t e = a.find('=');
    string k = a.substr(0, e);
    auto v = [&]() { return e==string::npos? argv[i+1] : a.substr(e+1); };
    if (a.find('-')!=0) o.input = a;
    else if (k=="-o" || k=="--output") o.output = v();
    else if (k=="-f" || k=="--format") o.format = v();
    else if (k=="-a" || k=="--alpha") o.alpha = stof(v());
    else if (k=="-t" || k=="--tolerance") o.tolerance = stof(v());
    else if (k=="-i" || k=="--max_iter") o.max_iter = stoi(v());
    else estream << "unknown option \"" << k << "\"";
  }
  o.error = estream.str();
}

auto runPageRankParse(int argc, char **argv) {
  RunPageRankOptions o; runPageRankParse(o, argc, argv);
  return o;
}


string runPageRankVerify(RunPageRankOptions o) {
  stringstream a;
  if (!o.error.empty()) return o.error;
  if (o.input.empty()) return "no input file given";
  if (o.alpha<0 || o.alpha>1) return "alpha must be between 0 and 1";
  if (o.tolerance<1e-10 || o.tolerance>1) return "tolerance must be between 1e-10 and 1";
  if (o.format=="json" || o.format=="yaml") return "";
  a << "unknown format \"" << o.format << "\"";
  return a.str();
}


template <class G, class C>
void runPageRankOutput(RunPageRankOptions o, G& x, float t, C& ranks) {
  stringstream s;
  writeBegin(s, o.format);
  writeValue(s, "graph_file", o.input, o.format);
  writeValue(s, "graph_order", x.order(), o.format);
  writeValue(s, "graph_size", x.size(), o.format);
  writeValue(s, "pagerank_alpha", o.alpha, o.format);
  writeValue(s, "pagerank_tolerance", o.tolerance, o.format);
  writeValue(s, "pagerank_max_iter", o.max_iter, o.format);
  writeValue(s, "result_time_ms", t, o.format);
  writeValues(s, "result_ranks", ranks, o.format);
  writeEnd(s, o.format);
  ofstream f(o.output);
  f << s.rdbuf();
  f.close();
}


void runPageRank(int argc, char **argv) {
  RunPageRankOptions o = runPageRankParse(argc, argv);
  string e = runPageRankVerify(o);
  if (!e.empty()) { printf("error: %s\n", e.c_str()); return; }
  printf("Loading graph %s ...\n", o.input.c_str());
  auto x  = readMtx(o.input.c_str()); print(x);
  auto xt = transposeForNvgraph(x);   print(xt);
  float t; auto ranks = pageRank(t, xt, o.alpha, o.tolerance, o.max_iter);
  printf("[%08.1f ms] nvgraphPagerank\n", t);
  if (!o.output.empty()) runPageRankOutput(o, x, t, ranks);
}
