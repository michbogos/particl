$input v_color0

uniform vec4 time[1];

#include "../_deps/bgfx-src/bgfx/src/bgfx_shader.sh"

void main()
{
	gl_FragColor = v_color0;
}