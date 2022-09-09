# Redesign Under Namespace `dfill`

After initial design and implementation as code
under namespace `regfill`, improved design and
implementation is under namespace `dfill`.

Design under dfill internally uses Eigen more
throughly, solves the problem separately for
each disjoint region, and does not use
`std::map`.

The basic idea is to have a pointer-based
interface.  Constructor for
[`dfill::Image`](Image.hpp) will produce copy of
original image but with indicated region(s)
filled.

Constructor takes
- pointer to initial image,
- dimensions of image,
- 1-d array of coordinates for pixels to be filled.

Internal allocations include:
- 2-d array of int32_t with same size as image.
  - Each element contains -1 or offset of pixel
    in array of coordinates indicating pixels to
    be filled.
- 2-d array of `float` (or `float[2]` or
  `float[3]`) with copy of image except for
  filled regions,
- 1-d array of records (of size TBD) with same
  length as array of coordinates.

<!--
Narrow textwidth allows editing of file in
cell-phone's browser.

vim: set tw=48:
-->
