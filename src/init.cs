#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"
#include "../_deps/bgfx-src/bgfx/src/bgfx_compute.sh"

uniform vec4 uniforms[1];

BUFFER_RW(positionBuffer, vec4, 0);
BUFFER_RW(prevPosition, vec4, 1);

NUM_THREADS(8, 8, 8)
void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint z = gl_GlobalInvocationID.z;
	uint workgroups = uint(uniforms[0].x);
	uint position = (workgroups*8*workgroups*8*z+workgroups*8*y+x);
	// if((x%4+y%4+z%4)==0){
		positionBuffer[position] = vec4(float(x)/uniforms[0].x/8.0f, float(y)/uniforms[0].x/8.0f, float(z)/uniforms[0].x/8.0f, 1);
		prevPosition[position] = vec4(float(x)/uniforms[0].x/8.0f, float(y)/uniforms[0].x/8.0f, float(z)/uniforms[0].x/8.0f, 1);
	// }
	// else{
	// 	positionBuffer[position] = vec4(-10., -10., -10., 0.0f);
	// 	prevPosition[position] = vec4(-10., -10., -10., 0.0f);
	// }
}