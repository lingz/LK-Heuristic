#include "LKMatrix.h"
#include <vector>
#include <cmath>
#include <set>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>

using namespace std;

pair<int, int> make_sorted_pair(int x, int y) {
  if (x < y) {
    return make_pair(x, y);
  } else {
    return make_pair(y, x);
  }
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
    cout << edgeDistances[i][tour[i]] << "; ";
    distance += edgeDistances[i][tour[i]];
  }
  cout << endl;

  return distance;
}

void LKMatrix::LKMove(int tourStart) {
  set<pair<int,int> > broken_set, joined_set;
  vector<int> tour_opt = tour;
  double g_opt = 0;
  double g = 0; // := G_i
  double g_local; // := g_i
  int lastNextV = tourStart;
  int fromV;
  int nextV;
  int nextFromV;
  int lastPossibleNextV;
  pair<int, int> broken_edge;
  double y_opt_length;
  double broken_edge_length;
  double g_opt_local;

  fromV = tour[lastNextV];

  do {
    // default, no nextV is found
    nextV = -1;

    cout << "Breaking " << lastNextV << " " << fromV << endl;
    cout << "Testing from " << fromV << endl;;

    broken_edge = make_sorted_pair(lastNextV, fromV); // := x_i
    broken_edge_length = edgeDistances[broken_edge.first][broken_edge.second];

    // Condition 4(c)(1)
    if (joined_set.count(broken_edge) > 0) break;

    // y_i := (fromV, nextV)
    for (int possibleNextV = tour[fromV]; nextV == -1 && possibleNextV != tourStart; possibleNextV = tour[possibleNextV]) {
      //cout << "Testing " << possibleNextV << endl;
      //cout << (broken_set.count(make_sorted_pair(fromV, possibleNextV)) == 0) << endl; 
      //cout << (possibleNextV != fromV) << endl; 
      //cout << (g + g_local > 0) << endl; 
      //cout << (joined_set.count(make_sorted_pair(possibleNextV, tour[possibleNextV])) == 0) << endl; 


      // calculate local gain
      g_local = broken_edge_length - edgeDistances[fromV][possibleNextV];

      //cout << "Distances" << endl;
      //cout << broken_edge_length << endl;
      //cout << g_local << endl;

      // conditions that make this edge not a valid y_i
      if (!(
        // condition 4(c)(2)
        broken_set.count(make_sorted_pair(fromV, possibleNextV)) == 0 &&
        // condition 4(d)
        g + g_local > 0 &&
        // condition 4(e)
        // x_{i+1} has never been joined before
        joined_set.count(make_sorted_pair(lastPossibleNextV, possibleNextV)) == 0 &&
        tour[possibleNextV] != 0 && // not already joined to start
        possibleNextV != tour[fromV] // not the one already joined to fromV
      )) {
        lastPossibleNextV = possibleNextV;
        continue;
      }

      // If we are here, then y_i := (fromV, possibleNextV)
      nextV = possibleNextV;
      cout << "Moving to " << nextV << endl;
    }

    // a next y_i exists
    if (nextV != -1) {

      // add to our broken_set and joined_set
      broken_set.insert(broken_edge);
      joined_set.insert(make_sorted_pair(fromV, nextV));

      // condition 4(f)
      y_opt_length = edgeDistances[tour[fromV]][tourStart]; // y_i_opt
      
      // The tour length if we exchanged the broken edge (x_i)
      // with y_opt, (t_{2i}, t_0)
      g_opt_local = g + (broken_edge_length - y_opt_length);

      if (g_opt_local > g_opt) {
        g_opt = g_opt_local;
        tour_opt = tour;
        // join the optimal tour
        tour_opt[tourStart] = fromV;
      }

      // recalculate g
      g += broken_edge_length - edgeDistances[fromV][nextV];

      // reverse tour direction between newNextV and fromV
      reverse(fromV, lastPossibleNextV);

      // remember our new t_{2i+1}
      nextFromV = lastPossibleNextV;
      cout << "Joined to " << nextFromV << endl;

      // build y_i
      tour[fromV] = nextV;
      
      // set new fromV to t_{2i+1}
      // and out lastNextV to t_{2i}
      lastNextV = nextV;
      fromV = nextFromV;

    }

  } while (nextV != -1);


  // join up
  cout << "terminated" << endl;
  tour = tour_opt;
  printTour();
  assert(isTour());

}

void LKMatrix::optimizeTour() {
  for (int i = 0; i < size; i++) {
    LKMove(i);
  }
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

// Sanity check function
bool LKMatrix::isTour() {
  int count = 1;
  int start = tour[0];
  while (start != 0) {
    start = tour[start];
    count++;
  }
  return (count == size);
}


void LKMatrix::printTour() {
  int current = 0;
  do {
    cout << current << " ; ";
    current = tour[current];
  } while (current != 0);
  cout << endl;
}
