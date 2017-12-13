/* This is the cpp file for patient queue,  managing a waiting line of patients
 * to be processed at a hospital.
 * The comments for each function are in the cpp file.
 * Refer to the code on the ppt from lecture15.
 *
 * by Zixuan Zhou
 */
#ifndef _linkedpq_h   // do not modify these two lines
#define _linkedpq_h

#include <iostream>
#include <string>
#include "pnode.h"
using namespace std;

class LinkedPQ {
public:
    LinkedPQ();
    ~LinkedPQ();
    void clear();
    string frontName() const;
    int frontPriority() const;
    bool isEmpty() const;
    void newPatient(string name, int priority);
    string processPatient();
    void upgradePatient(string name, int oldPriority, int newPriority);
    void debug();
    friend ostream& operator <<(ostream& out, const LinkedPQ& queue);

private:
    PNode* PQ_front;// front point of the list
};

ostream& operator <<(ostream& out, const LinkedPQ& queue);

#endif
