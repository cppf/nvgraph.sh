#pragma once




template <class G, class H>
void lowerTriangular(H& a, G& x) {
  for (auto u : x.vertices()) {
    a.addVertex(u, x.vertexData(u));
    for (auto v : x.edges(u))
      if (u > v) a.addEdge(u, v, x.edgeData(u, v));
  }
}

template <class G>
auto lowerTriangular(G& x) {
  G a; lowerTriangular(a, x);
  return a;
}

template <class G>
void lowerTriangularW(G& a) {
  for (auto u : a.vertices()) {
    for (auto v : a.edges(u))
      if (u <= v) a.removeEdge(u, v);
  }
}
