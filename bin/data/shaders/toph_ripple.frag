#version 330

// ripple uniforms
uniform int nRipples; // number of ripples
uniform float bandStrength; // base amount of light that each band adds
uniform float rippleAttenDist; // distance at which a band fades out

uniform vec2 screenDim;

// need to store:
//  - band widths
//  - band radius
//  - band origin (L,C,R)
uniform samplerBuffer ripples;

in vec4 vs_Color;
in vec3 vs_distToSpheres;
out vec4 out_Color;

bool fequals(float a, float b) {
    return abs(a - b) < 0.00001;
}

bool fequals(float a, float b, float epsilon) {
    return abs(a - b) < epsilon;
}

void main(void) {

    float nRipplesAffecting;

    // find out how many ripples are affecting this spot currently
    for (int i = 0; i < nRipples; ++i) {
        // look up the current ripple
        vec3 rippleLookup = texelFetch(ripples, i).xyz;

        // get the origin index
        int sphereIdx = int(rippleLookup.z);

        //float rippleAtten = 1.0 - (rippleLookup.x / rippleAttenDist);
        float rippleAtten = 1.0;

        // if the distance from the appropriate sphere is the same as the
        // current radius, then this ripple is affecting this fragment
        //
        // this is modified by the attenuation
        nRipplesAffecting += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;
    }

    // find the color modification for this spot and add it to the color, this
    // is a flat increase in lightness (adding white light)
    out_Color = vs_Color + vec4(bandStrength * nRipplesAffecting);
    out_Color.a = 1.0f;
}
