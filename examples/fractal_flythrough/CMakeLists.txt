add_executable(fractal_flythrough 
    model_controller.hpp
    model_controller.cpp
    fractal_flythrough.cpp)

target_link_libraries(fractal_flythrough PUBLIC hybrid_lib)

if(WIN32)
    set_property(TARGET fractal_flythrough PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${HYBRID_EXAMPLES_WORKING_DIR}")
endif(WIN32)
