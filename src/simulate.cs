#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"
#include "../_deps/bgfx-src/bgfx/src/bgfx_compute.sh"

uniform vec4 uniforms[1];

BUFFER_RW(positionBuffer, vec4, 0);
BUFFER_RW(prevPosition, vec4, 1);
BUFFER_RW(positionBuffer2, vec4, 2);

NUM_THREADS(8, 8, 8)
void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint z = gl_GlobalInvocationID.z;
	uint workgroups = uint(uniforms[0].x);
	uint position = (workgroups*8*workgroups*8*z+workgroups*8*y+x);
	float dt = uniforms[0].w/8;
	if(positionBuffer[position].w == 1.0f){
		for(int iter = 0; iter < 8; iter++){
			vec4 pPosition = positionBuffer[position];
			positionBuffer[position] = positionBuffer[position] + (positionBuffer[position]-prevPosition[position])*dt+vec4(0, -1.0, 0, 0)*dt*dt;
			if(distance(positionBuffer[position], vec4(0, 0, 0, 1)) > 8){
				vec3 norm = (distance(positionBuffer[position], vec4(0, 0, 0, 1))-8)*normalize(vec3((positionBuffer[position]-vec4(0, 0, 0, 1))));
				positionBuffer[position].x -= norm.x;
				positionBuffer[position].y -= norm.y;
				positionBuffer[position].z -= norm.z;
			}
			for(int i = 0; i < 8; i++){
			if(i%2==0){
				uint newPosition = workgroups*8*workgroups*8*(positionBuffer[position].z+8.0)/uniforms[0].z+workgroups*8*(positionBuffer[position].y+8.0)/uniforms[0].z + (positionBuffer[position].x+8.0)/uniforms[0].z;
				positionBuffer2[newPosition] = positionBuffer[position];
				memoryBarrierShared(); // Ensure change to foobar is visible in other invocations
  				barrier();
				//for(int i = 0; i < 5; i++){
				for(int dx = -1; dx <= 1; dx++){
					for(int dy = -1; dy <= 1; dy++){
						for(int dz = -1; dz <= 1; dz++){
							if(dx != 0 && dy != 0 && dz != 0){
								uint other = workgroups*8*workgroups*8*(z+dz)+workgroups*8*(y+dy)+(x+dx);
								if(distance(positionBuffer2[position], positionBuffer2[other]) < uniforms[0].z*8){
									vec3 norm = (distance(positionBuffer2[position], positionBuffer2[other])-uniforms[0].z)*normalize(vec3((positionBuffer2[position]-positionBuffer2[other])));
									atomicAdd(positionBuffer2[position].x, norm.x/2);
									atomicAdd(positionBuffer2[position].y, norm.y/2);
									atomicAdd(positionBuffer2[position].z, norm.z/2);
									atomicAdd(positionBuffer2[other].x, -norm.x/2);
									atomicAdd(positionBuffer2[other].y, -norm.y/2);
									atomicAdd(positionBuffer2[other].z, -norm.z/2);
									positionBuffer2[other].w = 1.0f;
								}
							}
						}
					}
				}
			// }
			}

			if(i%2==1){
				uint newPosition = workgroups*8*workgroups*8*(positionBuffer2[position].z+8.0)/uniforms[0].z+workgroups*8*(positionBuffer2[position].y+8.0)/uniforms[0].z + (positionBuffer2[position].x+8.0)/uniforms[0].z;
				positionBuffer[newPosition] = positionBuffer2[position];
				memoryBarrierShared(); // Ensure change to foobar is visible in other invocations
  				barrier();
				//for(int i = 0; i < 5; i++){
				for(int dx = -1; dx <= 1; dx++){
					for(int dy = -1; dy <= 1; dy++){
						for(int dz = -1; dz <= 1; dz++){
							if(dx != 0 && dy != 0 && dz != 0){
								uint other = workgroups*8*workgroups*8*(z+dz)+workgroups*8*(y+dy)+(x+dx);
								if(distance(positionBuffer[position], positionBuffer[other]) < uniforms[0].z*8){
									vec3 norm = (distance(positionBuffer[position], positionBuffer[other])-uniforms[0].z)*normalize(vec3((positionBuffer[position]-positionBuffer[other])));
									atomicAdd(positionBuffer[position].x, norm.x/2);
									atomicAdd(positionBuffer[position].y, norm.y/2);
									atomicAdd(positionBuffer[position].z, norm.z/2);
									atomicAdd(positionBuffer[other].x, -norm.x/2);
									atomicAdd(positionBuffer[other].y, -norm.y/2);
									atomicAdd(positionBuffer[other].z, -norm.z/2);
									positionBuffer2[other].w = 1.0f;
								}
							}
						}
					}
				}
				}
			// }
			}
			positionBuffer[position].w = 1.0f;
			prevPosition[position] = pPosition;
		}
	}
}