# region-fill-dirichlet

The new design is now the recommended default.

The test-code for the old design has the neat
feature of propagating noise from the boundary
into the interior, but I have not reproduced
that in the test-code for the new design.

## New Under Namespace `dirichlet`

Redesigned code requires at least

- Eigen-3.4
- Catch2-3.0 (for unit-tests)

Design under `dirichlet` internally uses Eigen
throughly (almost no for-loops), minimizes
copying, and does not use `std::map`, as old
design did for building coefficients for
sparse matrix.

The basic idea is to employ an interface
enabling speed and flexibility.  The constructor
for
[`dirichlet::Fill`](include/dirichlet/Fill.hpp)
and the constructed function-object will do very
little copying.  The function that solves the
Dirichlet-problem will copy the solution back
into the original image only if that image be
passed to constructor by way of non-const
pointer.  Anyway, the solution, only for the
specified pixels, is returned by the
function-object.

Speed with `-O3` is much faster than that of
old design with `-O3`.

Here are a mask, the original image, and the
correspdonding, filled image produced by
implementation of new design:

![gray.png](test/gray.png)
![mask.png](test/mask.png)
![gray-filled.png](test/gray-filled.png)

## Idea for Even Faster Performance

In a large, filled region, the gradient of the
pixel-values is very smooth in the vicinity of
every filled pixel that is farther than a
handfull of pixels from the border of the filled
region.  This suggests a triangle-based approach
because a triangle allows for natural, linear
interpolation of a scalar field defined at its
vertices.  One need only find a reasonable
method for picking a sparse set of points in the
interior of the region.

### One Possible Sparse Set

First, consider every successive, binned image
of the original image.  First 2x2, then 4x4,
then 8x8, etc.  In each case, the superpixel in
the binned image might be imagined to contain
the mean of the four corresponding pixel-values
in the image at the next higher stage of
resolution, but that doesn't matter because we
care here only about the corner-coordinates of
each pixel at each stage of binning.

Then, for any given region, begin with the
binning level at which at least one superpixel
fits entirely within the region to be filled.
Save the coordinates of the corners of each such
superpixel.

Then advance to the next higher resolution, and
find all of the superpixels outside those
already found but completely within the region
to be filled.  Save the coordinates of each such
superpixel.

At the last stage in this process, the corners
of the highest-resolution pixels are saved.

At any stage in the process, if a binned pixel
lie along a straight section of the boundary,
include the region-facing corners of the
boundary-pixels.

After all of the pixel-corners are saved, the
number of points will be far small than the
number of pixels in the region when the region
be large.

### Triangulation

Perform a Delaunay triangulation on the points,
and solve the Dirichlet problem on the
triangles.  This will be much faster than
solving for the full pixel-grid because the
number of solution-points (corner-points) will
be much smaller than for the full pixel-grid
whenever the region to be filled is large.

At the end, texture-map the triangle-vertex
values back onto the pixel-grid.

## Old Design Under Namespace `regfill`

Application of the Dirichlet problem to the
filling of a bounded region of an image.

Although I worked out the solution while waiting
for my daughter to finish softball-practice,
when I later searched for this technique, I
found that (not surprising!) this had already
been done.  See [Region Filling and Laplace's
Equation](https://blogs.mathworks.com/steve/2015/06/17/region-filling-and-laplaces-equation/).

Here are some images from that blog post:

![exploring_regionfill_01.png](old/exploring_regionfill_01.png)
![exploring_regionfill_12.png](old/exploring_regionfill_12.png)

My contribution is, first of all, to provide a
solution that depends not at all on proprietary
software like Matlab.  Rather, I use C++ and
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page).
Second, I improve upon the approach of Eddins by
using Laplace's equation in order to propagate
high-spatial-frequency noise from the boundary
into the region.

Here are some images from implementation of old
design:

![trees-mod3.png](old/trees-mod3.png)
![trees-mod3-cut.png](old/trees-mod3-cut.png)

## Copyright

Copyright 2018-2022 Thomas E. Vaughan.  See
terms of redistribution in [LICENSE](LICENSE).

<!--
Narrow textwidth allows editing of file in
cell-phone's browser.

vim: set tw=48:
-->
