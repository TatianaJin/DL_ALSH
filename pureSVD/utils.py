#! /usr/bin/env python

import numpy as np
import progressbar
import scipy.sparse

# Load Data
def load_matrix(file_path):
    M = []
    with open(file_path) as f:
        for line in f:
            tmp = line.strip().split('\t')
            M.append([float(v) for v in tmp])
    return np.array(M)

def preprocess_load(train_path, nusers, nitems, offset=0, debug=False):
    I, J, V = [], [], []
    item_dict = {}
    item_count = 0;
    bar = progressbar.ProgressBar(maxval=10**7, widgets=["Loading data: ",
     progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage(), ' ', progressbar.ETA()]).start()
    with open(train_path) as f:
        for i, line in enumerate(f):
            if (i % 1000) == 0:
                bar.update(i % bar.maxval)
            try:
                user, item, rating = line.strip().split('\t')[:3]
                user = int(user)-offset
                if not item_dict.has_key(item):
                    item_dict[item] = item_count
                    item_count += 1
                I.append(user)
                J.append(item_dict[item])
                V.append(float(rating))
            except:
		if debug:
                    print "Ignoring Input: ", line
    R = scipy.sparse.csr_matrix((V, (I, J)), shape=(nusers, nitems))
    return R

def load(train_path, nusers, nitems, debug=False):
    I, J, V = [], [], []
    bar = progressbar.ProgressBar(maxval=10**7, widgets=["Loading data: ",
     progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage(), ' ', progressbar.ETA()]).start()
    with open(train_path) as f:
        for i, line in enumerate(f):
            if (i % 1000) == 0:
                bar.update(i % bar.maxval)
            try:
                user, item, rating = line.strip().split('\t')[:3]
                I.append(int(user))
                J.append(int(item))
                V.append(float(rating))
            except:
		if debug:
                    print "Ignoring Input: ", line
    R = scipy.sparse.csr_matrix((V, (I, J)), shape=(nusers, nitems))
    return R
