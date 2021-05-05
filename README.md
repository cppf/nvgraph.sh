CLI for nvGraph, which is a GPU-based graph analytics library written by
NVIDIA, using CUDA.<br>
:package: [NPM](https://www.npmjs.com/package/nvgraph.sh),
:smiley_cat: [GitHub](https://github.com/orgs/nodef/packages?repo_name=nvgraph.sh),
:scroll: [Files](https://unpkg.com/nvgraph.sh/),
:blue_book: [Wiki](https://github.com/nodef/nvgraph.sh/wiki/).

This is for running **nvGraph** functions right from the CLI with graphs in
MatrixMarket format (.mtx) directly. It just needs a x86_64 linux machine
with NVIDIA GPU drivers installed. Execution time, along with the results
can be saved in JSON/YAML file. The [packaging code] is written in Node.js.
You can install this with `npm install -g nvgraph.sh`.

<br>
<br>


## References

- [nvGraph pagerank example, EN605.617, JHU-EP-Intro2GPU](https://github.com/JHU-EP-Intro2GPU/EN605.617/blob/master/module9/nvgraph_examples/nvgraph_Pagerank.cpp)
- [nvGraph pagerank example, CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/archive/10.0/nvgraph/index.html#nvgraph-pagerank-example)
- [nvGraph Library User's Guide](https://docs.nvidia.com/cuda/archive/10.1/pdf/nvGRAPH_Library.pdf)
- [RAPIDS nvGraph NVIDIA graph library](https://github.com/rapidsai/nvgraph)

<br>
<br>

[![](https://img.youtube.com/vi/3s9psf01ldo/maxresdefault.jpg)](https://www.youtube.com/watch?v=3s9psf01ldo)

[packaging code]: https://github.com/nodef/nvgraph.sh
