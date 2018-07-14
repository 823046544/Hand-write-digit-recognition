
#include "canny.h"
#include "hough.h"
#include "convex.h"
#include "CImg.h"
using namespace cimg_library;


const int red[] = {255, 0, 0}, green[] = {0, 255, 0},
			blue[] = {0, 0, 255}, yellow[] = {255, 255, 0},
			black[] = {0, 0, 0}, white[] = {255, 255, 255};
int direct[8][2] = {{0,1},{0,-1},{-1,0},{1,0},{-1,1},{-1,1},{1,1},{1,-1}};
//膨胀 white
void Recover(CImg<float> &Img, int T) {
	CImg<int> temp = Img;
	int maxx = 10 + temp._width * temp._height;
	temp.fill(maxx);
	vector< pair<int, int> > L;
	L.clear();
	int n_x, n_y;
	cimg_forXY(Img, x, y) if (Img(x, y) > 100) {
		temp(x, y) = 0;
		L.push_back(make_pair(x, y));
	}
	for (int i = 0; i < L.size(); i++) {
		int x = L[i].first, y = L[i].second;
		for (int k = 0; k < 8; k++) {
			n_x = x+direct[k][0];
			n_y = y+direct[k][1];
			if (n_x < 0 || n_x >= Img._width || n_y < 0 || n_y >= Img._height) continue;
			if (temp(x, y) + 1 >= temp(n_x, n_y)) continue;
			temp(n_x, n_y) = temp(x, y) + 1;
			L.push_back(make_pair(n_x, n_y));
		}
	}
	cimg_forXY(Img, x, y) if (temp(x, y) <= T)
		Img(x, y) = 255;
}

// from the edge erase the extra edge
void Erase_extra_edge(CImg<float> &Img, int x, int y) {
	Img(x, y) = 0;
	for (int k = 0; k < 8; k++) {
		int n_x = x+direct[k][0];
		int n_y = y+direct[k][1];
		if (n_x < 0 || n_x >= Img._width || n_y < 0 || n_y >= Img._height) continue;
		if (!Img(n_x, n_y)) continue;
		Erase_extra_edge(Img, n_x, n_y);
	}
}


//clear anything except the edge
//100 as edge
//50 as trash
void Detect_edge(CImg<float> &Img_edge, int xx, int yy) {
	Img_edge(xx, yy) = 50;
	vector< pair<int, int> > L;
	L.clear();
	L.push_back(make_pair(xx, yy));
	int i = 0, x, y, n_x, n_y;
	while (i < L.size()) {
		x = L[i].first;
		y = L[i].second;
		for (int k = 0; k < 4; k++) {
			n_x = x+direct[k][0];
			n_y = y+direct[k][1];
			if (n_x < 0 || n_x >= Img_edge._width || n_y < 0 || n_y >= Img_edge._height) continue;
			if (Img_edge(n_x, n_y) == 0) {
				Img_edge(n_x, n_y) = 50;
				L.push_back(make_pair(n_x, n_y));
			} else if (Img_edge(n_x, n_y) == 255) {
				Img_edge(n_x, n_y) = 100;
			}
		}
		i++;
	}
}

// BFS Optimize the area inside of paper
void Optimize(CImg<float> &Trans_Graph, int width, int height, int paper_width, int paper_height) {
	CImg<int> S;
	S.assign(Trans_Graph._width, Trans_Graph._height);
	S.fill(0);
	S(0, 0) = 10;
	vector< pair<int, int> > L;
	L.clear();
	L.push_back(make_pair(0, 0));
	int i = 0, x, y, n_x, n_y;
	while (i < L.size()) {
		x = L[i].first;
		y = L[i].second;
		for (int k = 0; k < 4; k++) {
			n_x = x+direct[k][0];
			n_y = y+direct[k][1];
			if (n_x < 0 || n_x >= width || n_y < 0 || n_y >= height) continue;
			if (n_x >= (width-paper_width)/2 && n_x <= (width-paper_width)/2+paper_width &&
				n_y >= (height-paper_height)/2 && n_y <= (height-paper_height)/2+paper_height) continue;
			if (S(n_x, n_y)) continue;
			S(n_x, n_y) = 10;
			if (Trans_Graph(n_x, n_y, 0) > 23 && Trans_Graph(n_x, n_y, 1) > 23 && Trans_Graph(n_x, n_y, 2) > 23)
				for (int z = 0; z < 3; z++) Trans_Graph(n_x, n_y, z) =  Trans_Graph(x, y, z);
			L.push_back(make_pair(n_x, n_y));
		}
		i++;
	}
}

struct triangle {
	int a, b, c;
	triangle() {}
	triangle(int x, int y, int z) {
		a = x, b = y, c = z;
	}
};

int A4_Correct(string file_name) {
    
    string infile = "../Dataset/";      // required input filename
    infile = infile+file_name+".jpg";

    float sigma = 1.5f;
	float threshold = 4.0f;

    CImg<float> in(infile.c_str());
	float point_index_correct = 1.0;
	if (in._width < 2300) {
		point_index_correct = 2300.0/in._width;
		in.resize(2300, (int)(2300.0/in._width*in._height));
	}
	CImg<float> Origin_Graph = in;
	const int widthIn = in._width;
	const int heightIn = in._height;
	if ( widthIn == 0 || heightIn == 0 ) {
		cerr << "Error when loading input image." << endl;
		return -1;
	}
	in = in.get_channel(0); // ensure greyscale img!

	/*handle the edge*/
	CImg<float> Img_edge;
    CannyDiscrete(in, sigma, threshold, Img_edge);
    Img_edge.display("non-maximum suppression");

	cimg_forXY(Img_edge, x, y) {
		if (Img_edge(x, y) < 100) Img_edge(x, y) = 0;
		else Img_edge(x, y) = 255;
	}
	
	Recover(Img_edge, 10);
	for (int i = Img_edge._width/2-25; i <= Img_edge._width/2+25; i++)
		for (int j = Img_edge._height/2-25; j <= Img_edge._height/2+25; j++)
			Detect_edge(Img_edge, i, j);
	
	cimg_forXY(Img_edge, x, y) {
		if (Img_edge(x, y) == 50) Img_edge(x, y) = 0;
		else if (Img_edge(x, y) == 255) Img_edge(x, y) = 0;
		else if (Img_edge(x, y) == 100) Img_edge(x, y) = 255;
		else Img_edge(x, y) = 0;
	}
	Detect_edge(Img_edge, 0, 0);
	cimg_forXY(Img_edge, x, y) {
		if (Img_edge(x, y) == 50) Img_edge(x, y) = 0;
		else if (Img_edge(x, y) == 255) Img_edge(x, y) = 0;
		else if (Img_edge(x, y) == 100) Img_edge(x, y) = 255;
		else Img_edge(x, y) = 0;
	}
	
	Img_edge.display("OutLine_Detect");

	/*handle the lines*/
	vector< pair<int, int> > lines;
	lines.clear();
	CImg<float> HoughSpace;
	// define thread
	float in_thread = 10.0f;
	float out_thread = 150.0f;
	hough(Img_edge, HoughSpace, lines, in_thread, out_thread);
	const int width = Img_edge._width;
    const int height = Img_edge._height;
    const float diagonal = sqrt(sqr(width)+sqr(height));
    const int offset_n = (int)diagonal;
	printf("offset_n: %d\n", offset_n);

	/*handle the points*/
	printf("Lines\n");
	int m = lines.size();
	printf("%d\n", m);

	vector< pair<int, int> > points;
	const float POINT_EPS = 20.0f;
	CImg<float> Points_Graph;
	Points_Graph = Img_edge;
	Points_Graph.fill(0.0f);
	
	/*draw lines*/
	for (int t = 0; t < m; t++) {
		printf("%d %d\n", lines[t].first, lines[t].second);
		cimg_forXY(Img_edge, x, y) if (!Points_Graph(x, y)) {
			int i = lines[t].first;
			float theta = theta_step*i;
            float offset = (x-width/2.0)*sin(theta)+(y-height/2.0)*cos(theta);
            int offset_int = (int) (offset_n*(offset/diagonal+1));
			if (offset_int == lines[t].second) {
				Points_Graph(x, y) = 255.0f-t;
			}
		}
	}
	Points_Graph.display("Lines");

	/*find points*/
	cimg_forXY(Points_Graph, x, y) if (Points_Graph(x, y) && x > 0 && x < width-1 && y > 0 && y < height-1) {
		int sum = 0;
		for (int i = 0; i < 8; i++) if (Points_Graph(x+direct[i][0], y+direct[i][1]) && Points_Graph(x+direct[i][0], y+direct[i][1]) != Points_Graph(x, y))
			sum = 1;
		if (sum) {
			bool repetition = false;
			for (int i = 0; i < points.size(); i++) {
				float distance = sqrt(sqr(x-points[i].first)+sqr(y-points[i].second));
				if (distance <= POINT_EPS) {
					repetition = true;
					break;
				}
			}
			if (!repetition) points.push_back(make_pair(x, y));
		}
	}
	int _size = points.size();
	printf("points\n%d\n", _size);
	for (int i = 0; i < points.size(); i++) printf("%d %d\n", points[i].first, points[i].second);
	Points_Graph = Origin_Graph;
	Graham(points);
	for (int i = 0; i < points.size(); i++) {
		float x = points[i].first;
		float y = points[i].second;
		Points_Graph.draw_circle(x, y, 25, red, 1);
	}
	Points_Graph.display("Corner");

	/*output to ../Ans/file_name.txt*/
	FILE *fp;
	string output_file_name = "../Ans/"+file_name+".txt";
	fp = fopen(output_file_name.c_str(), "w");
	for (int i = 0; i < points.size(); i++)
		fprintf(fp , "%d %d\n", (int)(points[i].first/point_index_correct), (int)(points[i].second/point_index_correct));
	fclose(fp);

	/*morphe*/
	printf("------------------PAPER CORRECT-------------------\n");
	int paper_width = width*3/4.0;
	int paper_height = paper_width/210.0*297.0;
	if (paper_height > height) {
		paper_height = height*3/4.0;
		paper_width = paper_height/297.0*210.0;
	}
	
	vector< Point > points_B;
	vector< Point > points_A;
	for (int i = 0; i < 4; i++) points_A.push_back(Point(points[i].first, points[i].second));

	points_B.push_back(Point((width-paper_width)/2, (height-paper_height)/2));
	points_B.push_back(Point((width-paper_width)/2+paper_width, (height-paper_height)/2));
	points_B.push_back(Point((width-paper_width)/2+paper_width, (height-paper_height)/2+paper_height));
	points_B.push_back(Point((width-paper_width)/2, (height-paper_height)/2+paper_height));
	
	points_B.push_back(Point(0, height-1));	points_B.push_back(Point(width-1, height-1));
	points_B.push_back(Point(width-1, 0));	points_B.push_back(Point(0, 0));
	points_A.push_back(Point(0, height-1));	points_A.push_back(Point(width-1, height-1));
	points_A.push_back(Point(width-1, 0));	points_A.push_back(Point(0, 0));

	vector<triangle> tri;
	tri.push_back(triangle(3, 4, 5));	tri.push_back(triangle(2, 3, 5));
	tri.push_back(triangle(2, 5, 6));	tri.push_back(triangle(1, 2, 6));
	tri.push_back(triangle(0, 1, 6));	tri.push_back(triangle(0, 6, 7));
	tri.push_back(triangle(0, 4, 7));	tri.push_back(triangle(0, 3, 4));
	tri.push_back(triangle(0, 1, 3));	tri.push_back(triangle(1, 2, 3));

	CImg<float> Trans_Graph = Origin_Graph;
	Trans_Graph.fill(0.0f);
	cimg_forXY(Trans_Graph, x, y) {
		for (int tri_num = 0; tri_num < 10; tri_num++) {
			if (PointInTriangle(Point(x, y), points_B[tri[tri_num].a], points_B[tri[tri_num].b], points_B[tri[tri_num].c])) {
				Point p1 = PointMap(Point(x, y), points_B[tri[tri_num].a], points_B[tri[tri_num].b], points_B[tri[tri_num].c], points_A[tri[tri_num].a], points_A[tri[tri_num].b], points_A[tri[tri_num].c]);
				if (p1.x < 0) p1.x = 0;
				if (p1.x >= width) p1.x = width-1;
				if (p1.y < 0) p1.y = 0;
				if (p1.y >= height) p1.y = height-1;
				for (int z = 0; z < 3; z++) Trans_Graph(x, y, z) = Origin_Graph(p1.x, p1.y, z);
				break;
			}
		}
		if ((x > (width-paper_width)/2.0 && x < (width-paper_width)/2.0+paper_width) && (y > (height-paper_height)/2.0 && y < (height-paper_height)/2.0+paper_height)) {	//in papaer
			double r1 = (x-(width-paper_width)/2.0)/(1.0*paper_width);
			double r2 = (y-(height-paper_height)/2.0)/(1.0*paper_height);
			Point p1(points_A[0].x+(points_A[1].x-points_A[0].x)*r1+(points_A[3].x-points_A[0].x)*r2,
					 points_A[0].y+(points_A[1].y-points_A[0].y)*r1+(points_A[3].y-points_A[0].y)*r2);
			Point p2(points_A[2].x+(points_A[3].x-points_A[2].x)*(1.0-r1)+(points_A[1].x-points_A[2].x)*(1.0-r2),
					 points_A[2].y+(points_A[3].y-points_A[2].y)*(1.0-r1)+(points_A[1].y-points_A[2].y)*(1.0-r2));
			Point p((p1.x+p2.x)/2.0, (p1.y+p2.y)/2.0);
			if (PointInTriangle(p, points_A[tri[8].a], points_A[tri[8].b], points_A[tri[8].c]) || PointInTriangle(p, points_A[tri[9].a], points_A[tri[9].b], points_A[tri[9].c])) {
				for (int z = 0; z < 3; z++) Origin_Graph(p.x, p.y, z);
			} else if (PointInTriangle(p1, points_A[tri[8].a], points_A[tri[8].b], points_A[tri[8].c]) || PointInTriangle(p1, points_A[tri[9].a], points_A[tri[9].b], points_A[tri[9].c])) {
				for (int z = 0; z < 3; z++) Trans_Graph(x, y, z) = Origin_Graph(p1.x, p1.y, z);
			} else if (PointInTriangle(p2, points_A[tri[8].a], points_A[tri[8].b], points_A[tri[8].c]) || PointInTriangle(p2, points_A[tri[9].a], points_A[tri[9].b], points_A[tri[9].c])) {
				for (int z = 0; z < 3; z++) Trans_Graph(x, y, z) = Origin_Graph(p2.x, p2.y, z);
			} else {
				for (int z = 0; z < 3; z++) Trans_Graph(x, y, z) = 255.0f;
			}
		}
	}
	Optimize(Trans_Graph, width, height, paper_width, paper_height);
	Trans_Graph.display();

	string outfile = "../Output/";      // required input filename
	outfile = outfile+file_name+".jpg";
	Trans_Graph.save(outfile.c_str());
}
