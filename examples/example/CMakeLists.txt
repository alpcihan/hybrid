add_executable(example example.cpp)
target_link_libraries(example PUBLIC hybrid_lib)
# add_dependencies(example example_shaders)

if(WIN32)
    set_property(TARGET example PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${HYBRID_EXAMPLES_WORKING_DIR}")
endif(WIN32)
