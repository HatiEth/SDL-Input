PROJECT(sdlinputwrapper-sample-usage)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
aux_source_directory(. SDLINPUTWRAPPER_SAMPLES_SRC_LIST)


FIND_PACKAGE(OpenGL)

INCLUDE_DIRECTORIES(
    ${SDL2_INCLUDE_DIR}
    ${SDLIW_INCLUDES}
    ${OpenGL_INCLUDE_DIR}
)

message(STATUS ${OpenGL_gl_LIBRARY})

add_executable(${PROJECT_NAME} ${SDLINPUTWRAPPER_SAMPLES_SRC_LIST})
target_link_libraries(${PROJECT_NAME}
    ${GTEST_LIBRARIES}
    ${SDL2_LIBRARY}
    ${OPENGL_LIBRARIES}
    sdlinputwrapper
)

