#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"
#include "../_deps/bgfx-src/bgfx/src/bgfx_compute.sh"

uniform vec4 uniforms[1];

BUFFER_RW(positionBuffer, vec4, 0);
BUFFER_RW(velocityBuffer, vec4, 1);

NUM_THREADS(8, 8, 8)
void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint z = gl_GlobalInvocationID.z;
	uint workgroups = uint(uniforms[0].x);
	uint position = (workgroups*8*workgroups*8*z+workgroups*8*y+x);
	positionBuffer[position] = vec4(x, y, z, 0);
}