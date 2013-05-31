/*
 author: Maxim Volgin (2012)
 mailto: technotopia(at)gmail.com
*/

#ifndef PROJECTION_H
#define PROJECTION_H

#include "linalg.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <map>

#include "common.h"

using namespace std;
using alglib::real_2d_array;
using alglib::real_1d_array;

class Subspace;
struct FaceVector;
typedef boost::shared_ptr<Subspace> PSubspace;
class GroupSum;

// Defines relation between two spaces
// Or should I use alglib matrixes instead of double*?
class Projection {
public:
   Projection(PSubspace sup, PSubspace sub, real_2d_array& v, real_1d_array& w, int first, int last, double weightPower); 
   
   vector<double> project(vector<int>& x);
   vector<double> project(vector<double>& x);
   void project(FaceVector&, vector<double>& x);
   void project(FaceVector&, FaceVector&);
   PSubspace getSub() ;
   PSubspace getSup() ;
protected:
   PSubspace psub;
   PSubspace psup;
   vector<double> basis;
   vector<double> weights;
   bool _inited;
};

typedef boost::shared_ptr<Projection> PProjection;

class Subspace {
public:
    Subspace(const string& name_, int dimmension_) : name(name_), dimmension(dimmension_) {
        weights.resize(dimmension);
        average.resize(dimmension);
        for (int i = 0; i < dimmension; i++) {
            weights[i] = 1;
            average[i] = 0;
        }
	}
	const string& getName() const {return name;}
	int getDimmension() const {return dimmension;}
	double getAverage(int i) const {return average[i];}
	void setAverage(GroupSum a);
protected:
    string name;
    vector<double> weights; // eigen values
    vector<double> average; // average values for each coordinate
    int dimmension;        
    
};

#endif
