#! /usr/bin/env python

import math
import numpy as np
import scipy.sparse
import sys
from sklearn.utils.extmath import randomized_svd
from utils import load

OUTPUT_DIR = "../data"

# Train
def fit(R, nfactors):
    U, sigma, VT = randomized_svd(R, nfactors)
    sigma = scipy.sparse.diags(sigma, 0)
    U = U * sigma
    V = VT.T
    return (U, V)

# Output result
def output(U, V, OUTPUT_DIR):
    with open(OUTPUT_DIR+'/U_factor', 'w') as f:
        f.write('\n'.join(['\t'.join([str(u) for u in row]) for row in U]))
        f.write('\n')
    with open(OUTPUT_DIR+'/V_factor', 'w') as f:
        f.write('\n'.join(['\t'.join([str(v) for v in row]) for row in V]))
        f.write('\n')

def validate(R, U, V):
    R_coo = R.tocoo()
    mse = 0
    for i in xrange(len(R.data)):
        row = R_coo.row[i]
        col = R_coo.col[i]
        pred = np.dot(U[row], V[col])
        mse += (pred - R.data[i])**2
    return math.sqrt(mse/len(R.data))

def main(argc, argv):
    if argc < 5:
        print "usage: ./pureSVD.py <train_path> <num_users> <num_items> <num_latent_factors> <output_directory>"
        exit()

    train_path = argv[1]
    nusers = int(argv[2])
    nitems = int(argv[3])
    nfactors = int(argv[4])
    OUTPUT_DIR = argv[5]

    R = load(train_path, nusers, nitems)
    U, V = fit(R, nfactors)
    print "RMSE on training data is %f" % validate(R, U, V)
    output(U, V, OUTPUT_DIR)

if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
