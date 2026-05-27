cmake_minimum_required(VERSION 4.0)
project(LibrarySystem)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")
file(GLOB_RECURSE MODULES "src/*.ixx")
file(GLOB_RECURSE LIBS "libs/sqlite3/*.c" "libs/sqlite3/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${LIBS})

target_include_directories(${PROJECT_NAME} PRIVATE "libs/sqlite3")
target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS src
        FILES ${MODULES}
)

file(GLOB_RECURSE ASSETS_FILES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
)
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        DEPENDS ${ASSETS_FILES}
        COMMENT "Copying assets to executable directory"
)
add_dependencies(${PROJECT_NAME} copy_assets)