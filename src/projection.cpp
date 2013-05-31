/*
 author: Maxim Volgin (2012)
 mailto: technotopia(at)gmail.com
*/

#include "projection.h"
#include "face.h"
#include "math.h"
#include <ostream>
#include <iostream>
#include <fstream>

void Subspace::setAverage(GroupSum a) {
    for (int i = 0; i < dimmension; i++) {
        average[i] = a.v[i];
    }
}

Projection::Projection (PSubspace sup, PSubspace sub, real_2d_array& v, real_1d_array& w, int first, int last, double weightPower = -0.5) {
    psub = sub;
    psup = sup;
    int rows = sub->getDimmension();
    int cols = sup->getDimmension();
    if (rows != (last - first + 1)) {
        throw("Error in Projection() - bad range of singular vectors/values. Doesnt match subspace dim");
    }
    if ((first < 0) && (last >= cols)) {
        throw("Error in Projection() - bad range of singular vectors/values. Out of bounds");
    }
    
    basis.reserve(cols * rows);
    weights.reserve(rows);
    cout << "Create Projection [" << rows << "X" << cols << "]" << endl;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << v(first + i,j) << " ";
            basis[cols * i + j] = v(first + i,j);
        }
        weights[i] = exp(weightPower * log(w[first + i]));
        cout << endl;
    }

}

PSubspace Projection::getSub() {return psub;}
PSubspace Projection::getSup() {return psup;}

void Projection::project(FaceVector& target, FaceVector& x) {
    if (target.pspace != psub) {
        cout << "Projection::project tried to project into incompatible subspace" << endl;
        return;
    }
    if (x.pspace != psup) {
        cout << "Projection::project tried to project from incompatible subspace" << endl;
        return;
    }
    int rows = psub->getDimmension();
    int cols = psup->getDimmension();

    for (int i = 0; i < rows; i++) {
        target.v[i] = 0;
        for (int j = 0; j < cols; j++) {
            target.v[i] += (x.v[j] - psup->getAverage(j)) * basis[i * cols + j];
        }
        target.v[i] /= weights[i];
    }
}

void GroupSum::push (PFace face) {
    count++;
    for (int i = 0; i < pspace->getDimmension(); i++) {
        v[i] += face->get(i);
    }
}

void GroupSum::getAverage () {
    cout << "GroupSum::getAverage. Count = " << count;
    if (!count) {
        cout << endl;
        return;        
    }
    cout << ", average = ( ";
    for (int i = 0; i < pspace->getDimmension(); i++) {
        v[i] /= count;
        cout << v[i] << " ";        
    }
    cout << ")" << endl;
}

