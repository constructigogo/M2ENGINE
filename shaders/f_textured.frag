$input v_wpos, v_view, v_normal, v_tangent, v_bitangent, v_texcoord0, v_shadowcoord

#include "bgfx_shader.sh"
#include "shaderlib.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);
SAMPLER2D(s_texSpecular, 2);
SAMPLER2D(s_shadowMap, 3);

#define Sampler sampler2D

uniform vec4 u_specular;
uniform vec4 u_lightPos;
uniform vec4 u_shadowTexelSize;
const float PI = 3.14159265359f;


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


float ShadowCalc(Sampler _sampler, vec4 coord, vec2 texelSize, float bias){

    vec2 texCoord = coord.xy/coord.w;

    bool outside = any(greaterThan(texCoord, vec2_splat(1.0)))
    || any(lessThan   (texCoord, vec2_splat(0.0)));

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
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
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
    vec2 texCoord = v_texcoord0;
    texCoord.y = 1.0 - texCoord.y;

    vec4 color = texture2D(s_texColor, texCoord);
    if (color.a ==0.0){
        discard;
    }
    color = pow(color, vec4_splat(2.2));

    vec3 normal;
    normal.xyz = texture2D(s_texNormal, texCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    vec3 specmap;
    specmap = texture2D(s_texSpecular, texCoord).xyz;

    float roughness = max(specmap.g, 0.2f);
    float metallic = specmap.b;
    vec3 diffuse = (1.0-metallic)*(color.xyz/PI);
    //float specularValue = 2.0/(roughness)-2.0;


    //light
    vec3 lightDir = normalize(-u_lightPos.xyz);
    vec3 viewDir = normalize(v_view);
    vec3 halfwayDir = normalize(lightDir + viewDir);


    vec3 F0 = vec3_splat(0.04);
    F0 = mix(F0, color.xyz, metallic);

    // cook-torrance brdf
    float NDF = DistributionGGX(normal, halfwayDir, roughness);
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness*roughness);
    vec3 F    = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3_splat(1.0) - kS;
    kD *= 1.0 - metallic;


    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = (numerator / denominator);

    float NdotL = max(dot(normal, lightDir), 0.0);
    //vec3 Lo = ((kD * (color.xyz / PI)) + specular) * 10.0 * NdotL;
    vec3 Lo = (diffuse + specular) * 10.0 * NdotL;

    //Shadow calc
    float bias =  0.0001;
    float inShadow=ShadowCalc(s_shadowMap, v_shadowcoord, u_shadowTexelSize.xy, bias);

    //Final Color
    vec3 ambient = vec3(0.03) * color.xyz;
    vec3 final=inShadow*ambient+(Lo * (1.0-inShadow));
    //final=(vec3(1.0)-inShadow);

    //Correction
    final = final / (final + vec3(1.0));
    final = pow(final, vec3(1.0/2.2));

    //Output
    gl_FragColor = vec4(final, 1.0);


    //gl_FragColor = vec4(specular, 1.0);
    //gl_FragColor = vec4(texture2D(s_shadowMap, texCoord).xyz*1.0,1.0);
    //gl_FragColor = vec4(vec3_splat(F0).xyz,1.0);
}