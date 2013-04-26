#######################################
# test/ CMakeLists.txt
#######################################

include_directories(${CMAKE_SOURCE_DIR})

# Enable ExternalProject CMake module
include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

# Add gtest
ExternalProject_Add(
    googletest
    SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
    TIMEOUT 10
    CMAKE_ARGS -Dgtest_force_shared_crt=ON
    # Disable install step
    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)


# Specify include directory for GTest 
ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/include)

# Specify link directory for GTest
ExternalProject_Get_Property(googletest binary_dir)
link_directories(${binary_dir})

# Create the test executable and add it as a test
add_executable(units_test msgpack-lite_test.cpp)
add_test(units_test units_test)

# Create dependency of units_test on GTest
add_dependencies(units_test googletest)

# Link test agains GTest libraries
target_link_libraries(units_test gtest gtest_main)
