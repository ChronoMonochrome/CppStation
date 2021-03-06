cmake_minimum_required(VERSION 3.16)

project(CppStation)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
else()
  set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif()

include(FetchContent)

add_definitions(-DGLFW_INCLUDE_NONE)
find_package(glfw3      CONFIG REQUIRED)
find_package(glm      CONFIG REQUIRED)

if (UNIX)
  find_package(OpenGL            REQUIRED)

  FetchContent_Declare(
    extern_glad

    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.27)

  FetchContent_GetProperties(extern_glad)
  if(NOT extern_glad_POPULATED)
    FetchContent_Populate(extern_glad)

    set(GLAD_INSTALL ON)
    add_subdirectory(
      ${extern_glad_SOURCE_DIR}
      ${extern_glad_BINARY_DIR}
      EXCLUDE_FROM_ALL)
  endif()
else()
  find_package(glad       CONFIG REQUIRED)
endif (UNIX)

find_package(OpenGL            REQUIRED)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

add_subdirectory(CppStation)
