#version 330

// ripple uniforms
uniform int nRipples; // number of ripples
uniform float bandStrength; // base amount of light that each band adds
uniform float rippleAttenDist; // distance at which a band fades out

uniform vec2 screenDim;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec4 sphereLpos;
uniform vec4 sphereCpos;
uniform vec4 sphereRpos;

// need to store:
//  - band widths
//  - band radius
//  - band origin (L,C,R)
uniform samplerBuffer ripples;

in vec4 vs_Color;
in vec4 vs_WorldPos;
out vec4 out_Color;

bool fequals(float a, float b) {
    return abs(a - b) < 0.00001;
}

bool fequals(float a, float b, float epsilon) {
    return abs(a - b) < epsilon;
}

void main(void) {

    float nRipplesAffecting;

    vec4 lightMod = vec4(0.0, 0.0, 0.0, 1.0);

    //vec4 diffL = modelMatrix * sphereLpos - vs_WorldPos;
    //vec4 diffC = modelMatrix * sphereCpos - vs_WorldPos;
    //vec4 diffR = modelMatrix * sphereRpos - vs_WorldPos;
    //vec3 vs_distToSpheres = vec3(   length(diffL.xyz), 
    //                                length(diffC.xyz),
    //                                length(diffR.xyz));
    vec4 diffL = modelMatrix * sphereLpos - vs_WorldPos;
    vec4 diffC = modelMatrix * sphereCpos - vs_WorldPos;
    vec4 diffR = modelMatrix * sphereRpos - vs_WorldPos;
    vec3 vs_distToSpheres = vec3(   length(diffL.xyz), 
                                    length(diffC.xyz),
                                    length(diffR.xyz));
    //vec4 diffL = vec4(100.0, 100.0, 0.0, 1.0) - vs_WorldPos;
    //vec4 diffC = vec4(100.0, 100.0, 0.0, 1.0) - vs_WorldPos;
    //vec4 diffR = vec4(100.0, 100.0, 0.0, 1.0) - vs_WorldPos;
    //vec3 vs_distToSpheres = vec3(   length(diffL.xyz), 
    //                                length(diffC.xyz),
    //                                length(diffR.xyz));

    // find out how many ripples are affecting this spot currently
    for (int i = 0; i < nRipples; ++i) {
        // look up the current ripple
        vec3 rippleLookup = texelFetch(ripples, i).xyz;

        // get the origin index
        //int sphereIdx = int(rippleLookup.z);
        int sphereIdx = 0;

        //float rippleAtten = 1.0 - (rippleLookup.y / rippleAttenDist);
        float rippleAtten = 1.0;

        // if the distance from the appropriate sphere is the same as the
        // current radius, then this ripple is affecting this fragment
        //
        // this is modified by the attenuation
        // for just white light
        nRipplesAffecting += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;

        // for colored lights
        lightMod[sphereIdx] += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;
    }

    // find the color modification for this spot and add it to the color, this
    // is a flat increase in lightness (adding white light)
    // for just white light
    out_Color = vs_Color + vec4(bandStrength * nRipplesAffecting);
    out_Color = vec4(sphereLpos == vec4(412, 307.2, 0.0, 1.0));
    // for tri lights
    //out_Color = vs_Color + lightMod * bandStrength;
    //out_Color = vec4(vs_distToSpheres / 1500, 1.0);
    out_Color.a = 1.0f;
}
