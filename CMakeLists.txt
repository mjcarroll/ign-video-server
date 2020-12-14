cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-video-server VERSION 0.0.1)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake2 REQUIRED)

#============================================================================
# Set up the project
#============================================================================
ign_configure_project(VERSION_SUFFIX)

#============================================================================
# Set project-specific options
#============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#============================================================================
# Search for project-specific dependencies
#============================================================================

#--------------------------------------
# Find ignition-common
ign_find_package(ignition-common3 REQUIRED COMPONENTS graphics av)
set(IGN_COMMON_VER ${ignition-common3_VERSION_MAJOR})

#--------------------------------------
# Find ignition-transport
ign_find_package(ignition-transport9 REQUIRED)
set(IGN_TRANSPORT_VER ${ignition-transport9_VERSION_MAJOR})

#--------------------------------------
# Find ignition-msgs
ign_find_package(ignition-msgs6 REQUIRED)
set(IGN_MSGS_VER ${ignition-msgs6_VERSION_MAJOR})

#--------------------------------------
# Find OpenSSL
find_package(OpenSSL REQUIRED)

ign_configure_build(QUIT_IF_BUILD_ERRORS) 

#============================================================================
# Create package information
#============================================================================
ign_create_packages()
