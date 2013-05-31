/*
 author: Maxim Volgin (2012)
 mailto: technotopia(at)gmail.com
*/

#include "linalg.h"
#include "face.h"
#include "projection.h"
#include "faceControl.h"

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <boost/algorithm/string.hpp>

using namespace std;

using alglib::real_2d_array;
using alglib::real_1d_array;
using alglib::rmatrixsvd;

Face::Face(int size_) : size(size_) {
    _inited = false;
}

Face::~Face() {
    _inited = false;
}

int Face::use (const string& spaceName) {
    FaceProjections::iterator fv = mv.find(spaceName);
    if (fv == mv.end()) {
        // Create new?
        return -1;
    }
    v = fv->second.v;
    return 0;
}

int Face::readString(char* str, PSubspace sup) {
    if (!sup) {
        printf("ERROR: Face::readString needs initialized superspace\n");
        return -1;
    }
    int size = sup->getDimmension();
    mv["Raw"] = FaceVector(sup);
    use("Raw");
    
    char* token = strtok(str, ",");
    int i = 0;
    if (!token) {
        printf("incomplete line : no id\n");
        return -2;
    }    
    id = atoi(token);
    token = strtok(NULL, ",");
    
    if (!token) {
        printf("incomplete line : no person id\n");
        return -2;
    }    
    pid = atoi(token);
    token = strtok(NULL, ",");
    
    while ((token != NULL) && (i < size)) {
        v[i] = atoi(token);
        token = strtok(NULL, ",");
        i++;
    }
    if (i < size) {
        printf("incomplete line : only %d fields\n", i);
        return -1;
    }
    _inited = true;
    return 0;
}

int Face::project(PProjection pr) {
    PSubspace sup = pr->getSup();
    PSubspace sub = pr->getSub();
    if (!mv.count(sup->getName())) {
        cout << "Face::project can not project from " << sup->getName() << " no coordinates in that space" << endl;
        return -1;
    }
    mv[sub->getName()] = FaceVector(sub);
    pr->project(mv[sub->getName()], mv[sup->getName()]);
    return 0;
}

int Face::normalize(PSubspace sub) {
    FaceVector& fv = mv[sub->getName()];
    int dim = sub->getDimmension();
    double r = 0;
    for (int i = 0; i < dim; i++) {
        r += fv.v[i] * fv.v[i];
    }
    r = sqrt(r);
    if (r > 0) {
        for (int i = 0; i < dim; i++) {
            fv.v[i] /= r;
        }
    }
    return 0;
}

void Face::print(stringstream& ss, const string& spaceName) {
    ss << "Id = " << id << ", person = " << pid << ", ";
    if (!mv.count(spaceName)) {
        ss << "No projection " << spaceName;
    } else {
        FaceVector& fv = mv[spaceName];
        int dim = fv.pspace->getDimmension();
        ss << "v = ( ";
        for (int i = 0; i < dim; i++) {
            ss << fv.v[i] << " ";
        }
        ss << ")";
    }
    ss << endl;
}

double Face::distance(PFace pf, const string& spaceName) {
    FaceProjections::iterator fv = mv.find(spaceName);
    if (fv == mv.end()) {
        throw ("Face not projected into that subspace");
    }
    v = fv->second.v;
    int dim = fv->second.pspace->getDimmension(); 
    pf->use(spaceName);
    double dist = 0;
    double d;
    for (int i = 0; i < dim; i++) {
        d = (v[i] - pf->get(i));
        dist += d*d;
    }
    return dist;
}
