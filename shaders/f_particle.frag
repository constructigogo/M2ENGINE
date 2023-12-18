$input v_color0

#include "bgfx_shader.sh"

void main(){
    vec2 point = gl_PointCoord.xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(point, point);
    if(mag > 1.0) discard; /* outside the circle */
    gl_FragColor = vec4(v_color0.xyz,1.0);
}
