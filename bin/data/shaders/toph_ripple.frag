#version 330

// TODO: eventually combine groups of uniforms into blocks
// ripple uniforms
uniform int nRipples; // number of ripples
uniform float bandStrength; // base amount of light that each band adds
uniform float rippleAttenDist; // distance at which a band fades out
uniform float rippleInitRadius;

uniform vec4 sphereLpos;
uniform vec4 sphereCpos;
uniform vec4 sphereRpos;
uniform vec3 sphereRadii;

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

bvec3 IsOnSphere(vec3 dist) {
    return bvec3(equal(dist, sphereRadii) || lessThan(dist, vec3(30.0f)));
}

void main(void) {
    float nRipplesAffecting;

    vec4 lightMod = vec4(0.0, 0.0, 0.0, 1.0);

    vec4 diffL = sphereLpos - vs_WorldPos;
    vec4 diffC = sphereCpos - vs_WorldPos;
    vec4 diffR = sphereRpos - vs_WorldPos;
    vec3 vs_distToSpheres = vec3(   length(diffL.xyz),
                                    length(diffC.xyz),
                                    length(diffR.xyz));
    // check how many ripples are new ripples
    vec3 newRipples = vec3(0.0f);

    // find out how many ripples are affecting this spot currently
    for (int i = 0; i < nRipples; ++i) {
        // look up the current ripple
        vec3 rippleLookup = texelFetch(ripples, i).xyz;

        // get the origin index
        int sphereIdx = int(rippleLookup.z);

        // ripple attenuation, make it fade out
        float rippleAtten = 1.0 - (rippleLookup.y / rippleAttenDist);

        // if the distance from the appropriate sphere is the same as the
        // current radius, then this ripple is affecting this fragment
        //
        // this is modified by the attenuation
        //
        // as a note, multiplying by booleans doesn't generate a branch, and as this would be a divergent branch, this (potentially) avoids
        // some performance loss without obfuscating things too much
        // -------------------
        // for just white light
        //nRipplesAffecting += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;
        // -------------------
        // for colored lights
        lightMod[sphereIdx] += float(fequals(vs_distToSpheres[sphereIdx], rippleLookup.y, rippleLookup.x)) * rippleAtten;

        // to make the spheres blink
        // if there is a new ripple, i.e. its radius is at or below the initial radius (start radius + 1 * dt * ripple_velocity, add 1 timestep of the speed)
        newRipples[sphereIdx] += float(rippleLookup.y <= rippleInitRadius);
    }

    // find the color modification for this spot and add it to the color, this
    // is a flat increase in lightness (adding white light)
    // for just white light
    //out_Color = vs_Color + vec4(bandStrength * nRipplesAffecting);

    // for tri lights
    vec3 onSpheres = vec3(IsOnSphere(vs_distToSpheres));
    float isSphereBottom = dot(vec4(0.0, -1.0, 0.0, 0.0), (vs_WorldPos - sphereLpos * onSpheres.x - sphereCpos * onSpheres.y - sphereRpos * onSpheres.z));
    // lol readability
    out_Color = vs_Color + lightMod.brga * bandStrength + vec4(newRipples * (isSphereBottom * float(isSphereBottom > 0.0) * 0.5) * bandStrength * onSpheres, 1.0f).brga;

    // ensure that everything is opaque
    out_Color.a = 1.0f;
}
