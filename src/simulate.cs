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
	float dt = uniforms[0].w/10.0f;
	if(positionBuffer[position].w == 1.0f){
		positionBuffer[position] = positionBuffer[position] + (positionBuffer[position]-prevPosition[position])*dt+vec4(0, -0.0001, 0, 0)*dt*dt;
		if(distance(positionBuffer[position], vec4(2, 2, -2, 1)) > 3.5){
			vec3 norm = (distance(positionBuffer[position], vec4(2, 2, -2, 1))-3.5)*normalize(vec3((positionBuffer[position]-vec4(2, 2, -2, 1))));
			positionBuffer[position].x += norm.x;
			positionBuffer[position].y += norm.y;
			positionBuffer[position].z += norm.z;

		}
		positionBuffer[position].w = 1.0f;
		// prevPosition[position] = vec4(float(x)/uniforms[0].x, float(y)/uniforms[0].x, float(z)/uniforms[0].x, 0);
	}
}