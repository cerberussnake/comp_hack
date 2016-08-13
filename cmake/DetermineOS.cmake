### code used from https://github.com/rtv/Stage/blob/master/cmake/internal/FindOS.cmake

# CMake does not distinguish Linux from other Unices.
STRING (REGEX MATCH "Linux" PROJECT_OS_LINUX ${CMAKE_SYSTEM_NAME})
# Nor *BSD
STRING (REGEX MATCH "BSD" PROJECT_OS_BSD ${CMAKE_SYSTEM_NAME})

IF (PROJECT_OS_LINUX)
    SET(LINUX 1)
ELSEIF (PROJECT_OS_BSD)
    SET(BSD 1)
ENDIF (PROJECT_OS_LINUX)
