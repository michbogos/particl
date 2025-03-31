#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"
#include "../_deps/bgfx-src/bgfx/src/bgfx_compute.sh"

uniform vec4 time[1];

BUFFER_RW(positionBuffer, vec4, 0);
BUFFER_RW(velocityBuffer, vec4, 1);

NUM_THREADS(32, 1, 1)
void main()
{
	positionBuffer[gl_GlobalInvocationID.x] = vec4(2*sin(time[0].y+gl_GlobalInvocationID.x), 2*sin(0.9+time[0].y+gl_GlobalInvocationID.x), 2*sin(1.3+time[0].y+gl_GlobalInvocationID.x), 0.0);
}