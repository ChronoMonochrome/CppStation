add_executable(CppStation)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "*.cpp"
)

target_sources(CppStation PRIVATE ${SRC_FILES})

if(UNIX)
	target_compile_options(CppStation PUBLIC "-std=c++17")
else()
    target_compile_options(CppStation PUBLIC "/std:c++17")
endif(UNIX)

target_include_directories(CppStation PUBLIC include)

if (UNIX)
  target_link_libraries(CppStation PUBLIC
      fmt::fmt
      glfw
      glad
  )
else()
  target_link_libraries(CppStation PUBLIC
      fmt::fmt
      glfw
      glad::glad
  )
endif (UNIX)