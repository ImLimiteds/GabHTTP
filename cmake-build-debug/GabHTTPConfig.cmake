include(CMakeFindDependencyMacro)

find_dependency(GabHTTP REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/GabHTTPTargets.cmake")