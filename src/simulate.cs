#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"
#include "../_deps/bgfx-src/bgfx/src/bgfx_compute.sh"

uniform vec4 time[1];

BUFFER_RW(positionBuffer, vec4, 0);
BUFFER_RW(velocityBuffer, vec4, 1);

NUM_THREADS(8, 8, 8)
void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint z = gl_GlobalInvocationID.z;
	uint position = (256*256*z+256*y+x);
	positionBuffer[position] = vec4(x, y, z, 0);
}