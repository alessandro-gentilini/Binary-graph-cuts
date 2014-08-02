// author: Alessandro Gentilini, 2014

// A C++ implementation of the Algorithm 12.1, *Binary graph cuts* as found in the
// [*Algorithm Booklet*](http://www.cs.ucl.ac.uk/staff/s.prince/book/Algorithms.pdf)
// by [Simon J.D. Prince](http://www0.cs.ucl.ac.uk/staff/s.prince/).

// The Algorithm Booklet is a companion to the book
// [*Computer Vision: Models, Learning, and Inference*]
// (http://www.computervisionmodels.com/) by Simon J.D. Prince.

// The maximum flow algorithm is [MAXFLOW version 3.03]
// (http://pub.ist.ac.at/~vnk/software/maxflow-v3.03.src.zip) based on the paper:

// BOYKOV, Yuri; KOLMOGOROV, Vladimir.
// An experimental comparison of min-cut/max-flow algorithms for energy minimization in vision.
// *Pattern Analysis and Machine Intelligence, IEEE Transactions on*, 2004, 26.9: 1124-1137.

#include <iostream>
#include "maxflow-v3.03.src/graph.h"

// Test for the example in Figure 12.6 of Computer Vision: Models, Learning, and Inference.
void test_Prince_figure_12_6()
{
	typedef Graph<double,double,double> GraphType;
	GraphType *g = new GraphType(6, 16);

	g -> add_node(6);

	g -> add_tweights( 0, 8.1, 0   );
	g -> add_tweights( 1, 7.8, 0   );
	g -> add_tweights( 2, 7.4, 0   );
	g -> add_tweights( 3, 0  , 8.2 );
	g -> add_tweights( 4, 0  , 9.1 );
	g -> add_tweights( 5, 0  , 4.1 );

	g -> add_edge( 0, 1, 7.1, 0   );
	g -> add_edge( 0, 3, 1.5, 6.4 );
	g -> add_edge( 1, 2, 2.4, 1.7 );
	g -> add_edge( 1, 3, 0.9, 5.2 );
	g -> add_edge( 1, 4, 2.9, 3.5 );
	g -> add_edge( 2, 4, 0  , 7.5 );
	g -> add_edge( 2, 5, 7.1, 0   );
	g -> add_edge( 3, 4, 7.1, 0   );
	g -> add_edge( 4, 5, 0  , 1.3 );

	double flow = g -> maxflow();

	// I do not know what the correct value should be.
	std::cout << "Max flow: " << flow << "\n";

	// The segmentation looks correct.
	assert(g->what_segment(0) == GraphType::SOURCE);
	assert(g->what_segment(1) == GraphType::SOURCE);
	assert(g->what_segment(2) == GraphType::SOURCE);
	assert(g->what_segment(5) == GraphType::SOURCE);

	assert(g->what_segment(3) == GraphType::SINK);
	assert(g->what_segment(4) == GraphType::SINK);

	delete g;
}

typedef int index_1D;

// Type accessing an image with a 2D coordinates, i.e. row index and column
// index.
class index_2D
{
public:
	index_2D(const index_1D& rr=-1, const index_1D& cc=-1):r(rr),c(cc){}
	index_1D r,c;
	bool operator==(const index_2D& rhs) const{return r==rhs.r && c==rhs.c;}
};

// Map a linear index to a (row, column) index (zero-based index, row-major order).
index_2D map_1D_to_2D(const index_1D& i, const index_1D& ncol)
{
	index_2D result;
	result.r = i/ncol;
	result.c = i%ncol;
	return result;
}

// Map a (row, column) index to a linear index (zero-based index, row-major order).
index_1D map_2D_to_1D(const index_2D& p, const index_1D& ncol)
{
	return p.r*ncol+p.c;
}

// Return the Manhattan distance between two 2D coordinates (aka L1 norm).
index_1D manhattan_distance(const index_2D& p1, const index_2D& p2)
{
	return std::abs(p1.r-p2.r)+std::abs(p1.c-p2.c);
}

// Return true if the two pixels m and n are 4-connected.
bool need_edge(const index_1D& m, const index_1D& n, const index_1D& ncol)
{
	return manhattan_distance(map_1D_to_2D(m,ncol),map_1D_to_2D(n,ncol)) == 1;
}

#include <limits>

void test()
{
	test_Prince_figure_12_6();

	assert(std::numeric_limits<index_1D>::is_integer);
	assert(std::numeric_limits<index_1D>::is_signed);
	const int ncol = 6;
	assert(map_2D_to_1D(index_2D(1,2),ncol)==8);
	assert(map_1D_to_2D(8,ncol)==index_2D(1,2));
	assert(need_edge(8,7,ncol));
	assert(need_edge(7,8,ncol));
	assert(need_edge(0,6,ncol));
	assert(need_edge(0,7,ncol)==false);
}

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
int main(int argc, char** argv)
{
	test();

  if( argc != 2)
  {
   std::cout <<" Usage: " << argv[0] << " image_to_process" << "\n";
   return -1;
  }

  std::string image_name(argv[1]);
  cv::Mat image = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);

  if( !image.data )
  {
      std::cout <<  "Could not open or find the image '" << image_name << "'\n";
      return -1;
  }

	return 0;
}
