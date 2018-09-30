
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class pgm_header {
  int num_cols_;
  int num_rows_;
  int max_val_;

public:
  pgm_header() : num_cols_(0), num_rows_(0), max_val_(0) {}

  istream &init(istream &is) {
    string m;
    is >> m;
    if (m != "P5") {
      throw "magic '" + m + "' not 'P5'";
    }
    if (!(is >> num_cols_)) {
      throw string("problem reading num_cols");
    }
    if (!(is >> num_rows_)) {
      throw string("problem reading num_rows");
    }
    if (!(is >> max_val_)) {
      throw string("problem reading max_val");
    }
    int const c = is.get();
    if (c != ' ' && c != '\t' & c != '\n') {
      throw "character after maxval not white space";
    }
    return is;
  }

  int num_cols() const { return num_cols_; }
  int num_rows() const { return num_rows_; }
  int max_val() const { return max_val_; }
};

class image {
  pgm_header header_;
  vector<float> pix_;

public:
  image(string fn) {
    ifstream ifs(fn);
    if (!ifs) {
      throw "problem opening '" + fn + "'";
    }
    header_.init(ifs);
    int const num_pix = header_.num_cols() * header_.num_rows();
    pix_.resize(num_pix);
    for (int i = 0; i < num_pix; ++i) {
      pix_[i] = ifs.get();
      if (!ifs) {
        throw "error reading image";
      }
    }
  }

  pgm_header const &header() const { return header_; }

  float &pixel(int c, int r) { return pix_[r * header_.num_cols() + c]; }

  float const &pixel(int c, int r) const {
    return pix_[r * header_.num_cols() + c];
  }
};

int main(int argc, char** argv) {
  return 0;
}

