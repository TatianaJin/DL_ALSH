#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <Eigen/Dense>

#include "utils.hpp"
#include "l2lsh.hpp"

int main(int argc, char** argv) {
    if (argc < 8) {
        std::cout << "usage: ./l2lsh U_factor V_factor topk output_path hash_r hash_K hash_L [latent_factors U_size V_size]" << std::endl;
        return 0;
    }
    int d = 150;
    int U_size = 1000;
    int V_size = 10681;
    if (argc > 8) {
        d = std::atoi(argv[8]);
        if (argc > 9) {
            U_size = std::atoi(argv[9]);
            if (argc > 10) V_size = std::atoi(argv[10]);
        }
    }

    Eigen::MatrixXf U(U_size, d);
    Eigen::MatrixXf V(V_size, d);
    // load queries
    load_matrix(U, argv[1], 1);
    // load items
    load_matrix(V, argv[2]);
    int k = std::atoi(argv[3]);
    char* output_path = argv[4];
    float r = std::atof(argv[5]);
    int K = std::atoi(argv[6]);
    int L = std::atoi(argv[7]);

    auto start = std::chrono::system_clock::now();
    // create hash functions
    L2LSH l2lsh(d, K, L, r);
    // hash each item K times into K tables
    l2lsh.hash_put(V.transpose());
    // for each query, hash K times to find the number of matches with all candidate items
    auto query_buckets = l2lsh.hash(U.transpose());
    std::vector<std::unordered_map<int, int>> query_candidates;
    l2lsh.gather(query_buckets, query_candidates);
    // sort the matches and find top k
    auto results = get_topk(query_candidates, k);
    auto end = std::chrono::system_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "time elapsed: " << elapsed << std::endl;

    output(output_path, results, k);
}
