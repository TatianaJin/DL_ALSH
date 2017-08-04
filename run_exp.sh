programs='alsh l2lsh linear_scan'
Ks='16 64 256'
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
for K in $Ks; do
    echo "K=$K"

    # ALSH
    echo "ALSH"
    echo "normalize_query=true"
    for i in {1..3}; do
        ./build/alsh $U_factor $V_factor $topk ./exp/alsh_norm_k"$K"_$i $r_ 1 $K $m $U 1
    done
    echo "normalize_query=false"
    for i in {1..3}; do
        ./build/alsh $U_factor $V_factor $topk ./exp/alsh_k"$K"_$i $r_ 1 $K $m $U 0
    done

    # L2LSH
    echo "L2LSH"
    for R in $Rs; do
        echo "R=$R"
        for i in {1..3}; do
            ./build/l2lsh $U_factor $V_factor $topk ./exp/l2lsh_r"$R"_k"$K"_$i $R 1 $K
        done
    done

    # Linear Scan
    echo "Linear Scan"
    for i in {1..3}; do
        ./build/linear_scan $U_factor $V_factor $topk ./exp/linear_scan $nfactors
    done

done
