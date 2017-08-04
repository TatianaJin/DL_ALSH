#! /usr/bin/env python

###################################################################
#                                                                 #
# For preprocessing movielens dataset                             #
# 1. Convert 1-indexed consecutive user ids to 0-indexed ids      #
# 2. Convert nonconsecutive item ids to 0-indexed consecutive ids #
#                                                                 #
# @params: <INPUT_PATH> <OUTPUT_PATH>                             #
#                                                                 #
###################################################################

from datetime import datetime
import sys

if len(sys.argv) < 3:
    print "usage: preprocessing.py <INPUT_PATH> <OUTPUT_PATH>"
    exit(0)

INPUT_PATH = sys.argv[1]
OUTPUT_PATH = sys.argv[2]

item_dict = {}
item_count = 0

with open(OUTPUT_PATH, 'w') as f_out:
    with open(INPUT_PATH, 'r') as f_in:
        print "Start preprocessing"
        t_start = datetime.now()
        for line in f_in:
            user, item, rating = line.strip().split('\t')[:3]
            user = int(user) - 1
            item = int(item)
            if not item_dict.has_key(item):
                item_dict[item] = item_count
                item_count += 1
            item = item_dict[item]
            f_out.write("%d\t%d\t%s\n" % (user, item, rating))
        t_end = datetime.now()
        print "Preprocessing time: %dms" % ((t_end - t_start).total_seconds() * 1000)
        print "Preprocessed %d users and %d items" % (user+1, item_count)
