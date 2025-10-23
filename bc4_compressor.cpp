#include <stdio.h>
#include <stdlib.h>

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

static void compressDataBlock(u8* dataBlock, u8* buffer){
    u8 retOrder[] = {1, 7, 6, 5, 4, 3, 2, 0};
    u8 minRed = 255;
    u8 maxRed = 0;

    for(u32 i = 0; i < 16; i += 2){
        if(dataBlock[i] < minRed) minRed = dataBlock[i];
        if(dataBlock[i] > maxRed) maxRed = dataBlock[i];
    }

    u64* rBuf64 = (u64*)&buffer[0];
    *rBuf64 = maxRed;
    *rBuf64 |= ((u32)minRed << 8);
     
    u32 shiftCtr = 8;
    u8 redRange = maxRed - minRed;
    for(u32 i = 0; i < 16; i += 2){
        u8 r = dataBlock[i];
        f32 redRat = (f32)(r - minRed) / (f32)redRange;
        f32 redRet = redRat * 7.0f;
        u32 redRetI = redRet;
        f32 redDec = redRet - redRetI;
        u64 redBits = redDec > 0.5 ? retOrder[redRetI + 1] : retOrder[redRetI];
        *rBuf64 |= (redBits << shiftCtr);
        shiftCtr += 3;
    }
}

static u8* compressBC4(u8* uncompressedData, u32 width, u32 height, u32* compressedDataSize){
    *compressedDataSize = width * height;
    u32 compressedDataCtr = 0;
    u8* compressedData = (u8*)malloc(*compressedDataSize);

    for(u32 y = 0; y < height; y += 4){
        for(u32 x = 0; x < width; x += 4){
            u8 dataBlock[16] = {};
            u8 compressedBlock[8] = {};
            u32 dbCtr = 0;
            for(u32 j = y; j < y + 4; j++){
                for(u32 i = x; i < x + 4; i++){
                    u32 r = (j * width * 4) + (i * 4);
                    dataBlock[dbCtr++] = uncompressedData[r];
                }
            }
            compressDataBlock(dataBlock, compressedBlock);
            for(u32 i = 0; i < 8; i++){
                compressedData[compressedDataCtr++] = compressedBlock[i];
            }
        }
    }

    return compressedData;
}