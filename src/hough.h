#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;
#include "CImg.h"
using namespace cimg_library;

float sqr(float x) { return x*x; }

const int theta_n = 180;
const float theta_step = 1.0*cimg::PI/(float)theta_n;

void hough(const CImg<float> &img, CImg<float> &HoughSpace,
            std::vector<std::pair<int,int> > &lines, float in_thresh, float out_thread) {
    const int width = img._width;
    const int height = img._height;
    const float diagonal = sqrt(sqr(width)+sqr(height));
    const int offset_n = (int)diagonal;

    HoughSpace.assign(theta_n, 2*offset_n);
    HoughSpace.fill(0.0f);
    cimg_forXY(img, x, y) {
        if (img(x, y) < in_thresh) continue;
        for (int i = 0; i < theta_n; i++) {
            float theta = theta_step*i;
            float offset = (x-width/2.0)*sin(theta)+(y-height/2.0)*cos(theta);
            int offset_int = round(offset_n*(offset/diagonal+1));
            HoughSpace(i, offset_int)++;
        }
    }
    // HoughSpace.display();
    lines.clear();
    // cimg_forXY(HoughSpace, i, offset) {
    for (int i = 0; i < theta_n; i++)
    for (int offset = 0; offset < 2*offset_n; offset++) {
        if (HoughSpace(i, offset) > out_thread) {
            bool repetition = false;
            for (int t = 0; t < lines.size(); t++)
                if (abs(i-lines[t].first) <= 20 && abs(offset-lines[t].second) <= 300) {
                    repetition = true;
                    if (HoughSpace(i, offset) > HoughSpace(lines[t].first, lines[t].second)) {
                        lines[t].first = i;
                        lines[t].second = offset;
                    }
                    break;
                }
            if (lines.size() >= 4) break;
            if (!repetition && offset > 1) {
                lines.push_back(make_pair(i, offset));
            }
            if (lines.size() >= 4) break;
        }
    }

}