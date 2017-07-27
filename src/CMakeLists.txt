set(cdobs_srcs
  config.cpp
  cdobs.cpp
  dbstore.cpp
  create.cpp
  main.cpp
  dberror.cpp
  init.cpp
  utils.cpp)

add_executable(cdobs ${cdobs_srcs})
target_link_libraries(cdobs sqlite3)

add_executable(debug EXCLUDE_FROM_ALL ${cdobs_srcs})
target_link_libraries(debug sqlite3)
set_target_properties(
    debug 
    PROPERTIES 
        OUTPUT_NAME "cdobs"
)
target_compile_definitions(debug PRIVATE IS_DEBUG=1)


