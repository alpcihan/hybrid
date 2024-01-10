if(WIN32)
    add_definitions(-DNOMINMAX)
    set(GLSLC glslc.exe) 
else()
    set(GLSLC glslc)
endif(WIN32)

# Compiles all shaders in /glsl folder to SPIR-V
file(GLOB GLSL_SHADERS CONFIGURE_DEPENDS "glsl/*.*")

# All of the shaders are dependent on all header files in /glsl/core 
# (any change to a header file will trigger a recompile of all shaders )
file(GLOB GLSL_HEADERS CONFIGURE_DEPENDS "glsl/core/*.*")

foreach(GLSL ${GLSL_SHADERS})
    get_filename_component(FILE_NAME ${GLSL} NAME_WE)
    get_filename_component(FILE_EXT ${GLSL} LAST_EXT)
    string(REPLACE "." "" FILE_TYPE ${FILE_EXT})
    set(SPIRV "${FILE_NAME}_${FILE_TYPE}.spv")
    add_custom_command( OUTPUT ${SPIRV}
                        COMMAND ${GLSLC} ${GLSL} -O -o ${SPIRV}
                        DEPENDS ${GLSL} ${GLSL_HEADERS})
    list(APPEND SPIRV_SHADERS ${SPIRV})
endforeach(GLSL)

add_custom_target(hybrid_shaders DEPENDS ${SPIRV_SHADERS})