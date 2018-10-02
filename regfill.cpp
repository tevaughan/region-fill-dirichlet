
#include "image.hpp"

using namespace regfill;
using namespace std;

static vector<pcoord> internal_boundary{
    {240, 155}, {245, 164}, {255, 170}, {270, 173}, {285, 172},
    {290, 166}, {280, 161}, {270, 155}, {250, 153}, {240, 155}};

static pcoord const zoom_tl{230,140};
static pcoord const zoom_br{300,180};

int main() {
  image img("trees-raw.pgm");
  for (unsigned i = 0; i < internal_boundary.size(); ++i) {
    // Correct for difference between Matlab index and C offset.
    internal_boundary[i].col -= 1;
    internal_boundary[i].row -= 1;
  }
  img.draw_polyline(internal_boundary, 255);
  img.fill({265, 163}, 255);
  img.write("boundary.pgm");
  return 0;
}

