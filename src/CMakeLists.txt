set(cdobs_srcs
  cdobs.cpp
  dbstore.cpp
  create.cpp
  main.cpp
  dberror.cpp
  init.cpp
  utils.cpp)

add_executable(cdobs ${cdobs_srcs})
target_link_libraries(cdobs sqlite3)

