#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <Eigen/Dense>

#include "l2lsh.hpp"

class ALSH_MIP : public L2LSH {
   public:
    ALSH_MIP(int dimension, int k, int l, float r, int m, float u) :
        L2LSH(dimension + m, k, l, r),
        m_(m), u_(u), d_(dimension) {
            assert(u < 1 && u > 0);
        }

    void hash_put(Eigen::MatrixXf x) override { // each column is an item, extended to high dimensions
        Eigen::MatrixXf l2_norms = scale(&x);
        transform_item(&x, &l2_norms);
        L2LSH::hash_put(x);
    }

    std::vector<std::vector<std::string>> hash(Eigen::MatrixXf x) override { // each column is a query
        transform_query(&x);
        return L2LSH::hash(x);
    }

    inline void set_query_normalization(bool b) { normalize_query_ = b; }

   protected:
    Eigen::MatrixXf scale(Eigen::MatrixXf* x) {
        Eigen::MatrixXf query_l2_norms = x->colwise().lpNorm<2>();
        float max = query_l2_norms.maxCoeff();
        *x *= (u_/max);
        query_l2_norms *= (u_/max);
        return query_l2_norms;
    }

    void transform_query(Eigen::MatrixXf* x) {
        int cols = x->cols();
        if (normalize_query_) {
            for (int i = 0; i < cols; ++i) {
                x->col(i) /= x->col(i).lpNorm<2>();
            }
        }
        x->block(d_, 0, m_, cols) = Eigen::MatrixXf::Constant(m_, cols, 0.5);
    }

    void transform_item(Eigen::MatrixXf* x, Eigen::MatrixXf* l2_norms = NULL) {
        int cols = x->cols();
        if (l2_norms == NULL || l2_norms->rows() != 1) {
            *l2_norms = x->block(0,0,d_,cols).colwise().lpNorm<2>();
        }
        for (int i = d_; i < dimension_; ++i) {
            *l2_norms = l2_norms->cwiseProduct(*l2_norms);
            (*x).row(i) = *l2_norms;
        }
    }

    int d_; // original dimension
    int m_; // added dimensions in transformation
    float u_; // scaling factor
    bool normalize_query_ = false;
};
