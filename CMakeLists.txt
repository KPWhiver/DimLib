cmake_minimum_required(VERSION 2.6)

project(libdim)

set(PACKAGE "libdim")
set(PACKAGE_NAME ${PACKAGE})
set(PACKAGE_VERSION_MAJOR "0")
set(PACKAGE_VERSION_MINOR "1")
set(PACKAGE_VERSION_PATCH "0")
set(VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
set(PACKAGE_VERSION ${VERSION})
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")


add_subdirectory(include/dim)

set(TMP)

foreach(CXXHEADER ${CXXHEADERS})
  list(APPEND TMP ../include/dim/${CXXHEADER})
endforeach(CXXHEADER)

set(CXXHEADERS ${TMP})

add_subdirectory(src)

