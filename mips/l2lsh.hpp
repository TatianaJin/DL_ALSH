#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <Eigen/Dense>

class L2LSH {
   public:
    L2LSH(int dimension, int k, int l, float r) :
        dimension_(dimension),
        k_(k), l_(l), r_(r),
        hash_tables_(l) {
            get_hash_functions();
    }

    virtual void hash_put(Eigen::MatrixXf x) { // each column is an item
        assert(x.rows() == dimension_);
        // get hashed values for each item (per column)
        Eigen::MatrixXi hashed = (((a_*x).colwise() + b_) / r_).unaryExpr([](float f){ return static_cast<int>(std::floor(f)); });
        for (int col = 0; col < hashed.cols(); ++col) {
            std::stringstream bucket_id;
            int table_idx = 0;
            for (int i = 0; i < k_*l_; ++i) {
                if (i && i % k_ == 0) {
                    hash_tables_[table_idx][bucket_id.str()].push_back(col);
                    ++table_idx;
                    bucket_id.clear();
                }
                bucket_id << hashed(i,col);
            }
            hash_tables_[table_idx][bucket_id.str()].push_back(col);
        }
    }

    virtual std::vector<std::vector<std::string>> hash(Eigen::MatrixXf x) { // each column is a query
        assert(x.rows() == dimension_);
        std::vector<std::vector<std::string>> query_buckets(x.cols());
        Eigen::MatrixXi hashed = (((a_*x).colwise()+b_) / r_).unaryExpr([](float f) { return static_cast<int>(std::floor(f)); });
        for (int col = 0; col < hashed.cols(); ++col) {
            std::stringstream bucket_id;
            int table_idx = 0;
            query_buckets[col].reserve(l_);
            for (int i = 0; i < k_*l_; ++i) {
                if (i && i % k_ == 0) {
                    query_buckets[col].push_back(bucket_id.str());
                    ++table_idx;
                    bucket_id.clear();
                }
                bucket_id << hashed(i,col);
            }
            query_buckets[col].push_back(bucket_id.str());
        }
        return query_buckets;
    }

    // Process quries in batches
    void gather(std::vector<std::vector<std::string>>& query_buckets, std::vector<std::unordered_map<int, int>>& query_candidates) {
        auto num_queries = query_buckets.size();
        query_candidates.resize(num_queries);
        for (int i = 0; i < num_queries; ++i) {
            gather(query_buckets[i], query_candidates[i]);
        }
    }
    void gather(std::vector<std::vector<std::string>>& query_buckets, std::vector<std::unordered_set<int>>& query_candidates) {
        auto num_queries = query_buckets.size();
        query_candidates.resize(num_queries);
        for (int i = 0; i < num_queries; ++i) {
            gather(query_buckets[i], query_candidates[i]);
        }
    }

    // gather the number of hash collisions of each candidate
    void gather(std::vector<std::string>& bucket_ids, std::unordered_map<int, int>& candidates) {
        assert(bucket_ids.size() == l_);
        for (int i = 0; i < l_; ++i) {
            for (auto item_id : hash_tables_[i][bucket_ids[i]]) {
                if (candidates.find(item_id) != candidates.end()) {
                    candidates[item_id] += 1;
                } else {
                    candidates[item_id] = 1;
                }
            }
        }
    }

    // gather the set of candidates
    void gather(std::vector<std::string>& bucket_ids, std::unordered_set<int>& candidates) {
        assert(bucket_ids.size() == l_);
        for (int i = 0; i < l_; ++i) {
            for (auto item_id : hash_tables_[i][bucket_ids[i]]) {
                candidates.insert(item_id);
            }
        }
    }

   protected:
    // construct K*L hash function parameters
    void get_hash_functions() {
        std::random_device rd;
        std::mt19937 gen(rd());
        // get a_
        std::normal_distribution<float> a_d(0,1);
        a_ = Eigen::MatrixXf::NullaryExpr(k_*l_, dimension_, [&gen, &a_d]() { return a_d(gen); });
        // get b_
        std::uniform_real_distribution<float> b_d(0, r_);
        b_ = Eigen::VectorXf::NullaryExpr(k_*l_, [&gen, &b_d](){ return b_d(gen); });
    }

    int dimension_; // dimensionality of items / queries
    int k_; // number of concatenations per hash table
    int l_; // number of hash tables
    float r_; // r in hash function (ax+b)/r
    Eigen::MatrixXf a_; // a in hash function (ax+b)/r
    Eigen::VectorXf b_; // b in hash function (ax+b)/r
    std::vector<std::unordered_map<std::string, std::vector<int>>> hash_tables_; // sparse hash tables
};
