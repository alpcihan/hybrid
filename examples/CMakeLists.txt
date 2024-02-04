find_package(Threads)
set(HYBRID_EXAMPLES_WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# download example assets
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    execute_process(
        COMMAND git clone https://github.com/alpcihan/hybrid-examples-assets.git ${CMAKE_CURRENT_SOURCE_DIR}/assets
        RESULT_VARIABLE GIT_CLONE_RESULT
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to clone example assets repository. Aborting.")
    endif()
endif()

add_subdirectory(fractal_flythrough)