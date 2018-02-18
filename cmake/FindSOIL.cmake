# Based on the find module example for LibXml2
# from https://cmake.org/Wiki/CMake:How_To_Find_Libraries
#
# Once done this will define
#  SOIL_FOUND
#  SOIL_INCLUDE_DIRS
#  SOIL_LIBRARIES

find_path(SOIL_INCLUDE_DIR SOIL.h
    PATHS ${SOIL_INCLUDEDIR}
)

find_library(SOIL_LIBRARY
    NAMES SOIL
    PATHS ${SOIL_LIBDIR}
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SOIL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SOIL DEFAULT_MSG
    SOIL_LIBRARY SOIL_INCLUDE_DIR
)

mark_as_advanced(SOIL_INCLUDE_DIR SOIL_LIBRARY)

set(SOIL_LIBRARIES ${SOIL_LIBRARY})
set(SOIL_INCLUDE_DIRS ${SOIL_INCLUDE_DIR})
