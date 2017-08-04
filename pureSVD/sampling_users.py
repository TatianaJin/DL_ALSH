#! /usr/bin/env python

import random

###################################################
#                                                 #
# For for sampling latent user factors as query   #
#                                                 #
# @params: <INPUT_PATH> <OUTPUT_PATH>             #
#                                                 #
###################################################

NUSER = 71567
SAMPLE_SIZE = 1000
USER_FILE = 'data/U_factor'

uids = random.sample(range(NUSER), SAMPLE_SIZE)
uids.sort()

with open(USER_FILE, 'r') as fin:
    with open(USER_FILE + '_' + str(SAMPLE_SIZE), 'w') as fout:
        index = 0
        for uid in uids:
            while index < uid:
                index += 1
                fin.readline()
            fout.write("%d\t%s" % (uid, fin.readline()))
            index += 1
