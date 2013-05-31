/*
 author: Maxim Volgin (2012)
 mailto: technotopia(at)gmail.com
*/

#include "face.h"
#include "faceControl.h"
#include "linalg.h"

#include <ostream>
#include <iostream>
#include <fstream>

using alglib::real_2d_array;
using alglib::real_1d_array;
using alglib::rmatrixsvd;

void print_matrix (const real_2d_array& mA) {
    int i,j;
    cout << "[";
    for (i = 0; i < mA.rows(); i++) {
        cout <<  "[";
        for (j = 0; j < mA.cols() - 1; j++) {
            cout << mA[i][j] << ",";
        }
        cout <<  mA[i][j] << "]\n";
    }
    cout << "]\n";
}

void testScenario() {
    FaceControl fc;
        
    fc.readFile("learning", LEARNING_DOMAIN);
    
    PSubspace rawSpace = fc.getSpace("Raw");    
    PProjection raw2face = fc.makeProjection(rawSpace, "Faces");
    
    fc.projectFaces(raw2face, LEARNING_DOMAIN); //use pair of string names instead?
    real_2d_array cov = fc.calculateCovariation("Faces");
    
    cout << endl << endl << "Faces Covariation" << endl;
    print_matrix(cov);
    cout << endl << endl;

    PProjection raw2samePerson = fc.makeProjectionGroupPid(rawSpace, "samePerson");
    fc.projectFaces(raw2samePerson, LEARNING_DOMAIN); //use pair of string names instead?
    
    PSubspace faceSpace = fc.getSpace("Faces");
    
    PProjection face2idPerson = fc.makeProjectionDistinctPid(faceSpace, "idPerson");
    fc.projectFaces(face2idPerson, LEARNING_DOMAIN); //use pair of string names instead?
    fc.normalizeFaces("idPerson", LEARNING_DOMAIN);

    fc.distanceStats("idPerson");
    
    fc.readFile("samples", SAMPLE_DOMAIN);
    fc.readFile("tests", TEST_DOMAIN);
    
    fc.projectFaces(raw2face, SAMPLE_DOMAIN);
    fc.projectFaces(raw2face, TEST_DOMAIN);
    fc.projectFaces(face2idPerson, SAMPLE_DOMAIN);
    fc.projectFaces(face2idPerson, TEST_DOMAIN);
    fc.normalizeFaces("idPerson", SAMPLE_DOMAIN);
    fc.normalizeFaces("idPerson", TEST_DOMAIN);
//     fc.distanceStats("idPerson");
    fc.sampleTests("idPerson");
}

void analyticLearning () {
    FaceControl fc;
    
    fc.readFile("learning", LEARNING_DOMAIN);
    
    PSubspace rawSpace = fc.getSpace("Raw");    
    PProjection raw2face = fc.makeProjection(rawSpace, "Faces");
    
    fc.projectFaces(raw2face, LEARNING_DOMAIN); //use pair of string names instead?
    real_2d_array cov = fc.calculateCovariation("Faces");
    
    cout << endl << endl << "Faces Covariation" << endl;
    print_matrix(cov);
    cout << endl << endl;

    PProjection raw2samePerson = fc.makeProjectionGroupPid(rawSpace, "samePerson");
    fc.projectFaces(raw2samePerson, LEARNING_DOMAIN); //use pair of string names instead?
    
    PSubspace faceSpace = fc.getSpace("Faces");
    
    PProjection face2idPerson = fc.makeProjectionDistinctPid(faceSpace, "idPerson");
    fc.projectFaces(face2idPerson, LEARNING_DOMAIN); //use pair of string names instead?
    fc.normalizeFaces("idPerson", LEARNING_DOMAIN);

    fc.distanceStats("idPerson");
    fc.printFaces("idPerson", LEARNING_DOMAIN);
}

int main () {
    try {
//      analyticLearning();
        testScenario();
        return 0;
    } catch (const char* e) {
        cout << e << endl;
        return -1;
    }
}
