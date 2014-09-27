#include<iostream>
#include<vector>

#include "LK_matrix.h"

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
  LKMatrix mat= new LKMatrix(coord, id);
  vector<int> tour = mat.getCurrentTour();
  for(int i=0; i<tour.size(); i++){
    cout<<tour[i]<<endl;
  }
}
