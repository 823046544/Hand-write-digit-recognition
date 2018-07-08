
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int sgn(double x) {
    if (fabs(x) < 1e-6) return 0;
    if (x < 0) return -1;
    else return 1;
}

struct Point {
    int x, y;
    Point() {x = y = 0;}
    Point(int _x, int _y): x(_x), y(_y) {}
    Point operator -(const Point &other) const {
        return Point(x-other.x, y-other.y);
    }
    double operator ^(const Point &b) const {
        return x*b.y - y*b.x;
    }
    double operator *(const Point &b) const {
        return x*b.x + y*b.y;
    }
};

struct Line {
    Point s, e;
    Line(){}
    Line(Point _s,Point _e) {
        s = _s; e = _e;
    }
    Point operator &(const Line &b)const {
        Point res = e;
        if (sgn((s-e)^(b.s-b.e)) == 0) {
            if(sgn((s-b.e)^(b.s-b.e)) == 0) return res;//重合
            else return res;//平行 tan90
        }
        res = s;
        double t = (((s-b.s)^(b.s-b.e))*1.0)/(((s-e)^(b.s-b.e))*1.0);
        res.x += (e.x-s.x)*t;
        res.y += (e.y-s.y)*t;
        return res;
    }
};

double dist(Point &p, Point &q) {
    return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

bool PointInTriangle(Point p, Point a, Point b, Point c) {
    float signOfTrig = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
    float signOfAB = (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
    float signOfCA = (a.x - c.x)*(p.y - c.y) - (a.y - c.y)*(p.x - c.x);
    float signOfBC = (c.x - b.x)*(p.y - c.y) - (c.y - b.y)*(p.x - c.x);
    bool d1 = (signOfAB * signOfTrig > 0);
    bool d2 = (signOfCA * signOfTrig > 0);
    bool d3 = (signOfBC * signOfTrig > 0);
    return d1 && d2 && d3;
}

//p, tri(a, b, c), tri(a1, b1, c1) => p1
Point PointMap(Point p, Point a, Point b, Point c, Point a1, Point b1, Point c1) {
    Point p1 = p, d, d1;
    d = Line(a, p) & Line(b, c);
    double r1 = dist(b, d)/dist(b, c);
    double r2 = dist(a, p)/dist(a, d);
    d1.x = b1.x+r1*(c1.x-b1.x);
    d1.y = b1.y+r1*(c1.y-b1.y);

    p1.x = a1.x+r2*(d1.x-a1.x);
    p1.y = a1.y+r2*(d1.y-a1.y);
    return p1;
}

Point p0;
bool _cmp(Point p1, Point p2) {
    double tmp = (p1-p0)^(p2-p0);
    if(sgn(tmp) > 0)return true;
    else if(sgn(tmp) == 0 && sgn(dist(p1,p0) - dist(p2,p0)) <= 0)
    return true;
    else return false;
}
void Graham(vector< pair<int, int> > &points) {
    int n = points.size(), k = 0, top;
    Point list[100];
    int Stack[100];
    for (int i = 0; i < n; i++) list[i] = Point(points[i].first, points[i].second);
    points.clear();
    p0 = list[0];
    Point pO(0, 0);
    //找最下边的一个点
    for(int i = 1;i < n;i++) {
        //if( (p0.y > list[i].y) || (p0.y == list[i].y && p0.x > list[i].x) ) {
        if (dist(pO, p0) > dist(pO, list[i])) {
            p0 = list[i];
            k = i;
        }
    }
    swap(list[k],list[0]);
    sort(list+1, list+n, _cmp);
    if (n == 1) {
        top = 1;
        Stack[0] = 0;
        return;
    } else if (n == 2) {
        top = 2;
        Stack[0] = 0;
        Stack[1] = 1;
        return ;
    } else {
        Stack[0] = 0;
        Stack[1] = 1;
        top = 2;
        for(int i = 2;i < n;i++) {
            while (top > 1 && sgn((list[Stack[top-1]]-list[Stack[top-2]])^(list[i]-list[Stack[top-2]])) <= 0) top--;
            Stack[top++] = i;
        }
    }
    for (int i = 0; i < top; i++) {
        points.push_back(make_pair(list[Stack[i]].x, list[Stack[i]].y));
    }
}
