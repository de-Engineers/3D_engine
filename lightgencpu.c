#include "main.h"
#include "vec3.h"
#include "raytracing.h"
#include "ray.h"

VEC3 rayColor;

typedef struct {
    union{
        VEC3 Pos;
        VEC3 Dir;
    };
    f32 rndOffset;
}RAYPROP;

RAYPROP rayy;

u64 threadItt;

void castLightRay(RAY3D ray,VEC3 color){
    VEC3 dir;
    VEC2 wall;
    while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
        u32 block = crds2map(ray.roundPos.x, ray.roundPos.y, ray.roundPos.z);
        switch (map[block].id){
        case BLOCK_AIR:
            if((rnd()-1.0f)<1.0f/(properties->lmapSz*properties->lmapSz)){
                lpmap[block].p1 += color.r*255.0f;
                lpmap[block].p2 += color.g*255.0f;
                lpmap[block].p3 += color.b*255.0f;
            }
            break;
        case BLOCK_CUBE:{
            VEC3 spos = getSubCoords(ray);
            VEC3 mtdt = { (f32)metadt[block].g / 255.0f,(f32)metadt[block].r / 255.0f,(f32)metadt[block].id / 255.0f };
            VEC3 mtdt2 = { (f32)metadt2[block].g / 255.0f,(f32)metadt2[block].r / 255.0f,(f32)metadt2[block].id / 255.0f };
            VEC3 rotdir = ray.dir;
            (VEC2){mtdt.x,mtdt.y}     = rotVEC2((VEC2){mtdt.x,mtdt.y},(f32)metadt3[block].r / 255.0f * PI);
            (VEC2){mtdt.x,mtdt.z}     = rotVEC2((VEC2){mtdt.x,mtdt.z},(f32)metadt3[block].g / 255.0f * PI);
            (VEC2){mtdt.y,mtdt.z}     = rotVEC2((VEC2){mtdt.y,mtdt.z},(f32)metadt3[block].id / 255.0f * PI);
            (VEC2){spos.x,spos.y}     = rotVEC2((VEC2){spos.x,spos.y},(f32)metadt3[block].r / 255.0f * PI);
            (VEC2){spos.x,spos.z}     = rotVEC2((VEC2){spos.x,spos.z},(f32)metadt3[block].g / 255.0f * PI);
            (VEC2){spos.y,spos.z}     = rotVEC2((VEC2){spos.y,spos.z},(f32)metadt3[block].id / 255.0f * PI);
            (VEC2){rotdir.x,rotdir.y} = rotVEC2((VEC2){rotdir.x,rotdir.y},(f32)metadt3[block].r / 255.0f * PI);
            (VEC2){rotdir.x,rotdir.z} = rotVEC2((VEC2){rotdir.x,rotdir.z},(f32)metadt3[block].g / 255.0f * PI);
            (VEC2){rotdir.y,rotdir.z} = rotVEC2((VEC2){rotdir.y,rotdir.z},(f32)metadt3[block].id / 255.0f * PI);
            f32 d = iBox(VEC3subVEC3R(spos,mtdt),rotdir,(VEC3){metadt2[block].g/255.0f,metadt2[block].r/255.0f,metadt2[block].id/255.0f});
            if(d > 0.0f){
                VEC3addVEC3(&spos,VEC3mulR(rotdir,d));
                VEC3 nspos = spos;
                (VEC2){nspos.y,nspos.z} = rotVEC2((VEC2){nspos.y,nspos.z}, -(f32)metadt3[block].r / 255.0f * PI);
                (VEC2){nspos.x,nspos.z} = rotVEC2((VEC2){nspos.x,nspos.z}, -(f32)metadt3[block].g / 255.0f * PI);
                (VEC2){nspos.x,nspos.y} = rotVEC2((VEC2){nspos.x,nspos.y}, -(f32)metadt3[block].id / 255.0f * PI);
                if(nspos.x >= 0.0f && nspos.y >= 0.0f && nspos.z >= 0.0f && nspos.x <= 1.0f && nspos.y <= 1.0f && nspos.z <= 1.0f){
                    color.r *= (f32)map[block].r / 256.0f;
                    color.g *= (f32)map[block].g / 256.0f;
                    color.b *= (f32)map[block].b / 256.0f;
                    if(spos.x > mtdt.x - mtdt2.x - 0.0001f && spos.x < mtdt.x - mtdt2.x + 0.0001f){
                        f32 mt = fmaxf(mtdt2.y, mtdt2.z) * 2.0f;
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
                    else if(spos.y > mtdt.y - mtdt2.y - 0.0001f && spos.y < mtdt.y - mtdt2.y + 0.0001f){
                        f32 mt = fmaxf(mtdt2.x, mtdt2.z) * 2.0f;
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
                    else if(spos.z > mtdt.z - mtdt2.z - 0.0001f && spos.z < mtdt.z - mtdt2.z + 0.0001f){
                        f32 mt = fmaxf(mtdt2.x, mtdt2.y) * 2.0f;
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
                    else if(spos.x > mtdt.x + mtdt2.x - 0.0001f && spos.x < mtdt.x + mtdt2.x + 0.0001f){
                        f32 mt = fmaxf(mtdt2.y, mtdt2.z) * 2.0f;
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
                    else if(spos.y > mtdt.y + mtdt2.y - 0.0001f && spos.y < mtdt.y + mtdt2.y + 0.0001f){
                        f32 mt = fmaxf(mtdt2.x, mtdt2.z) * 2.0f;
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
                    else if(spos.z > mtdt.z + mtdt2.z - 0.0001f && spos.z < mtdt.z + mtdt2.z + 0.0001f){
                        f32 mt = fmaxf(mtdt2.x, mtdt2.y) * 2.0f;
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
                    (VEC2){dir.x,dir.y} = rotVEC2((VEC2){dir.x,dir.y}, (f32) { metadt3[block].r } / 255.0f * PI);
                    (VEC2){dir.x,dir.z} = rotVEC2((VEC2){dir.x,dir.z}, (f32) { metadt3[block].g } / 255.0f * PI);
                    (VEC2){dir.y,dir.z} = rotVEC2((VEC2){dir.y,dir.z}, (f32) { metadt3[block].id } / 255.0f * PI);
                    ray = ray3dCreate(VEC3addVEC3R((VEC3){ray.roundPos.x,ray.roundPos.y,ray.roundPos.z },nspos), VEC3normalize(dir));
                    if(color.r < 0.01f && color.g < 0.01f && color.b < 0.01f){
                        return;
                    }
                }
            }
            break;
        }
        case BLOCK_REFLECTIVE2:
        case BLOCK_REFLECTIVE:
        case BLOCK_SOLID:
            color.r *= (f32)map[block].r / 255.0f;
            color.g *= (f32)map[block].g / 255.0f;
            color.b *= (f32)map[block].b / 255.0f;
            switch(ray.hitSide){
            case 0:{
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
                    ray = ray3dCreate((VEC3) { ray.roundPos.x, ray.roundPos.y+ fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y), ray.roundPos.z + fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z) }, dir);
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
                    ray = ray3dCreate((VEC3) { ray.roundPos.x + 1, ray.roundPos.y + fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y), ray.roundPos.z + fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z) }, dir);
                }
                break;
            }
            case 1:{   
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
                    ray = ray3dCreate((VEC3) { ray.roundPos.x + fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x), ray.roundPos.y, ray.roundPos.z + fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z) }, dir);
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
                    ray = ray3dCreate((VEC3) { ray.roundPos.x + fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x), ray.roundPos.y + 1, ray.roundPos.z + fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z) }, dir);
                }
                break;
            }
            case 2:{
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
                    ray = ray3dCreate((VEC3) { ray.roundPos.x + fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x), ray.roundPos.y + fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y), ray.roundPos.z }, dir);
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
                    ray = ray3dCreate((VEC3) { ray.roundPos.x + fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x), ray.roundPos.y + fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y), ray.roundPos.z + 1 }, dir);
                }
                break;
            }
            }
            if(color.r < 0.01f && color.g < 0.01f && color.b < 0.01f){
                return;
            }
            break;
        }
        ray3dItterate(&ray);
    }
}

void lightRaysGenerate(){
    for(u64 i = 0;i < threadItt;i++){
        VEC3 dir;
        for(int i = 0; i < 3; i++) {
            VEC3addVEC3(&dir,(VEC3){(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f,(rnd() - 1.5f) * 2.0f});
        }
        dir = VEC3normalize(dir);
        VEC3 pos = VEC3addVEC3R(rayy.Pos,(VEC3){(rnd()-1.5f)*rayy.rndOffset,(rnd()-1.5f)*rayy.rndOffset,(rnd()-1.5f)*rayy.rndOffset});
        castLightRay(ray3dCreate(pos,dir),rayColor);
        Sleep(0);
    }
}

void lightRaysGenerateAmbientX(){
    if(rayy.Dir.x < 0.0f){
        for(u64 i = 0;i < threadItt;i++){
            castLightRay(ray3dCreate((VEC3){ (f32)properties->lvlSz-1.0f,(rnd()-1.0f)*(properties->lvlSz-1.0f),(rnd()-1.0f)*(properties->lvlSz-1.0f)},rayy.Dir),rayColor);
            Sleep(0);
        }
    }
    else{
        for(u64 i = 0;i < threadItt;i++){
            castLightRay(ray3dCreate((VEC3){ 0.0f, (rnd()-1.0f)*(properties->lvlSz-1.0f),(rnd()-1.0f)*(properties->lvlSz-1.0f)},rayy.Dir),rayColor);
            Sleep(0);
        }
    }
}

void lightRaysGenerateAmbientY(){
    if(rayy.Dir.y < 0.0f){
        for(u64 i = 0;i < threadItt;i++){
            castLightRay(ray3dCreate((VEC3){ (rnd()-1.0f)*(properties->lvlSz-1.0f),(f32)properties->lvlSz-1.0f,(rnd()-1.0f)*(properties->lvlSz-1.0f)},rayy.Dir),rayColor);
            Sleep(0);
        }
    }
    else{
        for(u64 i = 0;i < threadItt;i++){
            castLightRay(ray3dCreate((VEC3){ (rnd()-1.0f)*(properties->lvlSz-1.0f),0.0f,(rnd()-1.0f)*(properties->lvlSz-1.0f)},rayy.Dir),rayColor);
            Sleep(0);
        }
    }
}

void lightRaysGenerateAmbientZ(){
    if(rayy.Dir.z < 0.0f){
        for(u64 i = 0;i < threadItt;i++){
            castLightRay(ray3dCreate((VEC3){ (rnd()-1.0f)*(properties->lvlSz-1.0f),(rnd()-1.0f)*(properties->lvlSz-1.0f),(f32)properties->lvlSz-1.0f},rayy.Dir),rayColor);
            Sleep(0);
        }
    }
    else{
        for(u64 i = 0;i < threadItt;i++){
            castLightRay(ray3dCreate((VEC3){ (rnd()-1.0f)*(properties->lvlSz-1.0f),(rnd()-1.0f)*(properties->lvlSz-1.0f),0.0f },rayy.Dir),rayColor);
            Sleep(0);
        }
    }
}

void cpuGenLightAmbientX(VEC3 dir,VEC3 color,u64 itt){
    rayColor = color;
    rayy.Dir = dir;
    threadItt = itt/8;
    HANDLE cpulightgenthreads[8];
    for(u64 i = 0;i < 8;i++){
        cpulightgenthreads[i] = CreateThread(0,0,lightRaysGenerateAmbientX,0,0,0);
    }
    WaitForMultipleObjects(8,cpulightgenthreads,1,INFINITE);
}

void cpuGenLightAmbientY(VEC3 dir,VEC3 color,u64 itt){
    rayColor = color;
    rayy.Dir = dir;
    threadItt = itt/8;
    HANDLE cpulightgenthreads[8];
    for(u64 i = 0;i < 8;i++){
        cpulightgenthreads[i] = CreateThread(0,0,lightRaysGenerateAmbientY,0,0,0);
    }
    WaitForMultipleObjects(8,cpulightgenthreads,1,INFINITE);
}

void cpuGenLightAmbientZ(VEC3 dir,VEC3 color,u64 itt){
    rayColor = color;
    threadItt = itt/8;
    rayy.Dir = dir;
    HANDLE cpulightgenthreads[8];
    for(u64 i = 0;i < 8;i++){
        cpulightgenthreads[i] = CreateThread(0,0,lightRaysGenerateAmbientZ,0,0,0);
    }
    WaitForMultipleObjects(8,cpulightgenthreads,1,INFINITE);
}

void cpuGenLight(VEC3 pos,VEC3 color,u64 itt,f32 rndOffset){
    rayy.Pos = pos;
    rayy.rndOffset = rndOffset;
    rayColor = color;
    threadItt = itt/8;
    HANDLE cpulightgenthreads[8];
    for(u64 i = 0;i < 8;i++){
        cpulightgenthreads[i] = CreateThread(0,0,lightRaysGenerate,0,0,0);
    }
    WaitForMultipleObjects(8,cpulightgenthreads,1,INFINITE);
}