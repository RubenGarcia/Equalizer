# generated by Buildyard, do not edit.

include(System)
list(APPEND FIND_PACKAGES_DEFINES ${SYSTEM})
find_package(PkgConfig)

set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
if(PKG_CONFIG_EXECUTABLE)
  find_package(vmmlib 1.7.0)
  if((NOT vmmlib_FOUND) AND (NOT VMMLIB_FOUND))
    pkg_check_modules(vmmlib vmmlib>=1.7.0)
  endif()
  if((NOT vmmlib_FOUND) AND (NOT VMMLIB_FOUND))
    message(FATAL_ERROR "Could not find vmmlib")
  endif()
else()
  find_package(vmmlib 1.7.0  REQUIRED)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(Lunchbox 1.9)
  if((NOT Lunchbox_FOUND) AND (NOT LUNCHBOX_FOUND))
    pkg_check_modules(Lunchbox Lunchbox>=1.9)
  endif()
  if((NOT Lunchbox_FOUND) AND (NOT LUNCHBOX_FOUND))
    message(FATAL_ERROR "Could not find Lunchbox")
  endif()
else()
  find_package(Lunchbox 1.9  REQUIRED)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(Collage 1.1)
  if((NOT Collage_FOUND) AND (NOT COLLAGE_FOUND))
    pkg_check_modules(Collage Collage>=1.1)
  endif()
  if((NOT Collage_FOUND) AND (NOT COLLAGE_FOUND))
    message(FATAL_ERROR "Could not find Collage")
  endif()
else()
  find_package(Collage 1.1  REQUIRED)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(OpenGL )
  if((NOT OpenGL_FOUND) AND (NOT OPENGL_FOUND))
    pkg_check_modules(OpenGL OpenGL)
  endif()
  if((NOT OpenGL_FOUND) AND (NOT OPENGL_FOUND))
    message(FATAL_ERROR "Could not find OpenGL")
  endif()
else()
  find_package(OpenGL   REQUIRED)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(Boost 1.41.0 COMPONENTS program_options filesystem system)
  if((NOT Boost_FOUND) AND (NOT BOOST_FOUND))
    pkg_check_modules(Boost Boost>=1.41.0)
  endif()
  if((NOT Boost_FOUND) AND (NOT BOOST_FOUND))
    message(FATAL_ERROR "Could not find Boost COMPONENTS program_options filesystem system")
  endif()
else()
  find_package(Boost 1.41.0  REQUIRED program_options filesystem system)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(X11 )
  if((NOT X11_FOUND) AND (NOT X11_FOUND))
    pkg_check_modules(X11 X11)
  endif()
else()
  find_package(X11  )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(hwsd 1.1)
  if((NOT hwsd_FOUND) AND (NOT HWSD_FOUND))
    pkg_check_modules(hwsd hwsd>=1.1)
  endif()
else()
  find_package(hwsd 1.1 )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(GLStats 0.3)
  if((NOT GLStats_FOUND) AND (NOT GLSTATS_FOUND))
    pkg_check_modules(GLStats GLStats>=0.3)
  endif()
else()
  find_package(GLStats 0.3 )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(hwloc 1.3)
  if((NOT hwloc_FOUND) AND (NOT HWLOC_FOUND))
    pkg_check_modules(hwloc hwloc>=1.3)
  endif()
else()
  find_package(hwloc 1.3 )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(OpenSceneGraph 3.0 COMPONENTS osgDB osgUtil)
  if((NOT OpenSceneGraph_FOUND) AND (NOT OPENSCENEGRAPH_FOUND))
    pkg_check_modules(OpenSceneGraph OpenSceneGraph>=3.0)
  endif()
else()
  find_package(OpenSceneGraph 3.0  COMPONENTS osgDB osgUtil)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(OpenCV 2.0)
  if((NOT OpenCV_FOUND) AND (NOT OPENCV_FOUND))
    pkg_check_modules(OpenCV OpenCV>=2.0)
  endif()
else()
  find_package(OpenCV 2.0 )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(VRPN 07.30)
  if((NOT VRPN_FOUND) AND (NOT VRPN_FOUND))
    pkg_check_modules(VRPN VRPN>=07.30)
  endif()
else()
  find_package(VRPN 07.30 )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(DisplayCluster 0.3)
  if((NOT DisplayCluster_FOUND) AND (NOT DISPLAYCLUSTER_FOUND))
    pkg_check_modules(DisplayCluster DisplayCluster>=0.3)
  endif()
else()
  find_package(DisplayCluster 0.3 )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(MAGELLAN )
  if((NOT MAGELLAN_FOUND) AND (NOT MAGELLAN_FOUND))
    pkg_check_modules(MAGELLAN MAGELLAN)
  endif()
else()
  find_package(MAGELLAN  )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(GLEW_MX )
  if((NOT GLEW_MX_FOUND) AND (NOT GLEW_MX_FOUND))
    pkg_check_modules(GLEW_MX GLEW_MX)
  endif()
else()
  find_package(GLEW_MX  )
endif()


if(EXISTS ${CMAKE_SOURCE_DIR}/CMake/FindPackagesPost.cmake)
  include(${CMAKE_SOURCE_DIR}/CMake/FindPackagesPost.cmake)
endif()

if(VMMLIB_FOUND)
  set(vmmlib_name VMMLIB)
  set(vmmlib_FOUND TRUE)
elseif(vmmlib_FOUND)
  set(vmmlib_name vmmlib)
  set(VMMLIB_FOUND TRUE)
endif()
if(vmmlib_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_VMMLIB)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} vmmlib")
  link_directories(${${vmmlib_name}_LIBRARY_DIRS})
  if(NOT "${${vmmlib_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${vmmlib_name}_INCLUDE_DIRS})
  endif()
endif()

if(LUNCHBOX_FOUND)
  set(Lunchbox_name LUNCHBOX)
  set(Lunchbox_FOUND TRUE)
elseif(Lunchbox_FOUND)
  set(Lunchbox_name Lunchbox)
  set(LUNCHBOX_FOUND TRUE)
endif()
if(Lunchbox_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_LUNCHBOX)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Lunchbox")
  link_directories(${${Lunchbox_name}_LIBRARY_DIRS})
  if(NOT "${${Lunchbox_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${Lunchbox_name}_INCLUDE_DIRS})
  endif()
endif()

if(COLLAGE_FOUND)
  set(Collage_name COLLAGE)
  set(Collage_FOUND TRUE)
elseif(Collage_FOUND)
  set(Collage_name Collage)
  set(COLLAGE_FOUND TRUE)
endif()
if(Collage_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_COLLAGE)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Collage")
  link_directories(${${Collage_name}_LIBRARY_DIRS})
  if(NOT "${${Collage_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${Collage_name}_INCLUDE_DIRS})
  endif()
endif()

if(OPENGL_FOUND)
  set(OpenGL_name OPENGL)
  set(OpenGL_FOUND TRUE)
elseif(OpenGL_FOUND)
  set(OpenGL_name OpenGL)
  set(OPENGL_FOUND TRUE)
endif()
if(OpenGL_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_OPENGL)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OpenGL")
  link_directories(${${OpenGL_name}_LIBRARY_DIRS})
  if(NOT "${${OpenGL_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${OpenGL_name}_INCLUDE_DIRS})
  endif()
endif()

if(BOOST_FOUND)
  set(Boost_name BOOST)
  set(Boost_FOUND TRUE)
elseif(Boost_FOUND)
  set(Boost_name Boost)
  set(BOOST_FOUND TRUE)
endif()
if(Boost_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_BOOST)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Boost")
  link_directories(${${Boost_name}_LIBRARY_DIRS})
  if(NOT "${${Boost_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(SYSTEM ${${Boost_name}_INCLUDE_DIRS})
  endif()
endif()

if(X11_FOUND)
  set(X11_name X11)
  set(X11_FOUND TRUE)
elseif(X11_FOUND)
  set(X11_name X11)
  set(X11_FOUND TRUE)
endif()
if(X11_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_X11)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} X11")
  link_directories(${${X11_name}_LIBRARY_DIRS})
  if(NOT "${${X11_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${X11_name}_INCLUDE_DIRS})
  endif()
endif()

if(HWSD_FOUND)
  set(hwsd_name HWSD)
  set(hwsd_FOUND TRUE)
elseif(hwsd_FOUND)
  set(hwsd_name hwsd)
  set(HWSD_FOUND TRUE)
endif()
if(hwsd_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_HWSD)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} hwsd")
  link_directories(${${hwsd_name}_LIBRARY_DIRS})
  if(NOT "${${hwsd_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${hwsd_name}_INCLUDE_DIRS})
  endif()
endif()

if(GLSTATS_FOUND)
  set(GLStats_name GLSTATS)
  set(GLStats_FOUND TRUE)
elseif(GLStats_FOUND)
  set(GLStats_name GLStats)
  set(GLSTATS_FOUND TRUE)
endif()
if(GLStats_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_GLSTATS)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} GLStats")
  link_directories(${${GLStats_name}_LIBRARY_DIRS})
  if(NOT "${${GLStats_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${GLStats_name}_INCLUDE_DIRS})
  endif()
endif()

if(HWLOC_FOUND)
  set(hwloc_name HWLOC)
  set(hwloc_FOUND TRUE)
elseif(hwloc_FOUND)
  set(hwloc_name hwloc)
  set(HWLOC_FOUND TRUE)
endif()
if(hwloc_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_HWLOC)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} hwloc")
  link_directories(${${hwloc_name}_LIBRARY_DIRS})
  if(NOT "${${hwloc_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${hwloc_name}_INCLUDE_DIRS})
  endif()
endif()

if(OPENSCENEGRAPH_FOUND)
  set(OpenSceneGraph_name OPENSCENEGRAPH)
  set(OpenSceneGraph_FOUND TRUE)
elseif(OpenSceneGraph_FOUND)
  set(OpenSceneGraph_name OpenSceneGraph)
  set(OPENSCENEGRAPH_FOUND TRUE)
endif()
if(OpenSceneGraph_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_OPENSCENEGRAPH)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OpenSceneGraph")
  link_directories(${${OpenSceneGraph_name}_LIBRARY_DIRS})
  if(NOT "${${OpenSceneGraph_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(BEFORE SYSTEM ${${OpenSceneGraph_name}_INCLUDE_DIRS})
  endif()
endif()

if(OPENCV_FOUND)
  set(OpenCV_name OPENCV)
  set(OpenCV_FOUND TRUE)
elseif(OpenCV_FOUND)
  set(OpenCV_name OpenCV)
  set(OPENCV_FOUND TRUE)
endif()
if(OpenCV_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_OPENCV)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OpenCV")
  link_directories(${${OpenCV_name}_LIBRARY_DIRS})
  if(NOT "${${OpenCV_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(SYSTEM ${${OpenCV_name}_INCLUDE_DIRS})
  endif()
endif()

if(VRPN_FOUND)
  set(VRPN_name VRPN)
  set(VRPN_FOUND TRUE)
elseif(VRPN_FOUND)
  set(VRPN_name VRPN)
  set(VRPN_FOUND TRUE)
endif()
if(VRPN_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_VRPN)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} VRPN")
  link_directories(${${VRPN_name}_LIBRARY_DIRS})
  if(NOT "${${VRPN_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(SYSTEM ${${VRPN_name}_INCLUDE_DIRS})
  endif()
endif()

if(DISPLAYCLUSTER_FOUND)
  set(DisplayCluster_name DISPLAYCLUSTER)
  set(DisplayCluster_FOUND TRUE)
elseif(DisplayCluster_FOUND)
  set(DisplayCluster_name DisplayCluster)
  set(DISPLAYCLUSTER_FOUND TRUE)
endif()
if(DisplayCluster_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_DISPLAYCLUSTER)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} DisplayCluster")
  link_directories(${${DisplayCluster_name}_LIBRARY_DIRS})
  if(NOT "${${DisplayCluster_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${DisplayCluster_name}_INCLUDE_DIRS})
  endif()
endif()

if(MAGELLAN_FOUND)
  set(MAGELLAN_name MAGELLAN)
  set(MAGELLAN_FOUND TRUE)
elseif(MAGELLAN_FOUND)
  set(MAGELLAN_name MAGELLAN)
  set(MAGELLAN_FOUND TRUE)
endif()
if(MAGELLAN_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_MAGELLAN)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} MAGELLAN")
  link_directories(${${MAGELLAN_name}_LIBRARY_DIRS})
  if(NOT "${${MAGELLAN_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${MAGELLAN_name}_INCLUDE_DIRS})
  endif()
endif()

if(GLEW_MX_FOUND)
  set(GLEW_MX_name GLEW_MX)
  set(GLEW_MX_FOUND TRUE)
elseif(GLEW_MX_FOUND)
  set(GLEW_MX_name GLEW_MX)
  set(GLEW_MX_FOUND TRUE)
endif()
if(GLEW_MX_name)
  list(APPEND FIND_PACKAGES_DEFINES EQUALIZER_USE_GLEW_MX)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} GLEW_MX")
  link_directories(${${GLEW_MX_name}_LIBRARY_DIRS})
  if(NOT "${${GLEW_MX_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${GLEW_MX_name}_INCLUDE_DIRS})
  endif()
endif()

set(EQUALIZER_BUILD_DEBS autoconf;automake;bison;cmake;doxygen;flex;freeglut3-dev;git;git-review;libavahi-compat-libdnssd-dev;libavcodec-dev;libavformat-dev;libavutil-dev;libbluetooth-dev;libboost-date-time-dev;libboost-filesystem-dev;libboost-program-options-dev;libboost-regex-dev;libboost-serialization-dev;libboost-system-dev;libboost-test-dev;libboost-thread-dev;libfcgi-dev;libgl1-mesa-dev;libglewmx1.6-dev;libhwloc-dev;libibverbs-dev;libjpeg-turbo8-dev;libopencv-dev;libopenmpi-dev;libopenscenegraph-dev;libpoppler-dev;libqt4-dev;librdmacm-dev;libspnav-dev;libswscale-dev;libturbojpeg;libudt-dev;libx11-dev;libxmu-dev;openmpi-bin;pkg-config;subversion)

set(EQUALIZER_DEPENDS vmmlib;Lunchbox;Collage;OpenGL;Boost;X11;hwsd;GLStats;hwloc;OpenSceneGraph;OpenCV;VRPN;DisplayCluster;MAGELLAN;GLEW_MX)

# Write defines.h and options.cmake
if(NOT PROJECT_INCLUDE_NAME)
  message(WARNING "PROJECT_INCLUDE_NAME not set, old or missing Common.cmake?")
  set(PROJECT_INCLUDE_NAME ${CMAKE_PROJECT_NAME})
endif()
if(NOT OPTIONS_CMAKE)
  set(OPTIONS_CMAKE ${CMAKE_BINARY_DIR}/options.cmake)
endif()
set(DEFINES_FILE "${CMAKE_BINARY_DIR}/include/${PROJECT_INCLUDE_NAME}/defines${SYSTEM}.h")
list(APPEND COMMON_INCLUDES ${DEFINES_FILE})
set(DEFINES_FILE_IN ${DEFINES_FILE}.in)
file(WRITE ${DEFINES_FILE_IN}
  "// generated by CMake/FindPackages.cmake, do not edit.\n\n"
  "#ifndef ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n"
  "#define ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n\n")
file(WRITE ${OPTIONS_CMAKE} "# Optional modules enabled during build\n")
foreach(DEF ${FIND_PACKAGES_DEFINES})
  add_definitions(-D${DEF}=1)
  file(APPEND ${DEFINES_FILE_IN}
  "#ifndef ${DEF}\n"
  "#  define ${DEF} 1\n"
  "#endif\n")
if(NOT DEF STREQUAL SYSTEM)
  file(APPEND ${OPTIONS_CMAKE} "set(${DEF} ON)\n")
endif()
endforeach()
if(CMAKE_MODULE_INSTALL_PATH)
  install(FILES ${OPTIONS_CMAKE} DESTINATION ${CMAKE_MODULE_INSTALL_PATH}
    COMPONENT dev)
else()
  message(WARNING "CMAKE_MODULE_INSTALL_PATH not set, old or missing Common.cmake?")
endif()
file(APPEND ${DEFINES_FILE_IN}
  "\n#endif\n")

include(UpdateFile)
configure_file(${DEFINES_FILE_IN} ${DEFINES_FILE} COPYONLY)
if(CUDA_FOUND)
  string(REPLACE "-std=c++11" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
  string(REPLACE "-std=c++0x" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
endif()
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
if(QT4_FOUND)
  if(WIN32)
    set(QT_USE_QTMAIN TRUE)
  endif()
  # Configure a copy of the 'UseQt4.cmake' system file.
  if(NOT EXISTS ${QT_USE_FILE})
    message(WARNING "Can't find QT_USE_FILE!")
  else()
    set(_customUseQt4File "${CMAKE_BINARY_DIR}/UseQt4.cmake")
    file(READ ${QT_USE_FILE} content)
    # Change all include_directories() to use the SYSTEM option
    string(REPLACE "include_directories(" "include_directories(SYSTEM " content ${content})
    string(REPLACE "INCLUDE_DIRECTORIES(" "INCLUDE_DIRECTORIES(SYSTEM " content ${content})
    file(WRITE ${_customUseQt4File} ${content})
    set(QT_USE_FILE ${_customUseQt4File})
    include(${QT_USE_FILE})
  endif()
endif()
if(FIND_PACKAGES_FOUND)
  if(MSVC)
    message(STATUS "Configured with ${FIND_PACKAGES_FOUND}")
  else()
    message(STATUS "Configured with ${CMAKE_BUILD_TYPE}${FIND_PACKAGES_FOUND}")
  endif()
endif()
