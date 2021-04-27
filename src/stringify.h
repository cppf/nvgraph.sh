#pragma once
#include <string>

using std::string;
using std::to_string;




template <class T>
string stringify(T x) {
  return to_string(x);
}

string stringify(string x) {
  string a("\"");
  a += x;
  a += "\"";
  return a;
}
