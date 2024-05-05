run_for_seed() {
    SEED=$1
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./build/ulci  <(./genexpr/build/main $SEED) \
        2>&1 | grep "ERROR SUMMARY"
}

TERM_NOT_REDUCING=(46 337)
PARSED_OK=(54 103 116 117 164 197 223 227 329 1117 1213 1253 1481 1575 1782 1843 1850 1851 1898 1906)
TOO_MANY_REDUCTIONS=(85 183 195 300 414 505 530 756 1037 1044 1050 1108 1153 1592 1688 1796 1933 1959)

TOO_MANY_REDUCTIONS_LARGE=(29 279 572 771 967 1020 1533)


for SEED in "${@}"; do
    printf "SEED: %d " $SEED
    run_for_seed $SEED 
done
