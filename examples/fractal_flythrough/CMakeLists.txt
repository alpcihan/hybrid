add_executable(fractal_flythrough fractal_flythrough.cpp)
target_link_libraries(fractal_flythrough PUBLIC hybrid_lib)

if(WIN32)
    set_property(TARGET fractal_flythrough PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${HYBRID_EXAMPLES_WORKING_DIR}")
endif(WIN32)
