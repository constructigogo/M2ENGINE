$input v_wpos, v_view, v_normal, v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);

vec2 blinn(vec3 _lightDir, vec3 _normal, vec3 _viewDir)
{
    float ndotl = dot(_normal, _lightDir);
    //vec3 reflected = _lightDir - 2.0*ndotl*_normal; // reflect(_lightDir, _normal);
    vec3 reflected = 2.0*ndotl*_normal - _lightDir;
    float rdotv = dot(reflected, _viewDir);
    return vec2(ndotl, rdotv);
}

float fresnel(float _ndotl, float _bias, float _pow)
{
    float facing = (1.0 - _ndotl);
    return max(_bias + (1.0 - _bias) * pow(facing, _pow), 0.0);
}

vec4 lit(float _ndotl, float _rdotv, float _m)
{
    float diff = max(0.0, _ndotl);
    float spec = step(0.0, _ndotl) * max(0.0, _rdotv * _m);
    return vec4(1.0, diff, spec, 1.0);
}


void main()
{
    vec4 color = texture2D(s_texColor, v_texcoord0);
    vec3 normal;
    normal.xyz = texture2D(s_texNormal, v_texcoord0).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    float res = dot(normalize(normal),v_view);
    float gamma = 1.0;
    vec3 fragColor = pow(color.rgb, vec3(1.0/gamma));
    gl_FragColor = vec4(fragColor.xyz*res,1.0);
}