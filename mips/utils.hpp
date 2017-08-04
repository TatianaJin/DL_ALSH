#include <cstdio>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Eigen/Dense>

// Load Matrix: the matrix need to be in right shape
void load_matrix(Eigen::MatrixXf& mat, const char* input_path, bool col_offset = false, int cols = 0) {
    FILE* f_in = fopen(input_path, "r");
    if (f_in == NULL) {
        std::cout << "Error open file: " << input_path << std::endl;
        return;
    }
    if (!cols) cols = mat.cols();
    for (int i = 0; i < mat.rows(); ++i) {
        if (col_offset) fscanf(f_in, "%*d");
        for (int j = 0; j < cols; ++j) {
            fscanf(f_in, "%f", &mat(i,j));
        }
    }
    fclose(f_in);
}

template <typename T>
void output(char* path, std::vector<std::vector<T>>& results, int k) {
    std::ofstream f_out;
    f_out.open(path, std::ofstream::out);

    for (auto record : results) {
        for (int i = 0; i < k && i < record.size(); ++i) {
            if (i) { f_out << "\t" << record[i]; }
            else { f_out << record[i]; }
        }
        f_out << "\n";
    }

    f_out.close();
}

std::vector<int> get_topk(std::unordered_map<int, int>& candidates, int k) {
    std::vector<std::pair<int, int>> kvs;
    kvs.reserve(candidates.size());
    for (auto& kv : candidates) { kvs.push_back(kv); }

    // sort
    std::sort(kvs.begin(), kvs.end(), [](auto a, auto b) { return a.second > b.second; });

    // return top k
    std::vector<int> results;
    results.reserve(k);
    for (int i = 0; i < k && i < kvs.size(); ++i) { results.push_back(kvs[i].first); }
    return results;
}

std::vector<std::vector<int>> get_topk(std::vector<std::unordered_map<int, int>>& query_candidates, int k) {
    std::vector<std::vector<int>> results;
    results.reserve(query_candidates.size());
    for (auto& candidates : query_candidates) {
        results.push_back(get_topk(candidates, k));
    }
    return results;
}

std::vector<std::vector<int>> get_topk(const Eigen::MatrixXf& U, const Eigen::MatrixXf& V,
        std::vector<std::unordered_set<int>>& queries_candidates, int k, int dimension) {
    std::vector<std::vector<int>> results;
    int num_queries = queries_candidates.size();
    results.resize(num_queries);
    int multiplication_count = 0;
    for (int i = 0; i < num_queries; ++i) { // for each query
        auto& query_candidates = queries_candidates[i];

        // collect multiplication count
        multiplication_count += query_candidates.size();

        std::unordered_map<int, float> products;
        products.reserve(query_candidates.size());
        for (int item : query_candidates) { // get ratings for all candidates
            products[item] = (U.block(i, 0, 1, dimension) * V.block(item, 0, 1, dimension).transpose())(0,0);
        }
        results[i] = std::vector<int>(query_candidates.begin(), query_candidates.end());

        std::sort(results[i].begin(), results[i].end(), [&products](int a, int b) {
            return products[a] > products[b];
        });
    }
    std::cout << "multiplication count = " << multiplication_count << std::endl;
    return results;
}
