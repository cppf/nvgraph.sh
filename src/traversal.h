#pragma once
#include <vector>
#include "_nvgraph.h"
#include "vertices.h"
#include "edges.h"

using std::vector;




template <class C>
struct TraversalBfsResult {
  C distances;
  C predecessors;
};

template <class C>
auto traversalBfsResult(C dists, C preds) {
  return TraversalBfs<C> {dists, preds};
}




template <class G, class K>
auto traversalBfs(float& t, G& x, K u) {
  nvgraphHandle_t     h;
  nvgraphGraphDescr_t g;
  struct nvgraphCSRTopology32I_st csr;
  vector<cudaDataType_t> vtype {CUDA_R_32I, CUDA_R_32I};
  vector<int> dists(x.order());
  vector<int> preds(x.order());
  auto ks    = vertices(x);
  auto vfrom = sourceOffsets(x);
  auto efrom = destinationIndices(x);

  TRY( nvgraphCreate(&h) );
  TRY( nvgraphCreateGraphDescr(h, &g) );

  csr.nvertices = x.order();
  csr.nedges    = x.size();
  csr.destination_offsets = vfrom.data();
  csr.source_indices      = efrom.data();
  TRY( nvgraphSetGraphStructure(h, g, &csr, NVGRAPH_CSR_32) );
  TRY( nvgraphAllocateVertexData(h, g, vtype.size(), vtype.data()) );

  nvgraphTraversalParameter_t p;
  nvgraphTraversalParameterInit(&p);
  nvgraphTraversalSetDistancesIndex(&p, 0);
  nvgraphTraversalSetPredecessorsIndex(&p, 1);
  nvgraphTraversalSetUndirectedFlag(&p, false);

  t = measureDuration([&]() { TRY( nvgraphTraversal(h, g, NVGRAPH_TRAVERSAL_BFS, &u, p) ); });
  TRY( nvgraphGetVertexData(h, g, dists.data(), 0) );
  TRY( nvgraphGetVertexData(h, g, preds.data(), 1) );

  TRY( nvgraphDestroyGraphDescr(h, g) );
  TRY( nvgraphDestroy(h) );
  return traversalBfsResult(
    vertexContainer(x, dists, ks),
    vertexContainer(x, preds, ks)
  );
}
