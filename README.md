# *Hybrid*
## An Hybrid Renderer for Mesh Rasterization and Fractal Ray Marching

A Hybrid Renderer project, leveraging the capabilities of *deferred rendering*, designed to visualize a scene comprising a static fractal and a 
user-controllable spaceship model navigating in a 3rd person view within an abstract environment. 
The spaceship is rendered using either *rasterization*, while the fractal employs *ray marching* for efficient rendering.
Traditional rasterization struggles with the intricate geometry of the fractal, making it inefficient for representation. 
However, by adopting ray marching, the rendering process becomes significantly more efficient. 
Conversely, certain objects are more suited for rendering with rasterization. 
Also lighting effects are handled differently: shadows and specular reflections of the spaceship are computed via hardware accelerated ray tracing, 
while soft shadows and specular reflections for the fractal rely on ray marching. 
Embracing a hybrid rendering approach enables each object to be rendered using the most suitable technique, and their outputs are merged in the g-buffer 
to generate a final image with cohesive lighting data.
