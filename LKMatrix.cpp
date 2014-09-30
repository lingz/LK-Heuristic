#include "LKMatrix.h"
#include <vector>
#include <cmath>
#include <set>
#include <iostream>

using namespace std;

// overwrite pair equality to equate reversed edges
bool operator==(pair<int, int> x, pair<int, int> y) {
  return (x.first == y.first && x.second == y.second) ||
    (x.second == y.first && x.first == y.second);
}

LKMatrix::LKMatrix(vector<pair<double, double> > &coords, vector<int> &ids) {
  this->coords = coords;
  this->ids = ids;
  size = ids.size();

  edgeFlags = vector<vector<int> > (size, vector<int> (size, OUT_TOUR));

  // initialize tour
  tour = vector<int>(size, 0);

  // initial 'random' tour
  for (int i = 0; i < size; i++) {
    tour[i] = (i + 1) % size;
  }

  // sets the distanceVector
  edgeDistances = vector<vector<double> > (size, vector<double> (size, 0));

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

double LKMatrix::getCurrentTourDistance() {
  int currentIndex = 0;
  double distance = 0;
  for (int i = 0; i < size; i++) {
    distance += edgeDistances[i][tour[i]];
  }

  return distance;
}

void LKMatrix::LKMove() {
  set<pair<int,int> > broken_set, joined_set;
  vector<int> tour_opt;
  double g_opt = 0;
  double g = 0; // := G_i
  double g_local; // := g_i
  int fromV = 0;
  int nextV;
  int newNextV;
  pair<int, int> broken_edge;
  double y_opt_length;
  double broken_edge_length;
  double g_opt_local;


  do {
    // default, no nextV is found
    nextV = -1;
    broken_edge = make_pair(fromV, tour[fromV]); // := x_i

    cout << fromV << endl;

    // Condition 4(c)(1)
    if (joined_set.count(broken_edge) > 0) break;

    // y_i := (fromV, nextV)
    for (int possibleNextV = 0; possibleNextV < size ; possibleNextV++) {
      cout << "Testing " << possibleNextV << endl;
      //cout << (broken_set.count(make_pair(fromV, possibleNextV)) == 0) << endl; 
      //cout << (possibleNextV != fromV) << endl; 
      //cout << (g + g_local > 0) << endl; 
      //cout << (joined_set.count(make_pair(possibleNextV, tour[possibleNextV])) == 0) << endl; 

      broken_edge_length = edgeDistances[broken_edge.first][broken_edge.second];

      // calculate local gain
      g_local = broken_edge_length - edgeDistances[fromV][possibleNextV];

      //cout << broken_edge_length << endl;
      //cout << g_local << endl;

      // conditions that make this edge not a valid y_i
      if (!(
        // condition 4(c)(2)
        broken_set.count(make_pair(fromV, possibleNextV)) == 0 &&
        // cannot join with self
        possibleNextV != fromV &&
        // condition 4(d)
        g + g_local > 0 &&
        // condition 4(e)
        joined_set.count(make_pair(possibleNextV, tour[possibleNextV])) == 0
      )) continue;

      // If we are here, then y_i := (fromV, possibleNextV)
      nextV = possibleNextV;
      break;
    }

    // a next y_i exists
    if (nextV != -1) {

      // add to our broken_set and joined_set
      broken_set.insert(broken_edge);
      joined_set.insert(make_pair(fromV, nextV));

      // condition 4(f)
      y_opt_length = edgeDistances[tour[fromV]][0]; // y_i_opt
      
      // The tour length if we exchanged the broken edge (x_i)
      // with y_opt, (t_{2i}, t_0)
      g_opt_local = g + (broken_edge_length - y_opt_length);
      if (g_opt_local > g_opt) {
        g_opt = g_opt_local;
        tour_opt = tour;
      }

      // recalculate g
      g += broken_edge_length - edgeDistances[fromV][nextV];

      // remember our new t_{2i+1}
      newNextV = tour[nextV];

      // reverse tour direction between newNextV and fromV
      reverse(newNextV, fromV);

      // build y_i
      tour[nextV] = fromV;
      
      // set new fromV (t_{2i+1})
      fromV = newNextV;
    }

  } while (nextV != -1);


  cout << "terminated" << endl;


}

void LKMatrix::optimizeTour() {
  LKMove();
}

/*
 * Reverse the tour between indices start and end
 */
void LKMatrix::reverse(int start, int end) {
  int current = start;
  int next = tour[start];
  int nextNext;
  do {
    //cout << "reversing" << endl;
    // look ahead to where we need to go after this iteration
    nextNext = tour[next];

    // reverse the direction at this point
    tour[next] = current;

    // move to the next pointer
    current = next;
    next = nextNext;
  } while (current != end); // terminate once we've reversed up to end
}



