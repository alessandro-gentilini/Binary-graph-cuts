// author: Alessandro Gentilini, 2014

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

	std::cout << "Max flow: " << flow << "\n";

	assert(g->what_segment(0) == GraphType::SOURCE);
	assert(g->what_segment(1) == GraphType::SOURCE);
	assert(g->what_segment(2) == GraphType::SOURCE);
	assert(g->what_segment(5) == GraphType::SOURCE);

	assert(g->what_segment(3) == GraphType::SINK);
	assert(g->what_segment(4) == GraphType::SINK);

	delete g;
}

int main()
{
	test_Prince_figure_12_6();

	return 0;
}
