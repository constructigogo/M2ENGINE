$input a_position, a_normal, a_tangent, a_texcoord0, a_color0,i_data0, i_data1, i_data2, i_data3
$output v_wpos, v_view, v_normal, v_texcoord0
#include "bgfx_shader.sh"

void main(){
    mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
    vec4 worldPos = mul(model, vec4(a_position, 1.0) );
    gl_Position = mul(u_viewProj, worldPos);

    v_view = mul(vec4(0.0,0.0,-1.0,1.0), u_view).xyz;
    vec3 a_bitangent = cross(a_tangent,a_normal);
    vec3 T = normalize(vec3(model * vec4(a_tangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(a_bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(a_normal,    0.0)));
    T = normalize(T - dot(T, N) * N);
    //mat3 TBN = mat3(T, B, N);
    mat3 TBN = transpose(mat3(T, B, N));

    v_view = TBN * v_view;

    //v_normal=(a_normal+1.0)/2.0;
    v_texcoord0 = a_texcoord0;
    v_normal=(a_normal);
}
