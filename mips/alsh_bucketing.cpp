#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <Eigen/Dense>

#include "utils.hpp"
// #include "l2lsh.hpp"
#include "alsh_mip.hpp"

int main(int argc, char** argv) {
    if (argc < 10) {
        std::cout << "usage: ./alsh_bucketing U_factor V_factor topk output_path hash_r hash_K hash_L trans_m trans_u [if_normalize_query latent_factors U_size V_size]" << std::endl;
        return 0;
    }
    int d = 150;
    int U_size = 1000;
    int V_size = 10681;
    int k = std::atoi(argv[3]);
    char* output_path = argv[4];
    int r = std::atoi(argv[5]);
    int K = std::atoi(argv[6]);
    int L = std::atoi(argv[7]);
    int m = std::atoi(argv[8]);
    float u = std::atof(argv[9]);
    if (argc == 14) {
        d = std::atoi(argv[11]);
        U_size = std::atoi(argv[12]);
        V_size = std::atoi(argv[13]);
    }

    Eigen::MatrixXf U(U_size, d+m); // each row is a query
    Eigen::MatrixXf V(V_size, d+m);
    // load queries
    load_matrix(U, argv[1], 1, d);
    // load items
    load_matrix(V, argv[2], 0, d);

    auto start = std::chrono::system_clock::now();
    // create hash functions
    ALSH_MIP alsh(d, K, L, r, m, u);
    if (argc == 11 && std::atoi(argv[10]) == 1) {
        alsh.set_query_normalization(true);
    }
    // hash each item K times into K tables
    alsh.hash_put(V.transpose());
    // for each query, hash K times to find the number of matches with all candidate items
    auto query_buckets = alsh.hash(U.transpose());
    std::vector<std::unordered_set<int>> query_candidates;
    alsh.gather(query_buckets, query_candidates);
    // sort the matches and find top k
    auto results = get_topk(U, V, query_candidates, k, d);
    auto end = std::chrono::system_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "time elapsed: " << elapsed << std::endl;

    output(output_path, results, k);
}
