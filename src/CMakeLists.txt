cmake_minimum_required(VERSION 3.9)
# Compiler flags
if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc" CACHE INTERNAL "" FORCE)
endif(MSVC)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Astral_Insanity)

#### Setup Directories ####
#Main output directory
SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
# Ouput all DLLs from all libs into main build folder
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})


#### Add External Dependencies ####
# SFML
add_subdirectory("lib/sfml")
include_directories("lib/sfml/include")
link_directories("${CMAKE_BINARY_DIR}/lib/sfml/lib")


#### Define sources and executable ####
file(GLOB_RECURSE RESOURCE_FILES  "res/*.*")
set(EXECUTABLE_NAME "astral_insanity")
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${RESOURCE_FILES})
set(EXECUTABLES ${EXECUTABLE_NAME})

#### Target for copying resources to build dir####
if(APPLE)
  add_custom_target(copy_resources ALL
          COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${PROJECT_SOURCE_DIR}/res" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
          COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/lib/sfml/extlibs/libs-osx/Frameworks" ${CMAKE_BINARY_DIR}/lib/sfml/Frameworks
          )
else()
  add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${PROJECT_SOURCE_DIR}/res" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/res
          )
endif()

#### Link Dependencies ####

  #Set working directory to build dir
  set_target_properties(${EXECUTABLE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
  )

  target_link_libraries(${EXECUTABLE_NAME}
    optimized sfml-graphics debug sfml-graphics-d 
    optimized sfml-audio debug sfml-audio-d 
    optimized sfml-window debug sfml-window-d 
    optimized sfml-system debug sfml-system-d 
    optimized sfml-graphics debug sfml-graphics-d
  )

  #target_link_libraries(${exe} "${CMAKE_SOURCE_DIR}/lib/sfml/extlibs/libs-osx/Frameworks/f‌​reetype.framework/Ve‌​rsions/A/freetype")

  add_dependencies(${EXECUTABLE_NAME} sfml-graphics sfml-audio copy_resources)



