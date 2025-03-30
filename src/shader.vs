$input a_position, a_color0, i_data0, i_data1, i_data2, i_data3, i_data4
$output v_color0

#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0)+i_data0 );
	v_color0 = a_position.xyzx;
}