#ifndef LK_MATRIX
#define LK_MATRIX


#include "LK_constants.h"
#include <vector>

using namespace std;

class LKMatrix {
  public:
    int size;
    LKMatrix(vector<pair<double, double> > &coords, vector<int> &ids);
    vector<int> getCurrentTour();
    double getCurrentTourDistance();

  private:
    vector<vector<int> > edgeFlags;
    vector<pair<double, double> > coords;
    vector<int> ids;
    void joinLocations(int i, int j);
    vector<vector<double> > edgeDistances;
};

#endif
