$input v_wpos, v_view, v_normal, v_tangent, v_bitangent, v_texcoord0, v_shadowcoord

#include "bgfx_shader.sh"
#include "shaderlib.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);
SAMPLER2D(s_shadowMap, 3);

#define Sampler sampler2D

uniform vec4 u_lightPos;
uniform vec4 u_shadowTexelSize;



float ShadowCalc(Sampler _sampler, vec4 coord, vec2 texelSize, float bias){

    vec2 texCoord = coord.xy/coord.w;

    bool outside = any(greaterThan(texCoord, vec2_splat(1.0)))
    || any(lessThan   (texCoord, vec2_splat(0.0)))
    ;

    // perform perspective divide
    vec3 projCoords = coord.xyz / coord.w;
    // transform to [0,1] range
    //projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture2D(s_shadowMap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    float shadow = 0.0;


    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture2D(s_shadowMap, projCoords.xy + vec2(x, y) * u_shadowTexelSize.x).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }


    if (projCoords.z > 1.0 || outside){
        shadow = 0.0;
    }
    return shadow/9.0;
}


void main()
{
    mat3 TBN = mat3(v_tangent, v_bitangent, v_normal);
    //vec3 lpos = vec3(-2.0,2.0,2.0);
    float shadowMapBias = 0.005;
    vec2 texCoord = v_texcoord0;
    texCoord.y = 1.0 - texCoord.y;

    vec4 color = texture2D(s_texColor, texCoord);
    if (color.a ==0.0){
        discard;
    }


    vec3 normal;
    normal.xyz = texture2D(s_texNormal, texCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    vec3 lightDir = normalize(-u_lightPos.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3_splat(1.0);
    float ambient =  0.1;

    float bias = min(0.01 * (1.0 - dot(normal, lightDir)), 0.001);

    float inShadow=ShadowCalc(s_shadowMap, v_shadowcoord, u_shadowTexelSize.xy, bias);


    vec3 final = (ambient + diffuse * (1.0-inShadow)) * color.xyz;
    gl_FragColor = vec4(final, 1.0);
    //gl_FragColor = vec4(texture2D(s_shadowMap, texCoord).xyz*1.0,1.0);
    //gl_FragColor = vec4(vec3_splat(inShadow).xyz,1.0);
}