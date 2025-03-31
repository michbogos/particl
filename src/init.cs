#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"
#include "../_deps/bgfx-src/bgfx/src/bgfx_compute.sh"

BUFFER_RW(positionBuffer, vec4, 0);
BUFFER_RW(velocityBuffer, vec4, 1);

NUM_THREADS(32, 1, 1)
void main()
{
	positionBuffer[gl_GlobalInvocationID.x] = vec4(0, 0, 0, 0.0);
	// currPositionBuffer[gl_GlobalInvocationID.x] = vec4(position, 0.0);
}