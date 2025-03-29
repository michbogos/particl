$input a_position, a_color0
$output v_color0

#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_color0 = a_color0;
}