<!-- vim: set filetype=none:
  Turn off markdown-type because LaTeX-notation
  doesn't work with markdown-mode.
  -->

# region-fill-dirichlet

The new design is now the recommended default.

The test-code for the old design has the neat
feature of propagating noise from the boundary
into the interior, but I have not reproduced
that in the test-code for the new design.

## Current Design Under Namespace `dirichlet`

Current code requires at least

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

Conjugate-gradient solution is now available as
final, optional argument to constructor.  It is
faster than Cholesky to construct instance of
`Fill` on my test-image, but it is slower than
Cholesky to solve.  Overall, CG is slower than
Cholesky on my test-image.  It is turned off by
default for the moment.  Nevertheless,
unit-tests all use conjugate-gradient and look
OK.

Here are the original image, the mask, the
filled image produced by implementation of new
design, and a histogram-equalized image zoomed
in on the central, filled circle:

![gray.png](test/gray.png)
![mask.png](test/mask.png)
![gray-filled.png](test/gray-filled.png)
![gray-filled-zoom-eq.png](test/gray-filled-zoom-eq.png)

## Idea for Yet More Speed in Future Version

Suppose that "deep" and "shallow" are taken to
refer to proximity to the boundary of the region
to be filled.  The deepest pixels are the ones
farthest from the boundary of the region to be
filled, and the shallowest are the ones nearest
the boundary.

In a large region filled according to Laplace's
equation, the pixel-values are most smoothly
distributed across deepest portions of the
region.

Because of the underlying geometry of the
pixel-grid, an approach based on the bilinear
interpolation over a square group of pixels
seems natural.  Although the bilinear
interpolant is linear along each
coordinate-axis, it is quadratic along any other
direction.  Anyway, the bilinear interpolant
satisfies Laplace's equation.  So, if the
corners of the square participate in a global
solution to Laplace's equation, then the
interpolant provides a local solution that is at
least continuous with the global one.

What I propose is a hierarchical approach, in
which the deepest portions of the region to be
filled have the largest interpolated squares,
shallower portions have smaller interpolated
squares, and, in the shallowest portions, every
pixel participates in the global, finite-element
solution to Laplace's equation.

### Prepare Mask

Consider a mask $M$, which is the same size $W
\times H$ as the original image but with value
of 1 at each pixel to be filled and 0 at each
pixel not to be filled.  Extend $M$ to a larger
mask $M_0$ by appending zeros toward the lower
right of $M$ until the width $W_0$ of $M_0$ is
the smallest power of two greater than or equal
to $W$; similarly, for $H_0$ and $H$.

### Find Squares Over Which To Interpolate

Construct every successive, binned image of
$M_0.$  First $M_1$, which has $W_1=W_0/2$, has
$H_1=H_0/2$, and is $2 \times 2$-binned; then
$M_2$, $4 \times 4$; $M_3$, $8 \times 8$; etc.
In each case, a superpixel contains the sum of
the four corresponding pixel-values in the mask
at the next higher stage of resolution.
Construct $M_1, M_2, \ldots, M_k$, where $k$ is
the largest value such that both $W_k \geq 4$
and $H_k \geq 4$.

After construction of the binned mask-images,
consider them, beginning with $M_k,$ in reverse
order.

For each image $M_i$ in $M_k, M_{k-1}, \ldots,
M_2,$ find every pixel $p$ whose value is
$2^{2i}$ and for which every one of $p$'s four
neighbors also has value $2^{2i}$.  Record the
corner-pixels of $p$ in the unbinned image as to
be solved for with the ordinary, two-dimensional
Laplacian.  Record the edge-pixels along each
horizontal or vertical line connecting the
corner-pixels as to be solved for with the
one-dimensional Laplacian.  Mark the interior
pixels in the unbinned image as to be
interpolated from the corners.  Mark every
mask-pixel corresponding to $p$ at every lower
binning as zero.

Mark every remaining pixel in unbinned mask
according to its being and interior pixel or
next to a boundary.

### Prepare Linear Model

When the size of the region to be filled is
larger than, say, $128 \times 128$ pixels, the
elimination of the interior pixels from each
interpolable superpixel drastically reduces the
size of the linear problem.  The time to factor
the matrix and to render the solution should be
greatly reduced for any sufficiently large
region of pixels to be filled.

### Bilinear Interpolation

After the problem is solved, the edges and
interior of each interpolable superpixel must be
filled with the values obtained by bilinearly
interpolating the corner-values, which were
solved for.

Because the bilinear interpolant is a
low-polynimal-order solution to Laplace's
equation over the coordinates of the grid, what
this approach does is, while solving Laplace's
equation, to mandate a low-order solution over
large portions of the deep interior of the
filled region.  But this is precisely where the
solution is likely to be of low order anyway,
and so a large increase in speed can be obtained
with minimal error relative to the full
solution.

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
