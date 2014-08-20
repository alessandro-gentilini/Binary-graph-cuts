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
    typedef Graph<double, double, double> GraphType;
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
    //std::cout << "Max flow: " << flow << "\n";

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
typedef unsigned char pixel_gray_level_t;

#include <sstream>

// Type accessing an image with a 2D coordinates, i.e. row index and column
// index.
class index_2D
{
public:
    index_2D(const index_1D &rr = -1, const index_1D &cc = -1): r(rr), c(cc) {}
    index_1D r, c;
    bool operator==(const index_2D &rhs) const
    {
        return r == rhs.r && c == rhs.c;
    }
    // std::ostream& operator<<(std::ostream& o, const index_2D& p) const
    // {
    //  o << "(" << p.r << "," << p.c << ")";
    //  return o;
    // }

    friend std::ostream &operator<< (std::ostream &stream, const index_2D &p)
    {
        stream << "(" << p.r << "," << p.c << ")";
        return stream;
    }
};

// Map a linear index to a (row, column) index (zero-based index, row-major order).
index_2D map_1D_to_2D(const index_1D &i, const index_1D &ncol)
{
    index_2D result;
    result.r = i / ncol;
    result.c = i % ncol;
    return result;
}

// Map a (row, column) index to a linear index (zero-based index, row-major order).
index_1D map_2D_to_1D(const index_2D &p, const index_1D &ncol)
{
    return p.r * ncol + p.c;
}

// Return the Manhattan distance between two 2D coordinates (aka L1 norm).
index_1D manhattan_distance(const index_2D &p1, const index_2D &p2)
{
    return std::abs(p1.r - p2.r) + std::abs(p1.c - p2.c);
}

// Return true if the two pixels m and n are 4-connected.
bool need_edge(const index_1D &m, const index_1D &n, const index_1D &ncol)
{
    return manhattan_distance(map_1D_to_2D(m, ncol), map_1D_to_2D(n, ncol)) == 1;
}

// P_mn in formula (12.12) of Computer Vision: Models, Learning, and Inference.
double pairwise_term(pixel_gray_level_t w_m, pixel_gray_level_t w_n, double theta_10, double theta_01)
{
    if ( w_m == w_n ) return 0;// diagonal cost
    if ( w_m != 0 && w_n == 0 ) return theta_10;
    if ( w_m == 0 && w_n != 0 ) return theta_01;
    throw 0;
}

// U_n in formula (12.12) of Computer Vision: Models, Learning, and Inference.
double unary_term_source(pixel_gray_level_t w_n, pixel_gray_level_t source)
{
    static const double equality_cost = 0;
    static const double difference_cost = 1;
    return w_n == source ? equality_cost : difference_cost;
}

// U_n in formula (12.12) of Computer Vision: Models, Learning, and Inference.
double unary_term_sink(pixel_gray_level_t w_n, pixel_gray_level_t sink)
{
    static const double equality_cost = 0;
    static const double difference_cost = 1;
    return w_n == sink ? equality_cost : difference_cost;
}

#include <random>
#include <vector>

// Get a double uniform distributed beetween 0 and 1
double GetUniform()
{
    static std::default_random_engine re;
    static std::uniform_real_distribution<double> Dist(0, 1);
    return Dist(re);
}

// See John D. Cook, http://stackoverflow.com/a/311716/15485
void SampleWithoutReplacement
(
    int populationSize,    // size of set sampling from
    int sampleSize,        // size of each sample
    std::vector<int> &samples   // output, zero-offset indicies to selected items
)
{
    // Use Knuth's variable names
    int &n = sampleSize;
    int &N = populationSize;

    int t = 0; // total input records dealt with
    int m = 0; // number of items selected so far
    double u;

    while (m < n)
    {
        u = GetUniform(); // call a uniform(0,1) random number generator

        if ( (N - t)*u >= n - m )
        {
            t++;
        }
        else
        {
            samples[m] = t;
            t++; m++;
        }
    }
}



#include <limits>

void test()
{

    test_Prince_figure_12_6();

    assert(std::numeric_limits<index_1D>::is_integer);
    assert(std::numeric_limits<index_1D>::is_signed);
    const int ncol = 6;
    assert(map_2D_to_1D(index_2D(1, 2), ncol) == 8);
    assert(map_1D_to_2D(8, ncol) == index_2D(1, 2));
    assert(need_edge(8, 7, ncol));
    assert(need_edge(7, 8, ncol));
    assert(need_edge(0, 6, ncol));
    assert(need_edge(0, 7, ncol) == false);

    const size_t sz = 20;
    std::vector<int> hist(sz);
    for ( size_t i = 0; i < 500; i++ )
    {
        ++hist[round(sz * GetUniform())]; // generate
    }
    // for (int i = 0; i<hist.size(); ++i) {
    //  std::cout << i << '\t';
    //  for (int j=0; j<hist[i]; ++j) std::cout << '*';
    //      std::cout << '\n';
    // }

    //  std::vector< int > samples(10);
    // SampleWithoutReplacement(100,10,samples);
    // for (size_t i = 0; i < 10; i++ ) {
    //  std::cout << samples[i] << " ";
    // }
    // std::cout << "\n";
}

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void corrupt( const cv::Mat input, cv::Mat &output, double percentage )
{
    const index_1D N = input.rows * input.cols;
    size_t sz = percentage * N;
    if ( sz > N ) return;

    output = input.clone();
    std::vector< index_1D > points_to_corrupt( sz );
    SampleWithoutReplacement(N, sz, points_to_corrupt);
    index_2D p;
    for ( size_t i = 0; i < sz; i++ )
    {
        p = map_1D_to_2D(points_to_corrupt[i], input.cols);
        output.at<pixel_gray_level_t>(p.r, p.c) = output.at<pixel_gray_level_t>(p.r, p.c) ? 0 : 255;
    }
}

int main(int argc, char **argv)
{
    test();

    if ( argc < 2)
    {
        std::cout << " Usage: " << argv[0] << " image_to_process" << "\n";
        return -1;
    }

    bool do_corruption = true;
    if ( argc == 3 )
    {
        do_corruption = false;
    }

    std::string image_name(argv[1]);
    cv::Mat image = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);

    if ( !image.data )
    {
        std::cout <<  "Could not open or find the image '" << image_name << "'\n";
        return -1;
    }

    cv::threshold(image, image, 128, 255, cv::THRESH_BINARY);
    cv::imwrite("binarized.png", image);

    //std::cout << "\n" << image << "\n\n";

    cv::Mat corrupted;
    if ( do_corruption )
    {
        corrupt(image, corrupted, 0.1);
    }
    else
    {
        corrupted = image.clone();
    }
    cv::imwrite("corrupted.png", corrupted);

    const index_1D N = image.rows * image.cols;
    const index_1D ncols = image.cols;

    const double theta_10 = 1;
    const double theta_01 = 1;

    const pixel_gray_level_t source_grey_value = 0;
    const pixel_gray_level_t sink_grey_value = 255;

    //std::cout << "source=" << (int)source_grey_value << " sink=" << (int)sink_grey_value << "\n\n";

    typedef Graph<double, double, double> GraphType;
    // Initialize graph to empty
    GraphType *g = new GraphType(N, 4 * N);

    // Add all the nodes in one instruction
    g->add_node(N);

    for ( index_1D n = 0; n < N; n++ )
    {
        index_2D p_n = map_1D_to_2D(n, ncols);
        pixel_gray_level_t w_n = corrupted.at<pixel_gray_level_t>(p_n.r, p_n.c);

        // Create edges from source and to sink and set capacity to zero
        pixel_gray_level_t unary_source = unary_term_source(w_n, source_grey_value);
        pixel_gray_level_t unary_sink = unary_term_sink(w_n, sink_grey_value);
        g->add_tweights( n, unary_source, unary_sink );

        //std::cout << "n=" << n << " 2D=" << p_n << " v=" << (int)w_n << " c_source=" << (int)unary_source << " c_sink=" << (int)unary_sink << "\n";

        // If edge between m and n is desired
        for ( index_1D m = 0; m < n; m++ )
        {
            if ( need_edge(m, n, ncols) )
            {
                index_2D p_m = map_1D_to_2D(m, ncols);
                pixel_gray_level_t w_m = corrupted.at<pixel_gray_level_t>(p_m.r, p_m.c);

                double c_mn = pairwise_term(w_m, w_n, theta_10, theta_01);
                double c_nm = pairwise_term(w_n, w_m, theta_10, theta_01);

                g->add_edge(m, n, c_mn, c_nm);
                //std::cout << "\t2D_m=" << p_m << " v=" << (int)w_m << " 2D_n=" << p_n << " v=" << (int)w_n << " c_mn=" << c_mn << " c_nm=" << c_nm << "\n";
            }
        }
    }

    std::cerr << "\n\nWARNING: REPARAMETERIZATION NOT EXECUTED.\n\n";

    double flow = g -> maxflow();

    cv::Mat result = corrupted.clone();
    for ( index_1D n = 0; n < N; n++ )
    {
        const index_2D p = map_1D_to_2D(n, ncols);
        if (g->what_segment(n) == GraphType::SOURCE)
        {
            result.at<pixel_gray_level_t>(p.r, p.c) = source_grey_value;
        }
        else if (g->what_segment(n) == GraphType::SINK)
        {
            result.at<pixel_gray_level_t>(p.r, p.c) = sink_grey_value;
        }
    }

    cv::imwrite("result.png", result);

    cv::Mat flipped = result.clone();
    for ( index_1D n = 0; n < N; n++ )
    {
        const index_2D p = map_1D_to_2D(n, ncols);
        flipped.at<pixel_gray_level_t>(p.r, p.c) = result.at<pixel_gray_level_t>(p.r, p.c) ? 0 : 255;
    }

    cv::imwrite("flipped.png", flipped);

    std::string opencv_working_version = "2.4.4";
    std::string current_opencv_version = CV_VERSION;
    if ( opencv_working_version != current_opencv_version ) {
    	std::cerr << "\n\nWARNING: TESTED WITH OPENCV " << opencv_working_version << ". IT SEEMS YOU HAVE VERSION " << current_opencv_version << " INSTEAD.\n\n";
    }

    return 0;
}
