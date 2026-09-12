cmake_minimum_required(VERSION 3.31) #this whole file was semi-vibecoded

if(WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
endif()

project(oxyuidemo LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_MANIFEST_MODE ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_library(ass STATIC
    src/uithing.h
    src/oxygenui-lib/oxygenui.cpp
    src/oxygenui-lib/oxygenui.h
) 

set_target_properties(ass PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ass PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
) 

target_link_libraries(ass PUBLIC 
    sfml-graphics
    sfml-window
    sfml-system
)

function(setup_exe TARGET_NAME SOURCE_FILE)
    set(EXE_SOURCES ${SOURCE_FILE})

    if(WIN32)
        list(APPEND EXE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc")
    endif()

    if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            add_executable(${TARGET_NAME} WIN32 ${EXE_SOURCES})
            target_link_options(${TARGET_NAME} PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
        else()
            add_executable(${TARGET_NAME} ${EXE_SOURCES})
        endif()
    else()
        add_executable(${TARGET_NAME} ${EXE_SOURCES})
    endif()
    
    target_link_libraries(${TARGET_NAME} PRIVATE ass)
    target_link_libraries(${TARGET_NAME} PRIVATE sfml-graphics sfml-window sfml-system)

    if(WIN32)
        target_link_libraries(${TARGET_NAME} PRIVATE opengl32 winmm gdi32 user32 advapi32 shell32)
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/res"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/res"
            COMMENT "copying res"
        )
    endif()
endfunction()

setup_exe(oxyuidemo src/main.cpp)