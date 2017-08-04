programs='alsh l2lsh linear_scan'
m=3
U=0.83
r_=2.5
Rs='1 1.5 2 2.5 3 3.5 4 4.5 5'
U_factor=data/U_factor_1000
V_factor=data/V_factor
topk=10681
norm_query=1 # normalize query
nfactors=150

echo topk=$topk

# different Ks
for K in {5..30}; do
    echo "K=$K"

    for L in {1..200}; do
        # ALSH
        echo "ALSH (normalized query)"
        for i in {1..3}; do
            ./build/alsh_bucketing $U_factor $V_factor $topk ./exp/alsh_norm_k"$K"_$i $r_ $K $L $m $U 1
        done

        # L2LSH
        echo "L2LSH (r=4)"
        for i in {1..3}; do
            ./build/l2lsh_bucketing $U_factor $V_factor $topk ./exp/l2lsh_r"$R"_k"$K"_$i 4 $K $L
        done
    done
done
