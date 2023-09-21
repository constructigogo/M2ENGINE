$input a_position, a_normal, a_tangent, a_texcoord0, a_color0
$output v_wpos, v_view, v_normal, v_tangent, v_bitangent, v_texcoord0, v_shadowcoord
#include "bgfx_shader.sh"

uniform mat4 u_lightMtx;

void main(){
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    vec3 wpos = mul(u_model[0], vec4(a_position, 1.0) ).xyz;
    v_wpos = wpos;

    v_view = mul(vec4(0.0,0.0,-1.0,1.0), u_view).xyz;
    vec3 T = normalize(vec3(u_model[0] * vec4(a_tangent,   0.0)));
    vec3 N = normalize(vec3(u_model[0] * vec4(a_normal,    0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    //mat3 TBN = mat3(T, B, N);
    mat3 TBN = transpose(mat3(T, B, N));
    // eye position in world space
    vec3 weyepos = mul(vec4(0.0, 0.0, 0.0, 1.0), u_view).xyz;
    // tangent space view dir
    //v_view = TBN * v_view;
    v_view = mul(weyepos - wpos, TBN);


    vec3 normal = a_normal * 2.0 - 1.0;
    v_normal = N;
    v_tangent=T;
    v_bitangent=B;

    v_texcoord0 = a_texcoord0;

    const float shadowMapOffset = 0.001;
    vec3 posOffset = a_position + a_normal.xyz * shadowMapOffset;
    v_shadowcoord = mul(u_lightMtx, vec4(wpos, 1.0) );
}
