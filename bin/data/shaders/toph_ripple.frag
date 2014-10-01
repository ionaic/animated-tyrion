#version 330

// ripple uniforms
uniform int nRipples; // number of ripples
uniform float bandStrength; // base amount of light that each band adds
uniform float rippleAttenDist; // distance at which a band fades out

uniform mat4 modelMatrix;
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

    vec4 diffL = sphereLpos - modelMatrix * vs_WorldPos;
    vec4 diffC = sphereCpos - modelMatrix * vs_WorldPos;
    vec4 diffR = sphereRpos - modelMatrix * vs_WorldPos;
    vec3 vs_distToSpheres = vec3(   length(diffL.xyz), 
                                    length(diffC.xyz),
                                    length(diffR.xyz));

    // find out how many ripples are affecting this spot currently
    for (int i = 0; i < nRipples; ++i) {
        // look up the current ripple
        vec3 rippleLookup = texelFetch(ripples, i).xyz;

        // get the origin index
        int sphereIdx = int(rippleLookup.z);

        float rippleAtten = 1.0 - (rippleLookup.y / rippleAttenDist);

        // if the distance from the appropriate sphere is the same as the
        // current radius, then this ripple is affecting this fragment
        //
        // this is modified by the attenuation
        // for just white light
        //nRipplesAffecting += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;

        // for colored lights
        lightMod[sphereIdx] += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;
    }

    // find the color modification for this spot and add it to the color, this
    // is a flat increase in lightness (adding white light)
    // for just white light
    //out_Color = vs_Color + vec4(bandStrength * nRipplesAffecting);

    // for tri lights
    out_Color = vs_Color + lightMod.brga * bandStrength;

    // ensure that everything is opaque
    out_Color.a = 1.0f;
}
