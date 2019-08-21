cmake_minimum_required(VERSION 3.0.0)
project(avif_experiments VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include(CTest)
enable_testing()

include_directories(include)

add_executable(avif_test_jpg_encode src/avif_test_jpg_encode.cpp src/utils.cpp src/y4m.c)
add_executable(avif_decode_test src/avif_decode_test.cpp src/utils.cpp)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_library(AVIF_LIBRARY avif lib)
find_library(AOM_LIBRARY aom lib)
find_library(DAV1D_LIBRARY dav1d lib)
target_link_libraries(avif_test_jpg_encode LINK_PUBLIC ${AVIF_LIBRARY} ${AOM_LIBRARY} LINK_PRIVATE ${OpenCV_LIBS} pthread)
target_link_libraries(avif_decode_test LINK_PUBLIC ${AVIF_LIBRARY} ${AOM_LIBRARY} ${DAV1D_LIBRARY} LINK_PRIVATE ${OpenCV_LIBS} pthread)