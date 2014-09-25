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
uniform sampler2D ripples;

//uniform vec4 sphereLpos;
//uniform vec4 sphereCpos;
//uniform vec4 sphereRpos;

in vec4 vs_Color;
//in vec4 vs_Pos;
in vec3 vs_distToSpheres;
out vec4 out_Color;

bool fequals(float a, float b) {
    return abs(a - b) < 0.00001;
}

bool fequals(float a, float b, float epsilon) {
    return abs(a - b) < epsilon;
}

void main(void) {
    //// calculate the distance from the spheres
    //vec4 diffL = sphereLpos - vs_Pos;
    //vec4 diffC = sphereCpos - vs_Pos;
    //vec4 diffR = sphereRpos - vs_Pos;
    //vec3 vs_distToSpheres = vec3(sqrt(dot(diffL, diffL)), 
    //                        sqrt(dot(diffC, diffC)),
    //                        sqrt(dot(diffR, diffR)));

    float nRipplesAffecting;
    float idx_amt = 1.0f / nRipples;

    // find out how many ripples are affecting this spot currently
    for (int i = 0; i < nRipples; ++i) {
        // look up the current ripple
        vec3 rippleLookup = texture(ripples, clamp(vec2(idx_amt * float(i), 0.0f), 0.0f, 1.0f)).xyz;

        // get the origin index
        int sphereIdx = int(rippleLookup.z);

        float rippleAtten = 1.0 - (rippleLookup.x / rippleAttenDist);

        // if the distance from the appropriate sphere is the same as the
        // current radius, then this ripple is affecting this fragment
        //
        // this is modified by the attenuation
        nRipplesAffecting += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;
        //nRipplesAffecting += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x));
    }

    // find the color modification for this spot and add it to the color, this
    // is a flat increase in lightness (adding white light)
    nRipplesAffecting = float(fequals(vs_distToSpheres[0], 100.0f, 4.0f)) * (1.0 - (100.0f / rippleAttenDist));
    out_Color = clamp(vs_Color + vec4(bandStrength * nRipplesAffecting), 0.0f, 1.0f);
    out_Color = texture(ripples, vec2(0.0f)) / 255.0;
    //out_Color = vec4(normalize(vs_distToSpheres), 1.0f);
    //out_Color = vec4((vs_distToSpheres/1000), 1.0f);

    //out_Color = vec4(gl_FragCoord.xy / floor(screenDim / nRipples), 0.0, 1.0);;
    //out_Color = texture(ripples, vec2(0, 0));
}
