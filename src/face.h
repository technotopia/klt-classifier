/*
 author: Maxim Volgin (2012)
 mailto: technotopia(at)gmail.com
*/

#ifndef FACE_H
#define FACE_H

#include "linalg.h"
#include "projection.h"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <vector>
#include <map>
#include <string>

using namespace std;
using boost::shared_ptr;
using boost::shared_array;
using alglib::real_2d_array;

typedef shared_array<double> PVector;

struct FaceVector {
    FaceVector() {v = PVector (NULL);}
    FaceVector(PSubspace _pspace) : pspace(_pspace) {
        int dim = pspace->getDimmension();
        v = PVector (new double[dim]);
        for (int i = 0; i < dim; v[i++] = 0);
    }
    PSubspace pspace;
    PVector v;
};

typedef map<string,FaceVector> FaceProjections;

class Face;
typedef boost::shared_ptr<Face> PFace;
typedef std::vector<PFace> Faces;

class GroupSum : public FaceVector {
public:
    GroupSum(PSubspace space) : FaceVector(space), count(0) {};
    GroupSum() : FaceVector(), count(0) {};
    void push(PFace);
    void getAverage();
protected:
    int count;
};

class Face {
public:
    Face(int size);
    ~Face();
    int readString(char* str, PSubspace sup);
    int readString(string& str, PSubspace sup);    
    int use (const string& spaceName);
    int project(PProjection pr);
    int normalize(PSubspace sub);
    
    double get(int i) const {return v[i];}
    int getId() const {return id;}
    int getPid() const {return pid;}
    void print(stringstream& ss, const string& name);
    double distance(PFace f, const string& name);
protected:
//     vector<int> raw;
    FaceProjections mv;
    PVector v;
    int size;
    int id;
    int pid;
    bool isSample; //picture that represents specific person 
    bool _inited;    
};

#endif


