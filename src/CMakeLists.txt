cmake_minimum_required(VERSION 2.8.4)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
IF(UNIX)
 link_libraries(m)
ENDIF(UNIX)

project (ALCOR) 
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_BUILD_TYPE Release)

add_executable (AlcoR alcor.c pa.c ex.c si.c lr.c kmer.c paint.c stats.c dist.c cm.c chash.c carray.c rand.c tolerant.c pmodels.c levels.c cache.c math.c dna.c keys.c alphabet.c buffer.c param.c strings.c mem.c msg.c common.c threads.h defs.h) 

TARGET_LINK_LIBRARIES(AlcoR pthread)
