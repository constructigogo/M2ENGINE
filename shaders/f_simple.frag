$input v_color0, v_view, v_normal

#include "bgfx_shader.sh"

void main(){

	float res = dot(normalize(v_normal),v_view);
	res=1.0;
	gl_FragColor = vec4(v_color0.xyz*res,1.0);
}
