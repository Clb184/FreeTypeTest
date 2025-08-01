# CMake version 3.5, idk why but it may be useful
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(MSVC)
	message("sal.h already comes with VS...")
else()
	message("Getting sal.h for DirectXMath")
	file(DOWNLOAD https://raw.githubusercontent.com/dotnet/runtime/main/src/coreclr/pal/inc/rt/sal.h  ${CMAKE_HOME_DIRECTORY}/include/sal.h )
endif()

project(FreeTypeTest LANGUAGES C CXX)

# Source files (cpp) and header files (hpp & h)
file(GLOB cpp_files src/*.cpp src/*.c)
file(GLOB hpp_files include/*.hpp include/*.h)

# exe
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_definitions(GLEW_STATIC)


set(EXE_NAME ${PROJECT_NAME}d)
message(${EXE_NAME})
add_executable(${EXE_NAME} ${cpp_files})
add_subdirectory(deps/glew/build/cmake)
add_subdirectory(deps/glfw)
add_subdirectory(deps/FreeType)
add_subdirectory(deps/DirectXMath)
target_link_libraries(${EXE_NAME} glfw glew_s freetype DirectXMath)
set_target_properties(${EXE_NAME} PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/)
message(${CMAKE_HOME_DIRECTORY})
# Additional include directories
target_include_directories(${EXE_NAME} 
	PUBLIC
		include/
		deps/glew/include/
)

