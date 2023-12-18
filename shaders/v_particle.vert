$input a_position
$output v_color0


#include "bgfx_shader.sh"

void main(){
    gl_Position = mul(u_viewProj, vec4(a_position, 1.0));
    v_color0   = vec4(1.0,1.0,1.0,1.0);
}
