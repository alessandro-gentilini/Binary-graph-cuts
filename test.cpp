// author: Alessandro Gentilini, 2014

#include <iostream>
#include "maxflow-v3.03.src/graph.h"

void test_Prince_figure_12_6()
{
	typedef Graph<double,double,double> GraphType;
	GraphType *g = new GraphType(/*estimated # of nodes*/ 6, /*estimated # of edges*/ 16);

	g -> add_node(6);

	g -> add_tweights( 0,   /* capacities */  8.1, 0 );
	g -> add_tweights( 1,   /* capacities */  7.8, 0 );
	g -> add_tweights( 2,   /* capacities */  7.4, 0 );
	g -> add_tweights( 3,   /* capacities */  0, 8.2 );
	g -> add_tweights( 4,   /* capacities */  0, 9.1 );
	g -> add_tweights( 5,   /* capacities */  0, 4.1 );

	g -> add_edge( 0, 1,    /* capacities */  7.1, 0 );
	g -> add_edge( 0, 3,    /* capacities */  1.5, 6.4 );
	g -> add_edge( 1, 2,    /* capacities */  2.4, 1.7 );
	g -> add_edge( 1, 3,    /* capacities */  0.9, 5.2 );
	g -> add_edge( 1, 4,    /* capacities */  2.9, 3.5 );
	g -> add_edge( 2, 4,    /* capacities */  0, 7.5 );
	g -> add_edge( 2, 5,    /* capacities */  7.1, 0 );
	g -> add_edge( 3, 4,    /* capacities */  7.1, 0 );
	g -> add_edge( 4, 5,    /* capacities */  0, 1.3 );

	int flow = g -> maxflow();

	std::cout << "Flow = " << flow << "\n\n";
	std::cout << "Minimum cut:\n";

	for ( GraphType::node_id i = 0; i < 6; i++) {
		if (g->what_segment(i) == GraphType::SOURCE)
			std::cout << "node " << i << " is in the SOURCE set\n";
		else
			std::cout << "node " << i << " is in the SINK set\n";
	}

	delete g;
}

int main()
{
	test_Prince_figure_12_6();

	return 0;
}
