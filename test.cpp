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

	// I do not what the correct value should be.
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

// Map a linear index to a (row, column) index.
index_2D map_1D_to_2D(const index_1D& i, const index_1D& ncol)
{
	index_2D result;
	result.r = i/ncol;
	result.c = i%ncol;
	return result;
}

// Map a (row, column) index to a linear index.
index_1D map_2D_to_1D(const index_2D& p, const index_1D& ncol)
{
	return p.r*ncol+p.c;
}

#include <limits>
int main()
{
	test_Prince_figure_12_6();
	assert(std::numeric_limits<index_1D>::is_integer);
	assert(map_2D_to_1D(index_2D(1,2),6)==8);
	assert(map_1D_to_2D(8,6)==index_2D(1,2));

	return 0;
}
