$input a_position, a_normal, a_texcoord0, a_color0, i_data0, i_data1, i_data2, i_data3
$output v_color0

#include "bgfx_shader.sh"

void main(){
    mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);

    vec4 worldPos = mul(model, vec4(a_position, 1.0) );
    gl_Position = mul(u_viewProj, worldPos);
    //gl_Position = mul(u_viewProj, vec4(a_position, 1.0));
    v_color0   = vec4(a_normal,1.0);
}
