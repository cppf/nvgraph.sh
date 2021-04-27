#pragma once
#include <vector>
#include "_nvgraph.h"
#include "vertices.h"
#include "edges.h"

using std::vector;




template <class G>
auto pageRank(float& t, G& x, float p=0.85f, float E=1e-6f) {
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSCTopology32I_st csc;
  vector<cudaDataType_t> vtype {CUDA_R_32F, CUDA_R_32F};
  vector<cudaDataType_t> etype {CUDA_R_32F};
  vector<float> ranks(x.order());
  auto ks    = vertices(x);
  auto vfrom = sourceOffsets(x);
  auto efrom = destinationIndices(x);
  auto vdata = vertexData(x);
  auto edata = edgeData(x);

  TRY( nvgraphCreate(&h) );
  TRY( nvgraphCreateGraphDescr(h, &g) );

  csc.nvertices = x.order();
  csc.nedges    = x.size();
  csc.destination_offsets = vfrom.data();
  csc.source_indices      = efrom.data();
  TRY( nvgraphSetGraphStructure(h, g, &csc, NVGRAPH_CSC_32) );

  TRY( nvgraphAllocateVertexData(h, g, vtype.size(), vtype.data()) );
  TRY( nvgraphAllocateEdgeData  (h, g, etype.size(), etype.data()) );
  TRY( nvgraphSetVertexData(h, g, vdata.data(), 0) );
  TRY( nvgraphSetVertexData(h, g, ranks.data(), 1) );
  TRY( nvgraphSetEdgeData  (h, g, edata.data(), 0) );

  t = measureDuration([&]() { TRY( nvgraphPagerank(h, g, 0, &p, 0, 0, 1, E, 0) ); });
  TRY( nvgraphGetVertexData(h, g, ranks.data(), 1) );

  TRY( nvgraphDestroyGraphDescr(h, g) );
  TRY( nvgraphDestroy(h) );
  return vertexContainer(x, ranks, ks);
}
