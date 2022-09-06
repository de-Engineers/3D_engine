#include "raytracing.h"

f32 iBox(VEC3 ro,VEC3 rd,VEC3 boxSize) {
    VEC3 m = VEC3divFR(rd,1.0f);
    VEC3 n = VEC3mulVEC3R(m,ro);
    VEC3 k = VEC3mulVEC3R(VEC3absR(m),boxSize);
    VEC3 t1 = VEC3subVEC3R(VEC3subVEC3R(n,VEC3mulR(n,2.0f)),k);
    VEC3 t2 = VEC3addVEC3R(VEC3subVEC3R(n,VEC3mulR(n,2.0f)),k);
    f32 tN = fmaxf(fmaxf(t1.x,t1.y),t1.z);
    f32 tF = fminf(fminf(t2.x,t2.y),t2.z);
    if(tN > tF || tF < 0.0f) {
        return -1.0f;
    }
    return tN;
}

f32 iSphere(VEC3 ro,VEC3 rd,f32 ra) {
    f32 b = VEC3dot(ro,rd);
    f32 c = VEC3dot(ro,ro) - ra * ra;
    f32 h = b * b - c;
    if(h < 0.0) {
        return -1.0;
    }
    return -b - sqrtf(h);
}