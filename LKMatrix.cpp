
#include "LKMatrix.h"
#include <vector>
#include <cmath>

using namespace std;

LKMatrix::LKMatrix(vector<pair<double, double> > &coords, vector<int> &ids) {
  this->coords = coords;
  this->ids = ids;
  size = ids.size();

  vector<vector<int> > edgeFlags(size, vector<int> (size, OUT_TOUR));

  // set the current tour

  for (int i = 0; i < size; i++) {
    joinLocations(i, (i+1) % size);
  }

  // sets the distanceVector
  vector<vector<double> > edgeDistances(size, vector<double> (size, 0));

  double edgeDistance;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {

      // Compute the edge distance
      edgeDistance = sqrt(pow((coords[i].first - coords[j].first), 2) + 
          pow((coords[i].second - coords[j].second), 2));
      edgeDistances[i][j] = edgeDistance;
      edgeDistances[j][i] = edgeDistance;
    }
  }
}

void LKMatrix::joinLocations(int i, int j) {
  edgeFlags[i][j] = IN_TOUR;
  edgeFlags[j][i] = IN_TOUR;
}


double LKMatrix::getCurrentTourDistance() {
  return 0;
}

vector<int> LKMatrix::getCurrentTour() {
  // Calculate current tour
  vector<int> currentTour(size, 0);

  // walk the adjacency matrix
  int tourIndex = 0;

  int fromIndex = -1;
  int currentIndex = 0;

  vector<int> traversal;
  do {
    currentTour[tourIndex++] = currentIndex;

    traversal = edgeFlags[currentIndex];
    for (int i = 0; i < size; i++) {
      if (traversal[i] == IN_TOUR && i != fromIndex) {
        fromIndex = currentIndex;
        currentIndex = i;
        break;
      }
    }
  } while (currentIndex != 0);

  return currentTour;
}
