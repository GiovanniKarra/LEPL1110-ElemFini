<<<<<<< HEAD
<<<<<<< HEAD
# Install script for directory: C:/Users/pierr/Q6/ElemFini/GeoMesh

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/myFemProject")
=======
=======
>>>>>>> 7107aea (début devoir 2)
# Install script for directory: /home/giogio/Documents/Unif/Code/LEPL1110-ElemFini/GeoMesh

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
<<<<<<< HEAD
>>>>>>> 7107aea (début devoir 2)
=======
>>>>>>> 7107aea (début devoir 2)
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
<<<<<<< HEAD
<<<<<<< HEAD
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
=======
    set(CMAKE_INSTALL_CONFIG_NAME "")
>>>>>>> 7107aea (début devoir 2)
=======
    set(CMAKE_INSTALL_CONFIG_NAME "")
>>>>>>> 7107aea (début devoir 2)
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> 7107aea (début devoir 2)
# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

<<<<<<< HEAD
>>>>>>> 7107aea (début devoir 2)
=======
>>>>>>> 7107aea (début devoir 2)
# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
<<<<<<< HEAD
<<<<<<< HEAD
  set(CMAKE_OBJDUMP "C:/TDM-GCC-64/bin/objdump.exe")
=======
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
>>>>>>> 7107aea (début devoir 2)
=======
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
>>>>>>> 7107aea (début devoir 2)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
<<<<<<< HEAD
<<<<<<< HEAD
  include("C:/Users/pierr/Q6/ElemFini/GeoMesh/build/glfw/cmake_install.cmake")
=======
  include("/home/giogio/Documents/Unif/Code/LEPL1110-ElemFini/GeoMesh/build/glfw/cmake_install.cmake")
>>>>>>> 7107aea (début devoir 2)
=======
  include("/home/giogio/Documents/Unif/Code/LEPL1110-ElemFini/GeoMesh/build/glfw/cmake_install.cmake")
>>>>>>> 7107aea (début devoir 2)

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
<<<<<<< HEAD
<<<<<<< HEAD
file(WRITE "C:/Users/pierr/Q6/ElemFini/GeoMesh/build/${CMAKE_INSTALL_MANIFEST}"
=======
file(WRITE "/home/giogio/Documents/Unif/Code/LEPL1110-ElemFini/GeoMesh/build/${CMAKE_INSTALL_MANIFEST}"
>>>>>>> 7107aea (début devoir 2)
=======
file(WRITE "/home/giogio/Documents/Unif/Code/LEPL1110-ElemFini/GeoMesh/build/${CMAKE_INSTALL_MANIFEST}"
>>>>>>> 7107aea (début devoir 2)
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
