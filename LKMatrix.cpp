#include "LKMatrix.h"
#include <vector>
#include <cmath>
#include <set>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

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
    //cout << edgeDistances[i][tour[i]] << "; ";
    distance += edgeDistances[i][tour[i]];
  }
  //cout << endl;

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
  long initialTourDistance = getCurrentTourDistance();

  do {
    // default, no nextV is found
    nextV = -1;


    broken_edge = make_sorted_pair(lastNextV, fromV); // := x_i
    broken_edge_length = edgeDistances[broken_edge.first][broken_edge.second];

    //cout << "Breaking " << lastNextV << " " << fromV << endl;
    //cout << "Testing from " << fromV << endl;;
    //cout << "Breaking of length: " << broken_edge_length << endl;

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
      //cout << "Moving to " << nextV << endl;
    }

    // a next y_i exists
    if (nextV != -1) {

      // add to our broken_set and joined_set
      broken_set.insert(broken_edge);
      joined_set.insert(make_sorted_pair(fromV, nextV));

      // condition 4(f)
      y_opt_length = edgeDistances[fromV][tourStart]; // y_i_opt
      
      // The tour length if we exchanged the broken edge (x_i)
      // with y_opt, (t_{2i}, t_0)
      g_opt_local = g + (broken_edge_length - y_opt_length);

      if (g_opt_local > g_opt) {
        //vector<int> temp_tour = tour;
        //temp_tour[tourStart] = fromV;
        //tour = tour_opt;
        //printTour();
        //long old_distance = getCurrentTourDistance();
        //tour = temp_tour;
        //printTour();
        //long new_distance = getCurrentTourDistance();
        //cout << "(Temp) Joining of distance: " << y_opt_length << endl;
        //cout << "Old distance: " << old_distance << endl;
        //cout << "New distance: " << new_distance << endl;
        //assert(new_distance <= old_distance);
        g_opt = g_opt_local;
        tour_opt = tour;
        // join the optimal tour
        tour_opt[tourStart] = fromV;
      }

      //cout << "Joining of distance: " << edgeDistances[fromV][nextV] << endl;

      // recalculate g
      g += broken_edge_length - edgeDistances[fromV][nextV];

      // reverse tour direction between newNextV and fromV
      // implicitly breaks x_i
      reverse(fromV, lastPossibleNextV);

      // remember our new t_{2i+1}
      nextFromV = lastPossibleNextV;
      //cout << "Joined to " << nextFromV << endl;

      // build y_i
      tour[fromV] = nextV;
      
      // set new fromV to t_{2i+1}
      // and out lastNextV to t_{2i}
      lastNextV = nextV;
      fromV = nextFromV;

    }

  } while (nextV != -1);


  // join up
  //cout << "terminated" << endl;
  tour = tour_opt;
  long distanceAfter = getCurrentTourDistance();
  assert(distanceAfter <= initialTourDistance);

  printTour();
  assert(isTour());

}

void LKMatrix::optimizeTour() {
  // we need to test for convergence and also the difference from last time
  int diff;
  int old_distance = 0;
  int new_distance = 0;

  for (int j = 0; j < 100; j++) {
    for (int i = 0; i < size; i++) {
      LKMove(i);
    }
    new_distance = getCurrentTourDistance();
    diff = old_distance - new_distance;
    if (j != 0) {
      assert(diff >= 0);
      if (diff == 0) {
        //cout << "Converged after " << j << " iterations" << endl;
        break;
      }
    };
    old_distance = new_distance;
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
    //cout << current << " ; ";
    current = tour[current];
  } while (current != 0);
  //cout << endl;
}

void LKMatrix::printTourIds() {
  int current = 0;
  do {
    cout << ids[current] << endl;
    current = tour[current];
  } while (current != 0);
}
