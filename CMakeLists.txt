cmake_minimum_required(VERSION 2.8)
PROJECT( binary_graph_cuts )
ADD_EXECUTABLE( binary_graph_cuts maxflow-v3.03.src/graph.cpp maxflow-v3.03.src/maxflow.cpp test.cpp)
SET(CMAKE_CXX_FLAGS "-std=c++0x")
