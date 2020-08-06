//
//  main.cpp
//  closestpair
//
//  Created by SIMON KAHAN on 8/5/20.
//  Copyright © 2020 Biocellion SPC. All rights reserved.
//
#include <float.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

class Point {
public:
    double x;
    double y;
    double z;
    Point(double a, double b, double c) : x(a), y(b), z(c) {}
    Point(){};
};
class Pair {
public:
    Point p1, p2;
    Pair(Point a, Point b) : p1(a), p2(b) {}
    Pair(){};
};
double dist(Point p1, Point p2) {
    return ((p1.x-p2.x)*(p1.x-p2.x)+
            (p1.y-p2.y)*(p1.y-p2.y)+
            (p1.z-p2.z)*(p1.z-p2.z));
}
double dist(Pair p) {
    return dist(p.p1, p.p2);
}
#define POINT_NFAR Point(-DBL_MAX,-DBL_MAX,-DBL_MAX)
#define POINT_FAR Point(DBL_MAX,DBL_MAX,DBL_MAX)
#define INF_PAIR Pair(POINT_NFAR,POINT_FAR)
Pair brute_force_search(Point * P, int i, int j) {
    Pair pmin = INF_PAIR;
    double dmin = DBL_MAX;
    if (j <= i) return INF_PAIR;
    for (int a = i; a < j; a++) {
        for (int b = a+1; b <= j; b++) {
            if (dist(P[a], P[b]) < dmin) {
                dmin = dist(P[a], P[b]);
                pmin = Pair(P[a],P[b]);
            }
        }
    }
    return pmin;
}
int compar_1(void * p, const void * i, const void * j) {Point * P = (Point *) p; double y1 = P[*(int*)i].y, y2 = P[*(int*)j].y; return y1 < y2 ? -1 : y1 > y2 ? 1 : 0;}

Pair search_strip(Point *P, int left, int right, double d) {
    int * byY = new int[right-left+1];
    for (int i = 0; i < right-left+1; i++) byY[i] = i+left;
    qsort_r (byY, right-left+1, sizeof(int), P, &compar_1);
    
    Pair pmin = INF_PAIR;
    double dmin = DBL_MAX;
    
    int low_candidate = 0;
    int high_candidate = 1;
    
    while (low_candidate < right-left) {
        while (high_candidate <= right-left && P[byY[high_candidate]].y-P[byY[low_candidate]].y < d ) {
            if (dist(P[byY[high_candidate]],P[byY[low_candidate]]) < dmin) {
                dmin = dist(P[byY[high_candidate]],P[byY[low_candidate]]);
                pmin = Pair(P[byY[low_candidate]],P[byY[high_candidate]]);
            }
            high_candidate++;
        }
        low_candidate++; high_candidate = low_candidate+1;
    }
    free (byY);
    return pmin;
}
/* search returns from points between pointers i & j inclusive
 ** the closest pair of points (i, j);
 */
#define THRESHOLD 4 // at least 2
Pair search(Point * P, int i, int j) {
    if ((j - i) < THRESHOLD) return brute_force_search(P, i, j);
    Pair pmin;
    double dmin;
    Pair p1 = search(P, i, (i+j)/2);
    Pair p2 = search(P, (i+j)/2 + 1, j);
    double d1 = dist(p1);
    double d2 = dist(p2);
    if (d1 > d2) {dmin = d2; pmin = p2;} else {dmin = d1; pmin = p1;}
    
    double midpoint = (P[(i+j)/2].x + P[(i+j)/2+1].x)/2;
    int left = i, right = j;
    while (P[left].x < midpoint - dmin) left ++;
    while (P[right].x > midpoint + dmin) right --;
    if (left >= right) return pmin;
    
    Pair p3 = search_strip(P, left, right, dmin);
    
    double d3 = dist(p3);
    if (dmin > d3) {dmin = d3; pmin = p3;}
    return pmin;
}

int compar_0(const void * xp, const void * yp) {double x = *(double*)xp, y = *(double*)yp; return x < y ? -1 : x > y ? 1 : 0;}

// expects first line of file to contain just an integer count of points
// and remaining lines each have three coordinates per point
// 3
// 1.0 2 3
//
int main(int argc, char** argv) {
    if (argc < 2) std::cerr << "Usage: ./a.out <filename>\n";
    std::ifstream file(argv[1], std::ios::in);
    if (!file.is_open()) {
        std::cerr<<"File "<<argv[1]<<" cannot be opened\n";
        exit(1);
    }
    
    /* read # points */
    int numpoints;
    file >> numpoints;
    Point * P = new Point[numpoints];
    
    /* read points */
    for (int i = 0; i < numpoints; i++) {
        file >> P[i].x;
        file >> P[i].y;
        file >> P[i].z;
    }
    
    /* sort by x-axis */
    qsort (P, numpoints, sizeof(P[0]), &compar_0);
    
    std::cerr << "searching for closest\n";
    /* recursive search for closest */
    Pair pmin = search (P, 0, numpoints-1);
    
    std::cout << "Closest pair is: " <<
    "\n\t" << pmin.p1.x << ":" << pmin.p1.y << ":" << pmin.p1.z <<
    "\n\t" << pmin.p2.x << ":" << pmin.p2.y << ":" << pmin.p2.z <<
    "\nat distance " << sqrt(dist(pmin))<<"\n";
}

