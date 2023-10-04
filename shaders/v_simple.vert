$input a_position, a_normal, a_texcoord0, a_color0
$output v_color0, v_view, v_normal

#include "bgfx_shader.sh"

void main(){
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    v_normal = a_normal;
    v_view = mul(vec4(0.0, 0.0, -1.0, 1.0), u_view).xyz;
    //v_color0   = vec4((a_normal+1.0)/2.0, 1.0);
    v_color0   = vec4(a_color0.xyz, 1.0);
}
