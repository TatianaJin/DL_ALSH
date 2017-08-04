#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

#include <Eigen/Dense>
#include "utils.hpp"

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cout << "usage: ./linear_scan query_matrix item_matrix topk output_path latent_factors [query_size item_size]" << std::endl;
        return 0;
    }

    // Storing the items sorted in descending order of mip for each query
    std::vector<std::vector<size_t>> results(1000);

    int k = std::atoi(argv[3]);
    char* output_path = argv[4];
    int nfactor = std::atoi(argv[5]);
    int U_size = 1000;
    int V_size = 10681;
    if (argc > 6) {
        U_size = std::atoi(argv[6]);
        if (argc > 7) V_size = std::atoi(argv[7]);
    }

    // U is query matrix, V is item matrix
    Eigen::MatrixXf U(U_size, nfactor);
    Eigen::MatrixXf V(V_size, nfactor);
    // Load the matrices
    load_matrix(U, argv[1], 1);
    load_matrix(V, argv[2]);

    auto start = std::chrono::system_clock::now();
    // Matrix Multiplication
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> ratings = U*V.transpose();
    // DEBUG std::cout << ratings << std::endl;
    size_t cols = ratings.cols();
    size_t rows = ratings.rows();
    for (int row = 0; row < rows; ++row) { // for each query
        auto record = ratings.row(row).data();
        results[row] = std::vector<size_t>(cols);
        std::iota(results[row].begin(), results[row].end(), 0);
        // Sort the items in descending order of mip
        std::sort(results[row].begin(), results[row].end(), [&record](size_t a, size_t b){
            return record[a] > record[b];
        });
    }
    auto end = std::chrono::system_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "time elapsed: " << elapsed << std::endl;

    output(output_path, results, k);
}
