/*
 author: Maxim Volgin (2012)
 mailto: technotopia(at)gmail.com
*/

#ifndef FACE_CONTROL_H
#define FACE_CONTROL_H

#include <vector>
#include <map>
#include "linalg.h"
#include "face.h"
#include "common.h"

using alglib::real_2d_array;
using alglib::real_1d_array;
using alglib::rmatrixsvd;

typedef std::map<string, PSubspace> SpaceMap;
typedef std::pair<string, PSubspace> SpacePair;


typedef std::map<int, GroupSum> GroupSums;
typedef std::map<int, int> PersonMap;
typedef std::pair<int, int> Range;

class FaceControl {
public:
    FaceControl() : raw_size(0) {}
    ~FaceControl() {}
    int readFile(const string& fileName, Domain domain);
    int readLearning(const string& fileName);
    int readSamples(const string& fileName);

    int getRawSize() {return raw_size;}
    
    // TODO rewrite using GroupBy and RestrictTo functors
    real_2d_array calculateCovariation(PSubspace super);
    real_2d_array calculateCovariationGroupPid(PSubspace super);
    real_2d_array calculateCovariation(const string& name);
    
    PProjection makeProjection(PSubspace super, const string& name);    
    PProjection makeProjectionGroupPid(PSubspace super, const string& name);
    PProjection makeProjectionDistinctPid(PSubspace super, const string& name);
    
    PSubspace getSpace(const string& spaceName);
    
    void projectFaces(PProjection pr, Domain domain);
    void normalizeFaces(string subspaceName, Domain domain);
    void printFaces(stringstream& ss, const string& name, Domain domain);
    void printFaces(const string& name, Domain domain);
    void printDistance(int id1, int id2, const string& spaceName, Domain domain);
    
    void distanceStats(const string& spaceName); // only needed for learning faces
    void sampleTests(const string& spaceName);
    
    int clusterTests(const string& spaceName, Domain domain, double threshold);
    
    
    void learn(); //make all subspaces and projections
    
    Faces& getFaces(Domain domain);
protected:
    PProjection makeProjectionCommon(PSubspace super, const string& name, real_2d_array& cov, bool distinct);
    Faces vfaces; // learning faces - build subspaces and projections
    Faces vsamples; // sample faces
    Faces vtests; // test faces
    std::vector<double> rawAverage;
    int raw_size;
    SpaceMap smap;
    PersonMap pidmap;
    Range learningFaces;
    Range sampleFaces;
};

#endif
