add_executable(projectName WIN32
    Main.cpp
    ProjectNameApp.h
    ProjectNameApp.cpp)

target_include_directories(projectName PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linking the engine pulls in SDL3, SDL3_ttf, glm, ImGui transitively.
target_link_libraries(projectName PRIVATE gameEngine_lib)

ge_apply_warnings(projectName)

#----------------------------
# Platform-specific post-build steps.
#----------------------------
if(WIN32)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT projectName)
    set_target_properties(projectName PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:projectName>")

    set(SDL_RUNTIME_DLLS
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
    )
    foreach(dllTarget IN LISTS SDL_RUNTIME_DLLS)
        add_custom_command(
            TARGET projectName POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${dllTarget}>"
                "$<TARGET_FILE_DIR:projectName>"
            VERBATIM
        )
    endforeach()

    add_custom_command(TARGET projectName POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:projectName>/resources"
    )
endif()
