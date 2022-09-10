# region-fill-dirichlet

Application of the Dirichlet problem to the
filling of a bounded region of an image.

In this project, I produce a document that
explains the theory of applying Laplace's
equation to the filling in of a closed region in
a monochromatic image.  Also, I produce a C++
program to implement the approach described in
the document.

Although I worked out the solution while waiting
for my daughter to finish softball-practice,
when I later searched for this technique, I
found that (not surprising!) this had already
been done.  See [Region Filling and Laplace's
Equation](https://blogs.mathworks.com/steve/2015/06/17/region-filling-and-laplaces-equation/).

Here are some images from that blog post:

![exploring_regionfill_01.png](doc/exploring_regionfill_01.png)
![exploring_regionfill_12.png](doc/exploring_regionfill_12.png)

My contribution is, first of all, to provide a
solution that depends not at all on proprietary
software like Matlab.  Rather, I use C++ and
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page).
Second, I improve upon the approach of Eddins by
using Laplace's equation in order to propagate
high-spatial-frequency noise from the boundary
into the region.

Here are some images from my work:

![trees-mod3.png](doc/trees-mod3.png)
![trees-mod3-cut.png](doc/trees-mod3-cut.png)

## Redesign Under Namespace `dirichlet`

After initial design and implementation as code
under namespace `regfill`, improved design and
implementation is under namespace `dirichlet`.

Design under `dirichlet` internally uses Eigen
more throughly, minimizes copying, solves the
problem separately for each disjoint region, and
does not use `std::map`.

The basic idea is to employ an interface
enabling speed and flexibility. Constructor for
[`dirichlet::Fill`](Fill.hpp) will do very
little copying: It will copy the solution into
the original image only if that image be passed
to constructor by way of non-const pointer.
Otherwise, the solution, only for the specified
pixels, may be read from the instance after
construction.

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

Copyright 2018-2022 Thomas E. Vaughan.  See
terms of redistribution in [LICENSE](LICENSE).

<!--
Narrow textwidth allows editing of file in
cell-phone's browser.

vim: set tw=48:
-->
