cmake_minimum_required(VERSION 3.21)
project(game)

if(NOT EMSCRIPTEN)
    include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    set(EmscriptenSettings -g -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 --preload-file=../Resources/ -sSDL2_IMAGE_FORMATS=[\"png\"])
    
    target_compile_options(${PROJECT_NAME} PRIVATE ${EmscriptenSettings})
    target_link_options(${PROJECT_NAME} PRIVATE    ${EmscriptenSettings} -sFULL_ES3 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 )    
else()
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(glad CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)

    target_link_libraries(${PROJECT_NAME} PUBLIC $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
    target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
    target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
    target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

    file(GLOB_RECURSE RESOURCES "${CMAKE_SOURCE_DIR}/Resources/*")
    target_sources(${PROJECT_NAME} PRIVATE ${RESOURCES})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E create_symlink
                    "${PROJECT_SOURCE_DIR}/Resources"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
    )

endif()
