#version 460 core

layout (location = 0) out vec4 fragColor;

// ty https://darkeclipz.github.io/fractals/
#define N 64.
#define B 32.
#define SS 4.

layout( push_constant ) uniform constants
{
    vec2 screenRes;
} PushConstants;

float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12454.1,78345.2))) * 43758.5);
}

vec2 random2(in vec2 st) {
    return vec2(random(st), random(st));    
}

vec3 pal( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d ) {
    return a + b*cos(6.28318 * (c*t + d));
}

float iterate(vec2 p) {

    vec2 z = vec2(0), c = p;
    float i;

    for(i=0.; i < N; i++) {
        z = mat2(z, -z.y, z.x) * z + c;
        if(dot(z, z) > B*B) break;
    }

    return i - log(log(dot(z, z)) / log(B)) / log(2.);;     
}



void main() {

    vec2 R = PushConstants.screenRes.xy;

    vec2 fragCoord = gl_FragCoord.xy;

    vec3 col = vec3(0);
    for(float i=0.; i < SS; i++) {

        vec2 uv = (2. * fragCoord + random2(R+i) - R) / R.y ;

        float sn = iterate(uv) / N;   

        col += pal(fract(2.*sn + 0.5), vec3(.5), vec3(0.5), 
                   vec3(1.0,1.0,1.0), vec3(.0, .10, .2));
    }

    fragColor = vec4(col / SS, 1.0);
}