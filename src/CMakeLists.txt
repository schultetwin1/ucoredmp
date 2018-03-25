add_library(ucoredmp STATIC ucoredmp.c)

target_include_directories(ucoredmp INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
