g++ -O2 -std=c++17 -static-libgcc -static-libstdc++ -pthread \
  -Iassignment_01/include \
  -Iassignment_02/include \
  assignment_01/src/csr.cpp \
  assignment_01/src/graph_algorithms.cpp \
  assignment_02/src/graph_analytics.cpp \
  common_wrapper/wrapper.cpp \
  -o wrapper.exe