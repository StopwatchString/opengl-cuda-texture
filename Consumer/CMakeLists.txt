cmake_minimum_required(VERSION 3.29)

project(          Consumer VERSION 0.1
                  DESCRIPTION "Consumer"
                  LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CUDA_STANDARD 17)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "extern/glh/src/*.cpp"
    "extern/glh/src/*.c"
)

file(GLOB_RECURSE INCLUDES 
    "include/*.h*"
    "extern/glh/include/**.h*"
)

source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${SOURCES})
source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${INCLUDES})

add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES} ${GLM_INCLUDES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
        PRIVATE
         "${PROJECT_SOURCE_DIR}/include"
         "${PROJECT_SOURCE_DIR}/include/imgui"
         "${PROJECT_SOURCE_DIR}/extern/glh/include")

target_link_libraries(${PROJECT_NAME} PRIVATE ${CUDA_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY})

add_subdirectory("extern/glm")
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
add_subdirectory("extern/glfw")
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)