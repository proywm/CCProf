# CMake generated Testfile for 
# Source directory: /home/proy/CCProf/benchmarks/kripke/kripke-openmp-1.1
# Build directory: /home/proy/CCProf/benchmarks/kripke/kripke-openmp-1.1/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_default_dgz "kripke" "--nest" "dgz" "--test")
add_test(test_default_dzg "kripke" "--nest" "dzg" "--test")
add_test(test_default_gdz "kripke" "--nest" "gdz" "--test")
add_test(test_default_gzd "kripke" "--nest" "gzd" "--test")
add_test(test_default_zdg "kripke" "--nest" "zdg" "--test")
add_test(test_default_zgd "kripke" "--nest" "zgd" "--test")
add_test(test_zset_dgz_block "kripke" "--nest" "dgz" "--test" "--zset" "2,3,5" "--gset" "3" "--groups" "6" "--layout" "0")
add_test(test_zset_dzg_block "kripke" "--nest" "dzg" "--test" "--zset" "2,3,5" "--gset" "3" "--groups" "6" "--layout" "0")
add_test(test_zset_gdz_block "kripke" "--nest" "gdz" "--test" "--zset" "2,3,5" "--gset" "3" "--groups" "6" "--layout" "0")
add_test(test_zset_gzd_block "kripke" "--nest" "gzd" "--test" "--zset" "2,3,5" "--gset" "3" "--groups" "6" "--layout" "0")
add_test(test_zset_zdg_block "kripke" "--nest" "zdg" "--test" "--zset" "2,3,5" "--gset" "3" "--groups" "6" "--layout" "0")
add_test(test_zset_zgd_block "kripke" "--nest" "zgd" "--test" "--zset" "2,3,5" "--gset" "3" "--groups" "6" "--layout" "0")
add_test(test_zset_dgz_scatter "kripke" "--nest" "dgz" "--zset" "2,3,5" "--gset" "3" "--groups" "6" "--layout" "1")
add_test(test_bj_dgz "kripke" "--nest" "dgz" "--pmethod" "bj")
subdirs("src")
