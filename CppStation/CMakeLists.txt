add_executable(CppStation)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "*.cpp"
)

target_sources(CppStation PRIVATE ${SRC_FILES})

if(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    target_compile_options(CppStation PUBLIC "/std:c++latest")
else()
    target_compile_options(CppStation PUBLIC "-std=c++14")
endif()

target_include_directories(CppStation PUBLIC include)

target_link_libraries(CppStation
  PRIVATE
    fmt::fmt
)
