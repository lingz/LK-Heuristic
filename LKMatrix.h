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
    void optimizeTour();
    bool isTour();
    void printTour();

  private:
    vector<int> tour;
    vector<vector<int> > edgeFlags;
    vector<pair<double, double> > coords;
    vector<int> ids;
    void joinLocations(int i, int j);
    vector<vector<double> > edgeDistances;
    void LKMove(int tourStart);
    void reverse(int start, int end);
};

#endif
