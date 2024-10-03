cmake_minimum_required(VERSION 3.29)
project(GabHTTP VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(GabHTTP STATIC src/GabHttp.cpp)

target_include_directories(GabHTTP INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if (WIN32)
    target_link_libraries(GabHTTP PRIVATE ws2_32)
endif()

install(TARGETS GabHTTP
        EXPORT GabHTTPTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT GabHTTPTargets
        FILE GabHTTPTargets.cmake
        NAMESPACE GabHTTP::
        DESTINATION lib/cmake/GabHTTP
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/GabHTTPConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_file(GabHTTPConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/GabHTTPConfig.cmake"
        COPYONLY
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/GabHTTPConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/GabHTTPConfigVersion.cmake"
        DESTINATION lib/cmake/GabHTTP
)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_GENERATOR "ZIP")

include(CPack)