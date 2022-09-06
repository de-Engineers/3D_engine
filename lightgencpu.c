#include "main.h"
#include "vec3.h"
#include "raytracing.h"

VEC3 rayColor,rayPos;
u64 threadItt;

inline void castLightRay(RAY ray,VEC3 color){
    VEC3 dir;
    VEC2 wall;
    while (ray.ix >= 0 && ray.iy >= 0 && ray.iz >= 0 && ray.ix < 64 && ray.iy < 64 && ray.iz < 64) {
        u32 block = crds2map(ray.ix, ray.iy, ray.iz);
        switch (map[block].id) {
        case 12:{
            VEC3 spos = getSubCoords(ray);
            VEC3 mtdt = { (float)metadt[block].r / 255.0f,(float)metadt[block].g / 255.0f,(float)metadt[block].id / 255.0f };
            VEC3 mtdt2 = { (float)metadt2[block].r / 255.0f,(float)metadt2[block].g / 255.0f,(float)metadt2[block].id / 255.0f };
            VEC3 rotdir = ray.dir;
            VEC2 tempv;
            tempv = rotVEC2((VEC2){mtdt.x,mtdt.y},(float)metadt3[block].r / 255.0f * PI);
            mtdt.x = tempv.x;
            mtdt.y = tempv.y;
            tempv = rotVEC2((VEC2){mtdt.x,mtdt.z},(float)metadt3[block].g / 255.0f * PI);
            mtdt.x = tempv.x;
            mtdt.z = tempv.y;
            tempv = rotVEC2((VEC2){mtdt.y,mtdt.z},(float)metadt3[block].id / 255.0f * PI);
            mtdt.y = tempv.x;
            mtdt.z = tempv.y;
            tempv = rotVEC2((VEC2){spos.x,spos.y},(float)metadt3[block].r / 255.0f * PI);
            spos.x = tempv.x;
            spos.y = tempv.y;
            tempv = rotVEC2((VEC2){spos.x,spos.z},(float)metadt3[block].g / 255.0f * PI);
            spos.x = tempv.x;
            spos.z = tempv.y;
            tempv = rotVEC2((VEC2){spos.y,spos.z},(float)metadt3[block].id / 255.0f * PI);
            spos.y = tempv.x;
            spos.z = tempv.y;
            tempv = rotVEC2((VEC2){rotdir.x,rotdir.y},(float)metadt3[block].r / 255.0f * PI);
            rotdir.x = tempv.x;
            rotdir.y = tempv.y;
            tempv = rotVEC2((VEC2){rotdir.x,rotdir.z},(float)metadt3[block].g / 255.0f * PI);
            rotdir.x = tempv.x;
            rotdir.z = tempv.y;
            tempv = rotVEC2((VEC2){rotdir.y,rotdir.z},(float)metadt3[block].id / 255.0f * PI);
            rotdir.y = tempv.x;
            rotdir.z = tempv.y;
            f32 d = iBox(VEC3subVEC3R(spos,mtdt),rotdir,(VEC3){metadt2[block].r/255.0f,metadt2[block].g/255.0f,metadt2[block].id/255.0f});
            if(d > 0.0f){
                VEC3addVEC3(&spos,VEC3mulR(rotdir,d));
                VEC3 nspos = spos;
                tempv = rotVEC2((VEC2){nspos.y,nspos.z}, -(float)metadt3[block].r / 255.0f * PI);
                nspos.y = tempv.x;
                nspos.z = tempv.y;
                tempv = rotVEC2((VEC2){nspos.x,nspos.z}, -(float)metadt3[block].g / 255.0f * PI);
                nspos.x = tempv.x;
                nspos.z = tempv.y;
                tempv = rotVEC2((VEC2){nspos.x,nspos.y}, -(float)metadt3[block].id / 255.0f * PI);
                nspos.x = tempv.x;
                nspos.y = tempv.y;
                if (nspos.x >= 0.0f && nspos.y >= 0.0f && nspos.z >= 0.0f && nspos.x <= 1.0f && nspos.y <= 1.0f && nspos.z <= 1.0f) {
                    color.r *= (float)map[block].r / 256.0f;
                    color.g *= (float)map[block].g / 256.0f;
                    color.b *= (float)map[block].b / 256.0f;
                    if (spos.x > mtdt.x - mtdt2.x - 0.0001f && spos.x < mtdt.x - mtdt2.x + 0.0001f) {
                        float mt = fmaxf(mtdt2.y, mtdt2.z) * 2.0;
                        u32 xt = (spos.y - mtdt.y + mtdt2.y) * properties->lmapSz / mt;
                        u32 yt = (spos.z - mtdt.z + mtdt2.z) * properties->lmapSz / mt;
                        u32 offset = xt + yt * properties->lmapSz;
                        lmapb[(lpmap[block].p1 * properties->lmapSz * properties->lmapSz + offset)].r += color.r / mt / mt;
                        lmapb[(lpmap[block].p1 * properties->lmapSz * properties->lmapSz + offset)].g += color.g / mt / mt;
                        lmapb[(lpmap[block].p1 * properties->lmapSz * properties->lmapSz + offset)].b += color.b / mt / mt;
                        dir = (VEC3){ (rnd() - 2.0f),(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f };
                        for (int i = 0; i < 2; i++) {
                            VEC3addVEC3(&dir,(VEC3){ (rnd() - 2.0f),(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f });
                        }
                    }
                    if (spos.y > mtdt.y - mtdt2.y - 0.0001f && spos.y < mtdt.y - mtdt2.y + 0.0001f) {
                        float mt = fmaxf(mtdt2.x, mtdt2.z) * 2.0;
                        u32 xt = (spos.x - mtdt.x + mtdt2.x) * properties->lmapSz / mt;
                        u32 yt = (spos.z - mtdt.z + mtdt2.z) * properties->lmapSz / mt;
                        u32 offset = xt + yt * properties->lmapSz;
                        lmapb[(lpmap[block].p2 * properties->lmapSz * properties->lmapSz + offset)].r += color.r / mt / mt;
                        lmapb[(lpmap[block].p2 * properties->lmapSz * properties->lmapSz + offset)].g += color.g / mt / mt;
                        lmapb[(lpmap[block].p2 * properties->lmapSz * properties->lmapSz + offset)].b += color.b / mt / mt;
                        dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 2.0f),(rnd() - 1.5f) * 2.0f };
                        for (int i = 0; i < 2; i++) {
                            VEC3addVEC3(&dir,(VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 2.0f),(rnd() - 1.5f) * 2.0f });
                        }
                    }
                    if (spos.z > mtdt.z - mtdt2.z - 0.0001f && spos.z < mtdt.z - mtdt2.z + 0.0001f) {
                        float mt = fmaxf(mtdt2.x, mtdt2.y) * 2.0;
                        u32 xt = (spos.x - mtdt.x + mtdt2.x) * properties->lmapSz / mt;
                        u32 yt = (spos.y - mtdt.y + mtdt2.y) * properties->lmapSz / mt;
                        u32 offset = xt + yt * properties->lmapSz;
                        lmapb[(lpmap[block].p3 * properties->lmapSz * properties->lmapSz + offset)].r += color.r / mt / mt;
                        lmapb[(lpmap[block].p3 * properties->lmapSz * properties->lmapSz + offset)].g += color.g / mt / mt;
                        lmapb[(lpmap[block].p3 * properties->lmapSz * properties->lmapSz + offset)].b += color.b / mt / mt;
                        dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 2.0f) };
                        for (int i = 0; i < 2; i++) {
                            VEC3addVEC3(&dir,(VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 2.0f) });
                        }
                    }
                    if (spos.x > mtdt.x + mtdt2.x - 0.0001f && spos.x < mtdt.x + mtdt2.x + 0.0001f) {
                        float mt = fmaxf(mtdt2.y, mtdt2.z) * 2.0;
                        u32 xt = (spos.y - mtdt.y + mtdt2.y) * properties->lmapSz / mt;
                        u32 yt = (spos.z - mtdt.z + mtdt2.z) * properties->lmapSz / mt;
                        u32 offset = xt + yt * properties->lmapSz;
                        lmapb[(lpmap[block].p4 * properties->lmapSz * properties->lmapSz + offset)].r += color.r / mt / mt;
                        lmapb[(lpmap[block].p4 * properties->lmapSz * properties->lmapSz + offset)].g += color.g / mt / mt;
                        lmapb[(lpmap[block].p4 * properties->lmapSz * properties->lmapSz + offset)].b += color.b / mt / mt;
                        dir = (VEC3){ (rnd() - 1.0f),(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f };
                        for (int i = 0; i < 2; i++) {
                            VEC3addVEC3(&dir,(VEC3){ (rnd() - 1.0f),(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f });
                        }
                    }
                    if (spos.y > mtdt.y + mtdt2.y - 0.0001f && spos.y < mtdt.y + mtdt2.y + 0.0001f) {
                        float mt = fmaxf(mtdt2.x, mtdt2.z) * 2.0;
                        u32 xt = (spos.x - mtdt.x + mtdt2.x) * properties->lmapSz / mt;
                        u32 yt = (spos.z - mtdt.z + mtdt2.z) * properties->lmapSz / mt;
                        u32 offset = xt + yt * properties->lmapSz;
                        lmapb[(lpmap[block].p5 * properties->lmapSz * properties->lmapSz + offset)].r += color.r / mt / mt;
                        lmapb[(lpmap[block].p5 * properties->lmapSz * properties->lmapSz + offset)].g += color.g / mt / mt;
                        lmapb[(lpmap[block].p5 * properties->lmapSz * properties->lmapSz + offset)].b += color.b / mt / mt;
                        dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.0f),(rnd() - 1.5f) * 2.0f };
                        for (int i = 0; i < 2; i++) {
                            VEC3addVEC3(&dir,(VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.0f),(rnd() - 1.5f) * 2.0f });
                        }
                    }
                    if (spos.z > mtdt.z + mtdt2.z - 0.0001f && spos.z < mtdt.z + mtdt2.z + 0.0001f) {
                        float mt = fmaxf(mtdt2.x, mtdt2.y) * 2.0;
                        u32 xt = (spos.x - mtdt.x + mtdt2.x) * properties->lmapSz / mt;
                        u32 yt = (spos.y - mtdt.y + mtdt2.y) * properties->lmapSz / mt;
                        u32 offset = xt + yt * properties->lmapSz;
                        lmapb[(lpmap[block].p6 * properties->lmapSz * properties->lmapSz + offset)].r += color.r / mt / mt;
                        lmapb[(lpmap[block].p6 * properties->lmapSz * properties->lmapSz + offset)].g += color.g / mt / mt;
                        lmapb[(lpmap[block].p6 * properties->lmapSz * properties->lmapSz + offset)].b += color.b / mt / mt;
                        dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 1.0f) };
                        for (int i = 0; i < 2; i++) {
                            VEC3addVEC3(&dir,(VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 1.0f) });
                        }
                    }
                    tempv = rotVEC2((VEC2){dir.x,dir.y}, (float) { metadt3[block].r } / 255.0 * PI);
                    dir.x = tempv.x;
                    dir.y = tempv.y;
                    tempv = rotVEC2((VEC2){dir.x,dir.z}, (float) { metadt3[block].g } / 255.0 * PI);
                    dir.x = tempv.x;
                    dir.z = tempv.y;
                    tempv = rotVEC2((VEC2){dir.y,dir.z}, (float) { metadt3[block].id } / 255.0 * PI);
                    dir.y = tempv.x;
                    dir.z = tempv.y;
                    ray = rayCreate(VEC3addVEC3R((VEC3){ray.ix,ray.iy,ray.iz },nspos), VEC3normalize(dir));
                    if (color.r < 0.01f && color.g < 0.01f && color.b < 0.01f) {
                        return;
                    }
                }
            }
            break;
        }
        case 28:
            color.r *= (f32)map[block].r / 255.0f;
            color.g *= (f32)map[block].g / 255.0f;
            color.b *= (f32)map[block].b / 255.0f;
            switch (ray.sid) {
            case 0: {
                wall.x = fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
                wall.y = fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
                u32 offset = (int)(fract(wall.y) * properties->lmapSz) * properties->lmapSz + (int)(fract(wall.x) * properties->lmapSz);
                if (ray.dir.x > 0.0f) {
                    lmapb[(lpmap[block].p1 * properties->lmapSz * properties->lmapSz + offset)].r += color.r;
                    lmapb[(lpmap[block].p1 * properties->lmapSz * properties->lmapSz + offset)].g += color.g;
                    lmapb[(lpmap[block].p1 * properties->lmapSz * properties->lmapSz + offset)].b += color.b;
                    dir = (VEC3){ (rnd() - 2.0f),(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f };
                    for (int i = 0; i < 2; i++) {
                        VEC3addVEC3(&dir, (VEC3) { (rnd() - 2.0f), (rnd() - 1.5f) * 2.0f, (rnd() - 1.5f) * 2.0f });
                    }
                    dir = VEC3normalize(dir);
                    ray = rayCreate((VEC3) { ray.ix, ray.iy + fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y), ray.iz + fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z) }, dir);
                }
                else {
                    lmapb[(lpmap[block].p2 * properties->lmapSz * properties->lmapSz + offset)].r += color.r;
                    lmapb[(lpmap[block].p2 * properties->lmapSz * properties->lmapSz + offset)].g += color.g;
                    lmapb[(lpmap[block].p2 * properties->lmapSz * properties->lmapSz + offset)].b += color.b;
                    dir = (VEC3){ (rnd() - 1.0f),(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f };
                    for (int i = 0; i < 2; i++) {
                        VEC3addVEC3(&dir, (VEC3) { (rnd() - 1.0f), (rnd() - 1.5f) * 2.0f, (rnd() - 1.5f) * 2.0f });
                    }
                    dir = VEC3normalize(dir);
                    ray = rayCreate((VEC3) { ray.ix + 1, ray.iy + fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y), ray.iz + fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z) }, dir);
                }
                break;
            }
            case 1: {   
                wall.x = fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
                wall.y = fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
                u32 offset = (int)(fract(wall.y) * properties->lmapSz) * properties->lmapSz + (int)(fract(wall.x) * properties->lmapSz);
                if (ray.dir.y > 0.0f) {
                    lmapb[(lpmap[block].p3 * properties->lmapSz * properties->lmapSz + offset)].r += color.r;
                    lmapb[(lpmap[block].p3 * properties->lmapSz * properties->lmapSz + offset)].g += color.g;
                    lmapb[(lpmap[block].p3 * properties->lmapSz * properties->lmapSz + offset)].b += color.b;
                    dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 2.0f),(rnd() - 1.5f) * 2.0f };
                    for (int i = 0; i < 2; i++) {
                        VEC3addVEC3(&dir, (VEC3) { (rnd() - 1.5f) * 2.0f, (rnd() - 2.0f), (rnd() - 1.5f) * 2.0f });
                    }
                    dir = VEC3normalize(dir);
                    ray = rayCreate((VEC3) { ray.ix + fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x), ray.iy, ray.iz + fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z) }, dir);
                }
                else {
                    lmapb[(lpmap[block].p4 * properties->lmapSz * properties->lmapSz + offset)].r += color.r;
                    lmapb[(lpmap[block].p4 * properties->lmapSz * properties->lmapSz + offset)].g += color.g;
                    lmapb[(lpmap[block].p4 * properties->lmapSz * properties->lmapSz + offset)].b += color.b;
                    dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.0f),(rnd() - 1.5f) * 2.0f };
                    for (int i = 0; i < 2; i++) {
                        VEC3addVEC3(&dir, (VEC3) { (rnd() - 1.5f) * 2.0f, (rnd() - 1.0f), (rnd() - 1.5f) * 2.0f });
                    }
                    dir = VEC3normalize(dir);
                    ray = rayCreate((VEC3) { ray.ix + fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x), ray.iy + 1, ray.iz + fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z) }, dir);
                }
                break;
            }
            case 2: {
                wall.x = fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
                wall.y = fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
                u32 offset = (int)(fract(wall.y) * properties->lmapSz) * properties->lmapSz + (int)(fract(wall.x) * properties->lmapSz);
                if (ray.dir.z > 0.0f) {
                    lmapb[(lpmap[block].p5 * properties->lmapSz * properties->lmapSz + offset)].r += color.r;
                    lmapb[(lpmap[block].p5 * properties->lmapSz * properties->lmapSz + offset)].g += color.g;
                    lmapb[(lpmap[block].p5 * properties->lmapSz * properties->lmapSz + offset)].b += color.b;
                    dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 2.0f) };
                    for (int i = 0; i < 2; i++) {
                        VEC3addVEC3(&dir, (VEC3) { (rnd() - 1.5f) * 2.0f, (rnd() - 1.5f) * 2.0f, (rnd() - 2.0f) });
                    }
                    dir = VEC3normalize(dir);
                    ray = rayCreate((VEC3) { ray.ix + fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x), ray.iy + fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y), ray.iz }, dir);
                }
                else {
                    lmapb[(lpmap[block].p6 * properties->lmapSz * properties->lmapSz + offset)].r += color.r;
                    lmapb[(lpmap[block].p6 * properties->lmapSz * properties->lmapSz + offset)].g += color.g;
                    lmapb[(lpmap[block].p6 * properties->lmapSz * properties->lmapSz + offset)].b += color.b;
                    dir = (VEC3){ (rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 1.0f) };
                    for (int i = 0; i < 2; i++) {
                        VEC3addVEC3(&dir, (VEC3) { (rnd() - 1.5f) * 2.0f, (rnd() - 1.5f) * 2.0f, (rnd() - 1.0f) });
                    }
                    dir = VEC3normalize(dir);
                    ray = rayCreate((VEC3) { ray.ix + fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x), ray.iy + fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y), ray.iz + 1 }, dir);
                }
                break;
            }
            if(color.r < 0.01f && color.g < 0.01f && color.b < 0.01f){
                return;
            }
            break;
            }
        }
        rayItterate(&ray);
    }
}

void lightRaysGenerate(){
    for(u64 i = 0;i < threadItt;i++){
        VEC3 dir;
        for(int i = 0; i < 3; i++) {
            VEC3addVEC3(&dir,(VEC3) { (rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f });
        }
        dir = VEC3normalize(dir);
        castLightRay(rayCreate(rayPos,dir),rayColor);
        Sleep(0);
    }
}

void cpuGenLight(VEC3 pos,VEC3 color,u64 itt){
    rayPos = pos;
    rayColor = color;
    threadItt = itt/8;
    HANDLE cpulightgenthreads[8];
    for(u64 i = 0;i < 8;i++){
        cpulightgenthreads[i] = CreateThread(0,0,lightRaysGenerate,0,0,0);
    }
    WaitForMultipleObjects(8,&cpulightgenthreads,1,INFINITE);
}