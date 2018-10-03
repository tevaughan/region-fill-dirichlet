
#include "image.hpp"
#include <random>

using namespace regfill;
using namespace std;

static vector<pcoord> vertices{
    {240, 155}, {245, 164}, {255, 170}, {270, 173}, {285, 172},
    {290, 166}, {280, 161}, {270, 155}, {250, 153}, {240, 155}};

static pcoord const zoom_tl{230,140};
static pcoord const zoom_br{300,180};

int main() {
  for (unsigned i = 0; i < vertices.size(); ++i) {
    // Correct for difference between Matlab index and C offset.
    vertices[i].col -= 1;
    vertices[i].row -= 1;
  }
  image trees("trees-raw.pgm");
  image s_mask(trees.num_cols(), trees.num_rows());
  image t_mask(trees.num_cols(), trees.num_rows());
  s_mask.draw_polyline(vertices, 1.0);
  s_mask.fill({265, 163}, 1.0);
  image trees_mod1 = trees;
  trees_mod1.laplacian_fill(s_mask);
  trees_mod1.write("trees-mod1.pgm");
  auto const s = s_mask.threshold();
  auto const b = s_mask.boundary();
  for (auto p : s) {
    t_mask(p) = 1.0;
  }
  for (auto p : b) {
    t_mask(p) = 1.0;
  }
  s_mask.write("s_mask.pgm");
  t_mask.write("t_mask.pgm");
  image trees_mod2 = trees;
  trees_mod2.laplacian_fill(t_mask);
  trees_mod2.write("trees-mod2.pgm");
  image noise(trees.num_cols(), trees.num_rows());
  for (auto p : b) {
    noise(p) = fabs(trees_mod2(p) - trees_mod1(p));
  }
  noise.laplacian_fill(s_mask);
  noise.write("noise.pgm");
  image trees_mod3 = trees;
  std::default_random_engine generator;
  for (auto p : s) {
    std::normal_distribution<double> d(0.0, noise(p));
    trees_mod3(p) = trees_mod1(p) + d(generator);
  }
  trees_mod3.write("trees-mod3.pgm");
  return 0;
}

