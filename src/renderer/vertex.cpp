#pragma once

#include "hybrid/renderer/vertex.hpp"


namespace hybrid {

tga::VertexLayout Vertex::layout() {
    return {sizeof(Vertex),
            {
                {offsetof(Vertex, position), tga::Format::r32g32b32_sfloat},
                {offsetof(Vertex, normal), tga::Format::r32g32b32_sfloat},
                {offsetof(Vertex, uv), tga::Format::r32g32_sfloat},
            }};
}

}  // namespace hybrid