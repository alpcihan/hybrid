add_executable(fractal_travel fractal_travel.cpp)
target_link_libraries(fractal_travel PUBLIC hybrid_lib)

if(WIN32)
    set_property(TARGET fractal_travel PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${HYBRID_EXAMPLES_WORKING_DIR}")
endif(WIN32)
