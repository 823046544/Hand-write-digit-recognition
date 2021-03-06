#include "a4_correct.h"
#include "CImg.h"
using namespace cimg_library;
#include <string>
#include <iostream>
#include <errno.h>
#include <sys/wait.h>

using namespace std;

// bfs erase disturb for seperation
void Erase_extra_disturb(CImg<float> &Img, int x, int y) {
	static int threshold = 100;
	if (Img(x, y) > threshold) return;
	Img(x, y) = 255;
	vector< pair<int, int> > L;
	L.clear();
	L.push_back(make_pair(x, y));
	int i = 0, n_x, n_y;
	while (i < L.size()) {
		x = L[i].first;
		y = L[i].second;
		for (int k = 0; k < 8; k++) {
			n_x = x+direct[k][0];
			n_y = y+direct[k][1];
			if (n_x < 0 || n_x >= Img._width || n_y < 0 || n_y >= Img._height) continue;
			if (Img(n_x, n_y) > threshold) continue;
			L.push_back(make_pair(n_x, n_y));
			Img(n_x, n_y) = 255;
		}
		i++;
	}
}

// Expand_white
void Expand_white(CImg<float> &Img) {
	CImg<float> temp = Img;
	temp.fill(0.0f);
	int width = temp._width, height = temp._height;
	int n_x, n_y;
	cimg_forXY(Img, x, y) if (x > 0 && x < width-1 && y > 0 && y < height-1) {
		int flag = 0;
		for (int k = 0; k < 8; k++) {
			n_x = x+direct[k][0];
			n_y = y+direct[k][1];
			if (n_x < 0 || n_x >= Img._width || n_y < 0 || n_y >= Img._height) continue;
			if (Img(n_x, n_y) > 100) {
				flag = 1; break;
			}
		}
		if (flag >= 1) temp(x, y) = 255;
	}
	cimg_forXY(Img, x, y) Img(x, y) = max(Img(x, y), temp(x, y));
}

// Expand_black
void Expand_black(CImg<float> &Img) {
	CImg<float> temp = Img;
	temp.fill(255.0f);
	int width = temp._width, height = temp._height;
	int n_x, n_y;
	cimg_forXY(Img, x, y) if (x > 0 && x < width-1 && y > 0 && y < height-1) {
		int flag = 0;
		for (int k = 0; k < 8; k++) {
			n_x = x+direct[k][0];
			n_y = y+direct[k][1];
			if (n_x < 0 || n_x >= Img._width || n_y < 0 || n_y >= Img._height) continue;
			if (Img(n_x, n_y) < 100) {
				flag = 1; break;
			}
		}
		if (flag >= 1) temp(x, y) = 0;
	} 
	cimg_forXY(Img, x, y) Img(x, y) = min(Img(x, y), temp(x, y));
}

struct box{
	int x, y, w, h, id;
	int ans, l;
	box() {}
	box(int _x, int _y, int _w, int _h, int _id): x(_x), y(_y), w(_w), h(_h), id(_id) {}
	bool operator < (const box &other) const {
		return this->y < other.y;
	}
};
bool _cmp_box_location(const box &a, const box &b) {
	if (a.l < b.l) return true;
	if (a.l > b.l) return false;
	return a.x < b.x;
}
bool _cmp_box_size(const box &a, const box &b) {
	if (a.w*a.h > b.w*b.h) return true;
	return false;
}
void Search_Box(CImg<float> &Img, vector<box> &B) {
	vector< pair<int, int> > L;
	int id = 0;
	cimg_forXY(Img, x, y) if (Img(x, y) < 50 && Img(x, y) > -0.5) {
		L.clear();
		L.push_back(make_pair(x, y));
		id++;
		Img(x, y) = -id;
		int xx, yy, lt = 0, l1 = Img._width, l2 = 0, l3 = Img._height, l4 = 0;
		int n_x, n_y;
		for (int i = 0; i < L.size(); i++) {
			xx = L[i].first;
			yy = L[i].second;
			l1 = min(l1, xx); l2 = max(l2, xx);
			l3 = min(l3, yy); l4 = max(l4, yy);
			for (int k = 0; k < 8; k++) {
				n_x = xx+direct[k][0];
				n_y = yy+direct[k][1];
				if (n_x < 0 || n_x >= Img._width || n_y < 0 || n_y >= Img._height) continue;
				if (Img(n_x, n_y) < 50 && Img(n_x, n_y) > -0.5) {
					L.push_back(make_pair(n_x, n_y));
					Img(n_x, n_y) = -id;
				}
			}
		}
		if ((l2-l1+1)*(l4-l3+1) > 10)
			B.push_back(box(l1, l3, l2-l1+1, l4-l3+1, id));		
	}

	//处理5分割的那一横
	for (int i = 0; i < B.size(); i++) if (B[i].w > 2*B[i].h) {
		swap(B[i], B[B.size()-1]);
		int t = 0, s = B.size()-1;
		for (int j = 1; j < B.size()-1; j++) {
			if (sqr(B[t].x-B[s].x)+sqr(B[t].y-B[s].y) > 
				sqr(B[j].x-B[s].x)+sqr(B[j].y-B[s].y))
					t = j;
		}
		if (sqr(B[t].x-B[s].x)+sqr(B[t].y-B[s].y) > 3000) {
			B.pop_back(); continue;
		}
		for (int x = B[s].x; x < B[s].x+B[s].w; x++)
			for (int y = B[s].y; y < B[s].y+B[s].h; y++) if (fabs(Img(x, y)+B[s].id) < 0.5)
				Img(x, y) = -B[t].id;
		int x = B[t].x, y = B[t].y;
		int x1 = max(B[t].x+B[t].w, B[s].x+B[s].w);
		int y1 = max(B[t].y+B[t].h, B[s].y+B[s].h);
		B[t].x = min(B[t].x, B[s].x);
		B[t].y = min(B[t].y, B[s].y);
		B[t].w = x1 - B[t].x;
		B[t].h = y1 - B[t].y;
		B.pop_back();
	}
	sort(B.begin(), B.end(), _cmp_box_size);
	while (B.size() > 120) B.pop_back();
	for (int i = B.size()-1; i > 0; i--) {
		if (B[B.size()/2].w*B[B.size()/2].h/10 > B[i].w*B[i].h)
			B.pop_back();
		else break;
	}
	sort(B.begin(), B.end());
}

void Canny_Separate(CImg<float> &Paper_Graph) {
	float sigma = 1.5f;
	float threshold = 4.0f;
	CImg<float> Img_edge;
    CannyDiscrete(Paper_Graph, sigma, threshold, Img_edge);

	cimg_forXY(Img_edge, x, y) {
		if (Img_edge(x, y) < 100) Img_edge(x, y) = 0;
		else Img_edge(x, y) = 255;
	}
	Recover(Img_edge, 3);
	int paper_width = Paper_Graph._width;
	int paper_height = Paper_Graph._height;
	int frame_threshold = 10;
	cimg_forXY(Paper_Graph, x, y) {
		if (x <= frame_threshold || y <= frame_threshold || paper_width-x <= frame_threshold || paper_height-y <= frame_threshold) {
			if (Img_edge(x, y) <= 10)
				Detect_edge(Img_edge, x, y);
		}
	}
	float color_threshold = 120;
	cimg_forXY(Paper_Graph, x, y) {
		if (fabs(Img_edge(x, y)-50) < 1) Paper_Graph(x, y) = 255;
		if (Img_edge(x, y) > 250 && Paper_Graph(x, y) < color_threshold+40) Paper_Graph(x, y) = 0;
		if (Paper_Graph(x, y) > color_threshold) Paper_Graph(x, y) = 255;
		else Paper_Graph(x, y) = 0;
	}
	Expand_black(Paper_Graph);
	cimg_forXY(Paper_Graph, x, y) {
		if (x <= frame_threshold || y <= frame_threshold || paper_width-x <= frame_threshold || paper_height-y <= frame_threshold) {
			if (Paper_Graph(x, y) < 100) {
				Erase_extra_disturb(Paper_Graph, x, y);
			}
		}
	}
	// Paper_Graph.display("Origin");
}

int main(int argc, char** argv) {
	
	/* Use A4  correct */
    string file_name = "";
	if (argc == 2) file_name = file_name+*(argv+1);
	else file_name = file_name+"1";
	A4_Correct(file_name);

	/* Input the correct graph */ 
	printf("------------------DIGIT SEPARATION-------------------\n");
	string path = "../Output/"+file_name+".jpg";
	CImg<float> Trans_Graph(path.c_str());
	int width = Trans_Graph._width;
	int height = Trans_Graph._height;
	int paper_width = width*3/4.0; 
	int paper_height = paper_width/210.0*297.0;
	if (paper_height > height) {
		paper_height = height*3/4.0;
		paper_width = paper_height/297.0*210.0;
	}
	Trans_Graph = Trans_Graph.get_channel(0);
	CImg<float> Paper_Graph;
	Paper_Graph.assign(paper_width, paper_height);
	cimg_forXY(Paper_Graph, x, y) {
		Paper_Graph(x, y) = Trans_Graph((width-paper_width)/2+x, (height-paper_height)/2+y);
	}

	/* use canny to separate front and back */
	Canny_Separate(Paper_Graph);	
	
	/* find independent digits */
	vector<box> B;
	B.clear();
	Search_Box(Paper_Graph, B);
	
	/* Draw the boxes */
	CImg<float> temp = Paper_Graph;
	for (int i = 0; i < B.size(); i++) {
		Paper_Graph.draw_line(B[i].x, B[i].y, B[i].x+B[i].w, B[i].y, black);
		Paper_Graph.draw_line(B[i].x, B[i].y, B[i].x, B[i].y+B[i].h, black);
		Paper_Graph.draw_line(B[i].x+B[i].w, B[i].y, B[i].x+B[i].w, B[i].y+B[i].h, black);
		Paper_Graph.draw_line(B[i].x, B[i].y+B[i].h, B[i].x+B[i].w, B[i].y+B[i].h, black);
	}
	// Paper_Graph.display();
	Paper_Graph = temp;

	/* CNN digit recognize */
	printf("------------------CNN PREDICT-------------------\n");
	//half of the max height as threshold
	vector<int> Box_Height;
	Box_Height.clear();
	for (int i = 0; i < B.size(); i++) Box_Height.push_back(B[i].h);
	sort(Box_Height.begin(), Box_Height.end());
	float h_threshold = 1.0 * Box_Height[Box_Height.size() - 15] / 2.0;
	// for (int i = 0; i < B.size(); i++) if (B[i].h > h_threshold)
	// 	h_threshold = B[i].h;
	// h_threshold /= 3;
	B[0].l = 1;
	//separate lines
	for (int i = 1; i < B.size(); i++) {
		if (B[i].y - B[i-1].y > h_threshold) B[i].l = B[i-1].l+1;
		else B[i].l = B[i-1].l; 
	}
	sort(B.begin(), B.end(), _cmp_box_location);

	/*digit recognize*/
	for (int i = 0; i < B.size(); i++) {
		CImg<float> number;
		int _size = round(max(B[i].w+1, B[i].h+1)) * 1.3;
		number.assign(_size, _size);
		number.fill(0.0f);
		for (int x = B[i].x; x < B[i].x+B[i].w; x++)
			for (int y = B[i].y; y < B[i].y+B[i].h; y++) if (fabs(Paper_Graph(x, y)+B[i].id) < 0.5) {
				number(_size/2+x-(B[i].x+B[i].w/2), _size/2+y-(B[i].y+B[i].h/2)) = 255;
			}
		Expand_white(number);
		number.resize(28, 28);
		string temp_dir = "./my_num/"+to_string(i) +".jpg";
		number.save(temp_dir.c_str());
	}
	
	/*output lines flag*/
	freopen("line.csv","w",stdout);
	printf("%d", (int)B.size());
	for (int i = 0; i < B.size(); i++)
		if (i == B.size()-1 || B[i].l < B[i+1].l) printf(",%d", i);
	fclose(stdout);

	printf("--------CALL PY----------\n");
	char command[1000];
	FILE *fp;
	/*将要执行的命令写入buf*/
	string python_order = "python digit_rec.py "+file_name;
	snprintf(command, sizeof(command), python_order.c_str());
	/*执行预先设定的命令，并读出该命令的标准输出*/
	fp = popen(command, "r");
	if(NULL == fp) {
		perror("popen执行失败！");
		exit(1);
	}
	printf("DONE");

}