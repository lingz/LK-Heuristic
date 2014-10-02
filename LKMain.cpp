#include<iostream>
#include<vector>

#include "LKMatrix.h"

vector<int> id;
vector<pair<double,double> > coord;

int main(){
  int n;
  double x, y;
  while(cin >> n){
    id.push_back(n);
    cin >> x >> y;
    coord.push_back(make_pair(x, y));
  }

  LKMatrix mat(coord, id);

  mat.optimizeTour();

  //cout << mat.getCurrentTourDistance() << endl;
  mat.printTourIds();
}
