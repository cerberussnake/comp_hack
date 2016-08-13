# LIBUV_INCLUDE_DIRS  - where to find uv.h.
# LIBUV_LIBRARIES     - List of libraries when using uv.
# LIBUV_FOUND - True if uv found.

# Look for the header file.
FIND_PATH(LIBUV_INCLUDE_DIR uv.h)

# Look for the library.
FIND_LIBRARY(LIBUV_LIBRARY NAMES uv)

# Handle the QUIETLY and REQUIRED arguments and set LIBUV_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBUV DEFAULT_MSG LIBUV_LIBRARY LIBUV_INCLUDE_DIR)

# Copy the results to the output variables.
IF(LIBUV_FOUND)
        SET(LIBUV_LIBRARIES ${LIBUV_LIBRARY})
        SET(LIBUV_INCLUDE_DIRS ${LIBUV_INCLUDE_DIR})
ELSE(LIBUV_FOUND)
        SET(LIBUV_LIBRARIES)
        SET(LIBUV_INCLUDE_DIRS)
ENDIF(LIBUV_FOUND)

MARK_AS_ADVANCED(LIBUV_INCLUDE_DIRS LIBUV_LIBRARIES)
