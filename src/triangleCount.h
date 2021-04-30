#pragma once
#include <cstdint>
#include <vector>
#include "_nvgraph.h"
#include "vertices.h"
#include "edges.h"

using std::vector;




template <class G>
auto triangleCount(float& t, int T, G& x) {
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSRTopology32I_st csr;
  auto ks    = vertices(x);
  auto vfrom = sourceOffsets(x);
  auto efrom = destinationIndices(x);

  TRY( nvgraphCreate(&h) );
  TRY( nvgraphCreateGraphDescr(h, &g) );

  csr.nvertices = x.order();
  csr.nedges    = x.size();
  csr.source_offsets      = vfrom.data();
  csr.destination_indices = efrom.data();
  TRY( nvgraphSetGraphStructure(h, g, &csr, NVGRAPH_CSR_32) );

  uint64_t count = 0;
  t = measureDuration([&]() { TRY( nvgraphTriangleCount(h, g, &count) ); }, T);

  TRY( nvgraphDestroyGraphDescr(h, g) );
  TRY( nvgraphDestroy(h) );
  return count;
}
