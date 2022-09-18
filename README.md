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

Here are the original image, the mask, the
filled image produced by implementation of new
design, and a histogram-equalized image zoomed
in on the central, filled circle:

![gray.png](test/gray.png)
![mask.png](test/mask.png)
![gray-filled.png](test/gray-filled.png)
![gray-filled-zoom-eq.png](test/gray-filled-zoom-eq.png)

## Idea for Even Faster Performance

In a large, filled region, the gradient of the
pixel-values tends toward planar smoothness in
the vicinity of every filled pixel that is
farther than a handfull of pixels from the
border of the filled region.  This suggests an
approach based on linear interpolation.  The
intrinsic geometry of the underlying pixel-grid
suggests interpolation over squares.

### Prepare Mask

Consider a mask $M$, which is the same size
$W \times H$ as the original image but with
value of 1 at each pixel to be filled and 0 at
each pixel not to be filled.  Make a new mask
$M'$ by copying $M$ and then resetting to zero
every pixel that lies within some marginal
distance $m$ pixels from a 0 along either the
row-direction or the column-direction.  Extend
$M'$ to a larger mask $M_0$ by appending zeros
toward the lower right of $M'$ until the width
$W_0$ of $M_0$ is the smallest power of two
greater than or equal to $W$; similarly, for
$H_0$ and $H$.

### Find Squares Over Which To Interpolate

Consider every successive, binned image of
$M_0$.  First $M_1$, which is
$2 \times 2$-binned, then $M_2$, $4 \times 4$;
$M_3$, $8 \times 8$; etc.  In each case, the
superpixel contains the sum of the four
corresponding pixel-values in the mask at the
next higher stage of resolution.  Find the
greatest $N$ such that $M_N$ has at least one
superpixel whose value is $2^{2N}$; that is, in
$M_N$ at least one superpixel whose every
corresponding pixel in $M'$ is 1.

If $N>1$, then, for each such superpixel in
$M_N$, let the four corresponding corner-pixels
in $M'$ remain set to 1, but set the other
$N \times N - 4$ corresponding pixels in $M'$ to
zero.  Keep track of each superpixel so treated,
so that pixels on its boundary in $M'$ can
contribute properly to the coefficients of the
sparse, square matrix used in the solution and
so that, after the solution is obtained, the
superpixel's corresponding pixels in $M'$ (other
than the corner pixels) can be filled by linear
interpolation of the corners.  Also, set all of
the corresponding pixels at the intermediate
resolutions to zero.

Do the same thing in $M_{N-1}$ for each
superpixel whose value is $2^{2[N-1]}$, in
$M_{N-2}$ for each superpixel whose value is
$2^{2[N-2]}$, etc., and concluding with the same
treatment in $M_2$ for each superpixel whose
value is 16.

### Prepare Linear Model

When the size of the region to be filled is
larger than, say, $8 \times 8$ pixels, the
elimination of all but the four corner pixels
from each full superpixel drastically reduces
the size of the linear problem.  The remaining
square matrix is still sparse, especially as the
margin $m$ increases, but the matrix does become
denser.  Whenever a lone pixel $p$ in the linear
system lie, say, to the left of the border of a
full superpixel, the contribution from the
border-pixel is computed as the linear
interpolation of the two corner-pixels that
bound that border.  So $p$ is connected not just
to a single value on the right but to a properly
weighted pair on the right.  Effects like this
increase the density of the problem.

However, the time to factor the matrix and to
render the solution should be greatly reduced
for any sufficiently large region of pixels to
be filled.

### Interpolation

After the problem is solved, the edges and
interior of each full superpixel in region to be
filled must be filled with the values obtained
by linearly interpolation the corner-values,
which were solved for.

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
