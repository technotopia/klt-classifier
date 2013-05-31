/*
 author: Maxim Volgin (2012)
 mailto: technotopia(at)gmail.com
*/

#include "linalg.h"
#include "face.h"
#include "faceControl.h"
#include "projection.h"

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <stack>
#include <boost/algorithm/string.hpp>

Faces& FaceControl::getFaces(Domain domain) {
    if (domain == LEARNING_DOMAIN) {return vfaces;}
    if (domain == SAMPLE_DOMAIN) {return vsamples;}
    if (domain == TEST_DOMAIN) {return vtests;}
    throw (string("Unknown domain"));
}

int FaceControl::readLearning(const string& fileName) {
    return readFile(fileName, LEARNING_DOMAIN);
}

int FaceControl::readSamples(const string& fileName) {
    return readFile(fileName, SAMPLE_DOMAIN);
}

int FaceControl::readFile(const string& fileName, Domain domain = LEARNING_DOMAIN) {
    ifstream ifile;
    ifile.open(fileName.c_str());
    Faces& vf = getFaces(domain);     

    string header;
    string line;
    char cline[4096];
    if (!ifile.is_open()) {
        cout << "ERROR: FaceControl::readFile can not open " << fileName << endl;
        return -1;
    }
    getline(ifile, header);
    strcpy(cline, header.c_str());
    
    char* token = strtok(cline, " ,\t\n");
    if (!raw_size) {
        raw_size = atoi(token);
    } else if (raw_size != atoi(token)) {
        cout << "ERROR: FaceControl::readFile will not read " << fileName <<", wrong number of columns" << endl;
        ifile.close();
        return -1;    
    }
    if (VERBOSITY_LEVEL > 0) {
        cout << "DEBUG: FaceControl::readFile header = " << header << endl;
        cout << "DEBUG: raw_size " << raw_size << endl;
    }
    
    PSubspace rawSpace;
    if (!smap.count("Raw")) {
        smap.insert(SpacePair("Raw", PSubspace(new Subspace("Raw", raw_size))));
        rawSpace = getSpace("Raw");
        if (VERBOSITY_LEVEL > 1) {
            cout << "registered subspace \"Raw\" of size " << rawSpace->getDimmension() << endl;
        }
    }
    rawSpace = getSpace("Raw");
    GroupSum average(rawSpace);  

    int n = 0;
    while (!ifile.eof()) {
        getline(ifile, line);
        strcpy(cline, line.c_str());
        if (VERBOSITY_LEVEL > 2) {
            cout << "DEBUG: FaceControl::readFile line = " << cline << endl;
        }
        PFace pface = PFace(new Face(rawSpace->getDimmension()));
        if (!pface->readString(cline, rawSpace)) {
            vf.push_back(pface);
            n++;         
        }
        average.push(pface);
    }
    average.getAverage();
    rawSpace->setAverage(average);
    if (VERBOSITY_LEVEL > 1) {
        cout << "Read " << n << " Records from " << fileName << endl;
    }
    return n;
}

PSubspace FaceControl::getSpace(const string& spaceName) {
    SpaceMap::iterator found = smap.find(spaceName);
    if (found != smap.end()) {
        return found->second;
    } else {
        return PSubspace();
    }
}    

real_2d_array FaceControl::calculateCovariation(const string& subspaceName) {
    return calculateCovariation(smap[subspaceName]);
}

real_2d_array FaceControl::calculateCovariationGroupPid(PSubspace sup) {
    if (!sup) {
        throw string("ERROR: makeProjection needs initialized superspace");
    }
    //need pid average
    if (VERBOSITY_LEVEL > 1) {
        cout << "FaceControl::calculateCovariationGroupPid(" << sup->getName() << ")" << endl;
    }
    GroupSums averages;
    for (Faces::iterator it = vfaces.begin(); it != vfaces.end(); it++) {
        PFace face = *it;
        face->use(sup->getName());
        int facePid = face->getPid();
        if (!averages.count(facePid)) {
            averages[facePid] = GroupSum(sup);
            if (VERBOSITY_LEVEL > 2) {
                cout << "FaceControl::calculateCovariationGroupPid add person " << facePid << endl;
            }
        } else {
            if (VERBOSITY_LEVEL > 2) {
                cout << "FaceControl::calculateCovariationGroupPid known person " << facePid << endl;
            }
        }
        averages[facePid].push(face);
    }
    for (GroupSums::iterator it = averages.begin(); it != averages.end(); it++) {
        if (VERBOSITY_LEVEL > 2) {
            cout << "FaceControl::calculateCovariationGroupPid average for group " << it->first << endl;
        }
        it->second.getAverage();
    }
    //calculate covariation
    real_2d_array cov;
    int size = sup->getDimmension();
    cov.setlength(size, size);
    if (VERBOSITY_LEVEL > 2) {
        cout << endl << "raw covariation" << endl;
    }
    double div = vfaces.size();
    div = (div > 0) ? div : 1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double covij = 0;
            for (Faces::iterator it = vfaces.begin(); it != vfaces.end(); it++) {
                int facePid = (*it)->getPid();
                GroupSum average = averages[facePid];
                covij += ((*it)->get(i) - average.v[i]) * ((*it)->get(j) - average.v[j]);
            }
            cov(i,j) = covij / div;
            if (VERBOSITY_LEVEL > 2) {
                cout << cov(i,j) << ",";
            }
        }
        if (VERBOSITY_LEVEL > 2) {
            cout << endl;
        }
    }
    return cov;
}

real_2d_array FaceControl::calculateCovariation(PSubspace sup) {
    if (!sup) {
        throw string("ERROR: makeProjection needs initialized superspace");
    }
    real_2d_array cov;
    const string& supName = sup->getName();
    
    int size = sup->getDimmension();
    cov.setlength(size, size);
    if (VERBOSITY_LEVEL > 1) {
        cout << "covariation in space "  << supName << endl;
    }
    
    
    double div = vfaces.size();
    div = (div > 0) ? div : 1;
    if (VERBOSITY_LEVEL > 2) {
        cout << "Averages = {" << endl;
        for (int i = 0; i < size; i++) {
            cout << "[" << i << "] = " << sup->getAverage(i) << endl;
        }
        cout << "}" << endl;
    }
    //TODO optimize loop
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double covij = 0;
            double e1 = sup->getAverage(i);
            double e2 = sup->getAverage(j);
            for (Faces::iterator it = vfaces.begin(); it != vfaces.end(); it++) {
                (*it)->use(sup->getName());
                covij += ((*it)->get(i) - e1) * ((*it)->get(j) - e2);
            }
            cov[i][j] = covij / div;
        }
    }
    if (VERBOSITY_LEVEL > 2) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                printf("%lf,", cov[i][j]);
            }
            printf("\n");
        }           
    }
    return cov;
}

PProjection FaceControl::makeProjectionCommon(PSubspace sup, const string& name, real_2d_array& cov, bool distinct = false) {
    int size = sup->getDimmension();
    real_1d_array sv; 
    sv.setlength(size);
    real_2d_array mU; 
    mU.setlength(size,size);
    real_2d_array mV; 
    mV.setlength(size,size);
        
    rmatrixsvd(cov, size, size, 2, 2, 2, sv, mU, mV);
    
    int subspaceDimmension = size;
    double maxSV = sv(0);
    int first = 0;
    int last = size - 1;
    double weightPower = COMMON_WEIGHT_POWER; 
    if (!distinct) {
        for (int i = 0; i < size; i++) {
            if (VERBOSITY_LEVEL > 1) {
                cout << "Singular value #" << i + 1 << " = " << sv(i) << endl;
            }
            if (sv(i) / maxSV < SV_THRESHOLD) {
                subspaceDimmension = i + 1;
                last = i;
                break;
            }
        }
    } else {
        weightPower = DISTINCT_WEIGHT_POWER; 
        // seek in range [maxSV * SV_DISTINCT_MIN, maxSV * SV_DISTINCT_MAX]
        for (first = 0; (first < size) && (sv(first) / maxSV > SV_DISTINCT_MAX); first++) {
            if (VERBOSITY_LEVEL > 1) {
                cout << "Too big Singular value #" << first + 1 << " = " << sv(first) << endl;
            }
        }
        for (last = size - 1; (last > -1) && (sv(last) / maxSV < SV_DISTINCT_MIN); last--) {
            if (VERBOSITY_LEVEL > 1) {
                cout << "Too small Singular value #" << last + 1 << " = " << sv(last) << endl;
            }
        }
        if (last < first) {
            throw ("Can not create 'distinction' subspace - no singular values in range");
        }
        subspaceDimmension = last - first + 1;
        for (int i = first; i <= last ; i++) {
            if (VERBOSITY_LEVEL > 1) {
                cout << "Singular value in range #" << i + 1 << " = " << sv(i) << endl;
            }
        }
    }

    PSubspace sub = PSubspace(new Subspace(name, subspaceDimmension));
    smap.insert(SpacePair(name, sub));

    PProjection pr = PProjection(new Projection(sup, sub, mV, sv, first, last, weightPower));
    return pr;
}

PProjection FaceControl::makeProjection(PSubspace sup, const string& name) {
    if (!sup) {
        throw string("ERROR: makeProjection needs initialized superspace\n");
    }
    real_2d_array cov = calculateCovariation(sup);
    return makeProjectionCommon(sup, name, cov);
}

PProjection FaceControl::makeProjectionGroupPid(PSubspace sup, const string& name) {
    if (!sup) {
        throw string("ERROR: makeProjection needs initialized superspace\n");
    }
    real_2d_array cov = calculateCovariationGroupPid(sup);
    return makeProjectionCommon(sup, name, cov);
}

PProjection FaceControl::makeProjectionDistinctPid(PSubspace sup, const string& name) {
    if (!sup) {
        throw string("ERROR: makeProjection needs initialized superspace\n");
    }
    real_2d_array cov = calculateCovariationGroupPid(sup);
    return makeProjectionCommon(sup, name, cov, true);
}

void FaceControl::projectFaces(PProjection pr, Domain domain) {
    Faces& vf = getFaces(domain);

    for (Faces::iterator it = vf.begin(); it != vf.end(); it++) {
        PFace face = *it;
        face->project(pr);
    }
}

void FaceControl::normalizeFaces(string subspaceName, Domain domain) {
    Faces& vf = getFaces(domain);
    if (smap.count(subspaceName)) {
        for (Faces::iterator it = vf.begin(); it != vf.end(); it++) {
            PFace face = *it;
            face->normalize(smap[subspaceName]);
        }
    }
}


void FaceControl::printFaces(stringstream& ss, const string& spaceName, Domain domain) {
    Faces& vf = getFaces(domain);
    for (Faces::iterator it = vf.begin(); it != vf.end(); it++) {
        PFace face = *it;
        face->print(ss, spaceName);        
    }
}

void FaceControl::printFaces(const string& spaceName, Domain domain) {
    stringstream ss;
    printFaces(ss, spaceName, domain);
    cout << ss.str();
}

void FaceControl::printDistance(int id1, int id2, const string& spaceName, Domain domain) {
    Faces& vf = getFaces(domain);
    PFace pf1 = vf[id1];
    PFace pf2 = vf[id2];
    
    cout << "In " << spaceName << " rho (" << id1 << "(pid = " << pf1->getPid() << "), " << id2 << "(pid = " << pf2->getPid() << ")) = " << pf1->distance(pf2, spaceName) << endl;
}

void FaceControl::distanceStats(const string& spaceName) {
    PFace pf1,pf2;
    int stats[24];
    int statsDistinct[24];
    int statsSame[24];
    for (int i = 0; i < 24; i++) {
        stats[i] = 0;
        statsDistinct[i] = 0;
        statsSame[i] = 0;
    }
    for (Faces::iterator i = vfaces.begin(); i != vfaces.end(); i++) {
        for (Faces::iterator j = i+1; j != vfaces.end(); j++) {
            pf1 = *i;
            pf2 = *j;
            bool same = (pf1->getPid() == pf2->getPid());
            double d = pf1->distance(pf2, spaceName);
            int k = 0;
            for (; (d < 200) && (k < 24); k++) {d *= 1.5;}
            if (same) {
                statsSame[k]++;
            } else {
                statsDistinct[k]++;
            }
            stats[k]++;
        }
    }
    cout << "Stats for " << spaceName << endl;
    cout << "# of More then\t\t Total \t Distinct Pid \t Same Pid" << endl;
    double v = 200;
    for (int i = 0; i < 24; i++) {
        printf ("%lf\t|\t%d\t|\t%d\t|\t%d\n", v, stats[i], statsDistinct[i], statsSame[i]);
        v /= 1.5;
    }
}

void FaceControl::sampleTests(const string& spaceName) {
    cout << "starting sample tests" << endl;
    
    vector<int> placeStats;
    placeStats.resize(vsamples.size());
    
    vector<DistanceTo> scores;
    scores.reserve(vsamples.size());
    int count = 0;
    for (Faces::iterator ti = vtests.begin(); ti != vtests.end(); ti++, count++) {
        PFace testFace = *ti;
        if (VERBOSITY_LEVEL > 2) {
            cout << "testing #" << testFace->getId() << ", pid = " << testFace->getPid() << endl;
        }
        if ((VERBOSITY_LEVEL > 1) && !(count % 1000)) {
            cout << "test #" << count << endl;
        }
        uint i = 0;
        for (Faces::iterator si = vsamples.begin(); si != vsamples.end(); si++, i++) {
            PFace sampleFace = *si;
            double distance = testFace->distance(sampleFace, spaceName);
            scores[i] = DistanceTo(sampleFace->getPid(), distance);
        }
        std::sort(scores.begin(), scores.begin() + vsamples.size());
        if (vsamples.size() < 30) {
            for (i = 0; i < vsamples.size(); i++) {
                if (scores[i].pid == testFace->getPid()) {
                    cout << "\t!!->\t";
                    placeStats[i]++;
                } else {
                    cout << "\t\t";                
                }
                cout << "at #" << i << " sample #" << scores[i].pid << " with distance = " << scores[i].distance << endl;
            }
        } else {
            for (i = 0; i < vsamples.size(); i++) {
                if (scores[i].pid == testFace->getPid()) {
                    placeStats[i]++;
                }
            }
        }
    }
    cout << "Place stats" << endl;
    int sofar = 0;
    for (unsigned int p = 0; p < vsamples.size(); p++) {
        sofar += placeStats[p];
        cout << "\tat #" << p << " - " << placeStats[p] << " times,\t sofar " << sofar << endl;
    }
}

int FaceControl::clusterTests(const string& spaceName, Domain domain, double threshold) {
    Faces& vf = getFaces(domain);
    int size = vf.size();
    stack<int> toCheck;
    vector<int> clusterMap;
    clusterMap.resize(size);
    int currentCluster = 0;
    int maxClusterSize = 0;
    while (true) {
        int nextFace = -1;
        for (int i = 0; i < size; i++) {
            if (!clusterMap[i]) {
                nextFace = i;
                break;
            }
        }
        if (nextFace < 0) {
            cout << "clustering done" << endl;
            break;
        } else {
            currentCluster++;
            int clusterSize = 1;
            cout << "clustering: face[#" << nextFace << "] pid = " << vf[nextFace]->getPid() << " is seed for cluster #" << currentCluster << endl;
            clusterMap[nextFace] = currentCluster;
            toCheck.push(nextFace);
            while (toCheck.size() > 0) {
                int checking = toCheck.top();
                toCheck.pop();
                for (int i = 0; i < size; i++) {
                    if (!clusterMap[i] && (vf[checking]->distance(vf[i], spaceName) < threshold)) {
                        clusterMap[i] = currentCluster;
                        toCheck.push(i);
                        clusterSize++;
                        cout << "\tclustering: face[#" << i << "] pid = " << vf[i]->getPid() << " added to cluster #" << currentCluster << endl;
                    }
                }
            }
            if (clusterSize > maxClusterSize) {
                maxClusterSize = clusterSize;
            }
        }        
    }
    return maxClusterSize;
}
