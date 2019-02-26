//
//  main.c
//  Edge_filter
//
//  Created by 장지선 on 2/8/19.
//  Copyright © 2019 Geesun. All rights reserved.
//

#include "bmp_format.h"

void fileInfo(BITMAPFILEHEADER *bh, BITMAPINFOHEADER *bi){
    printf("<BITMAP FILE HEADER>\n");
    printf("bfSize: %d\n",bh->bfSize);
    printf("bfOffBits: %d\n",bh->bfOffBits);
    
    printf("<BITMAP INFO HEADER>\n");
    printf("biSize: %d\n",bi->biSize);
    printf("biWidth: %d\n",bi->biWidth);
    printf("biHeight: %d\n",bi->biHeight);
    printf("biPlanes: %d\n",bi->biPlanes);
    printf("biBitCount: %d\n",bi->biBitCount);
    printf("biSizeImage: %d\n",bi->biSizeImage);
    printf("biXpixels: %d\n",bi->biXPelsPerMeter);
    printf("biYpixels: %d\n",bi->biYPelsPerMeter);
    printf("biClrUsed: %d\n",bi->biClrUsed);
    printf("biClrImportant: %d\n",bi->biClrImportant);
    
}

unsigned char* gray_palette_generate(){
    
    unsigned char *palette = (unsigned char*)malloc(sizeof(unsigned char)*GRAY_SCALE_PALETTE);
    unsigned char buffer[4];
    unsigned char* tmp = palette;
    
    buffer[3] = 0x00;
    
    for(int i=0; i<256; i++){
        buffer[0] = i;
        buffer[1] = buffer[0];
        buffer[2] = buffer[0];
        //printf("buf: %x %x %x %x \n",buffer[0],buffer[1],buffer[2],buffer[3]);
        memcpy(tmp, buffer, 4);
        
        //printf("pal1: %x %x %x %x \n", palette[0+i*4],palette[1+i*4],palette[2+i*4],palette[3+i*4]);
        tmp+=4;
    }
    
    return palette;
    
    
}

int xfilter_a[3][3] = {{-1,0,1}, {-1,0,1}, {-1,0,1}};
int yfilter_a[3][3] = {{-1,-2,1}, {0,0,0}, {-1,2,1}};

int xfilter_b[3][3] = {{-2,0,2}, {-2,0,2}, {-2,0,2}};
int yfilter_b[3][3] = {{-1,-2,1}, {0,0,0}, {-1,2,1}};

int xfilter_c[3][3] = {{0,1,2}, {-1,0,1}, {-2,-1,0}};
int yfilter_c[3][3] = {{-1,-2,1}, {0,0,0}, {-1,2,1}};

int xfilter_d[3][3] = {{0,1,2}, {-1,0,1}, {-2,-1,0}};
int yfilter_d[3][3] = {{-2,-1,0}, {-1,0,1}, {0,1,2}};

int xfilter_e[3][3] = {{-2,-1,0}, {-1,0,1}, {0,1,2}};
int yfilter_e[3][3] = {{-1,-2,-1}, {0,0,0}, {-1,2,1}};

int xfilter_f[3][3] = {{0,1,1}, {-1,0,1}, {-1,-1,0}};
int yfilter_f[3][3] = {{-1,-1,0}, {-1,0,1}, {0,1,1}};


int matrix_product(unsigned char m1[][3], int length, char index){
    int result1 = 0;
    int result2 = 0;
    int result = 0;
    int (*xfilter)[3];
    int (*yfilter)[3];
    if(index == 'A'){
        xfilter = xfilter_a;
        yfilter = yfilter_a;
    }
    else if(index == 'B'){
        xfilter = xfilter_b;
        yfilter = yfilter_b;
    }
    else if(index == 'C'){
        xfilter = xfilter_c;
        yfilter = yfilter_c;
    }
    else if(index == 'D'){
         xfilter = xfilter_d;
        yfilter = yfilter_d;
    }
    else if(index == 'E'){
         xfilter = xfilter_e;
        yfilter = yfilter_e;
    }
    else if(index == 'F'){
         xfilter = xfilter_f;
        yfilter = yfilter_f;
    }
    else {
        printf("ERROR: index missing\n");
        return 0;
    }
    
    
    for(int i=0; i<length; i++){
        for(int j=0; j<length; j++){
            result1 += xfilter[i][j]*m1[j][i];
            result2 += yfilter[i][j]*m1[j][i];
        }
    }
    
    //printf("result: %x\n", result);
    if(result1< 0) result1 = 0;
    if(result2< 0) result2 = 0;
    
    result = result1*result1 + result2*result2;
    result = (int)sqrt(result);
    return result;
}



int main(int argc, const char * argv[]) {
    
    FILE *fpBmp;                    // 비트맵 파일 포인터
    FILE *ftBmp_A;                    // 텍스트 파일 포인터
    FILE *ftBmp_B;                    // 텍스트 파일 포인터
    FILE *ftBmp_C;                    // 텍스트 파일 포인터
    FILE *ftBmp_D;                    // 텍스트 파일 포인터
    FILE *ftBmp_E;                    // 텍스트 파일 포인터
    FILE *ftBmp_F;                    // 텍스트 파일 포인터
    BITMAPFILEHEADER fileHeader;    // 비트맵 파일 헤더 구조체 변수
    BITMAPINFOHEADER infoHeader;    // 비트맵 정보 헤더 구조체 변수
    unsigned char* gray_palette;
    
    
    gray_palette = gray_palette_generate();
    
    
    //printf("\n");
    unsigned char *image;    // 픽셀 데이터 포인터
    int size;                // 픽셀 데이터 크기
    int width, height;       // 비트맵 이미지의 가로, 세로 크기
    int padding;             // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기
    
    // 각 픽셀을 표현할 ASCII 문자. 인덱스가 높을 수록 밝아지는 것을 표현
    char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' ' };   // 11개
    
    
    if(argc!=2){printf("Usage: <original.bmp>");}
    
    char* fileNameA;
    char* fileNameB;
    char* fileNameC;
    char* fileNameD;
    char* fileNameE;
    char* fileNameF;
    
    fileNameA = (char*)malloc(sizeof(char)*(strlen(argv[1])+5));
    fileNameB = (char*)malloc(sizeof(char)*(strlen(argv[1])+5));
    fileNameC = (char*)malloc(sizeof(char)*(strlen(argv[1])+5));
    fileNameD = (char*)malloc(sizeof(char)*(strlen(argv[1])+5));
    fileNameE = (char*)malloc(sizeof(char)*(strlen(argv[1])+5));
    fileNameF = (char*)malloc(sizeof(char)*(strlen(argv[1])+5));
    
    fileNameA[0] = '\0';
    fileNameB[0] = '\0';
    fileNameC[0] = '\0';
    fileNameD[0] = '\0';
    fileNameE[0] = '\0';
    fileNameF[0] = '\0';

    strcat(fileNameA, "filterA_");
    strcat(fileNameA,argv[1]);

    strcat(fileNameB, "filterB_");
    strcat(fileNameB,argv[1]);

    strcat(fileNameC, "filterC_");
    strcat(fileNameC,argv[1]);

    strcat(fileNameD, "filterD_");
    strcat(fileNameD,argv[1]);

    strcat(fileNameE, "filterE_");
    strcat(fileNameE,argv[1]);

    strcat(fileNameF, "filterF_");
    strcat(fileNameF,argv[1]);
    
    
    
    
    fpBmp = fopen(argv[1], "rb");    // 비트맵 파일을 바이너리 모드로 열기
    if (fpBmp == NULL)    // 파일 열기에 실패하면
        return 0;         // 프로그램 종료
    
    // 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }
    
    // 매직 넘버가 MB가 맞는지 확인(2바이트 크기의 BM을 리틀 엔디언으로 읽었으므로 MB가 됨)
    // 매직 넘버가 맞지 않으면 프로그램 종료
    if (fileHeader.bfType != 'MB')
    {
        fclose(fpBmp);
        return 0;
    }
    
    // 비트맵 정보 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }
    
    fileInfo(&fileHeader, &infoHeader);
    // 24비트 비트맵이 아니면 프로그램 종료
    
    size = infoHeader.biSizeImage;    // 픽셀 데이터 크기
    width = infoHeader.biWidth;       // 비트맵 이미지의 가로 크기
    height = infoHeader.biHeight;     // 비트맵 이미지의 세로 크기
    
    // 이미지의 가로 크기에 픽셀 크기를 곱하여 가로 한 줄의 크기를 구하고 4로 나머지를 구함
    // 그리고 4에서 나머지를 빼주면 남는 공간을 구할 수 있음.
    // 만약 남는 공간이 0이라면 최종 결과가 4가 되므로 여기서 다시 4로 나머지를 구함
    padding = (PIXEL_ALIGN - ((width * GRAY_PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;
    
    if (size == 0)    // 픽셀 데이터 크기가 0이라면
    {
        // 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
        // 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
        size = (width * GRAY_PIXEL_SIZE + padding) * height;
    }
    
    image = malloc(size);    // 픽셀 데이터의 크기만큼 동적 메모리 할당
    
    // 파일 포인터를 픽셀 데이터의 시작 위치로 이동
    fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);
    
    // 파일에서 픽셀 데이터 크기만큼 읽음. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
    if (fread(image, size, 1, fpBmp) < 1)
    {
        printf("read?\n");
        fclose(fpBmp);
        return 0;
    }
    
    fclose(fpBmp);    // 비트맵 파일 닫기
    
    
    
    /*--------------------------------------image reading completed------------------------------------------*/
    
    printf("filter?\n");
    ftBmp_A = fopen(fileNameA, "w");    // 결과 출력용 텍스트 파일 열기
    if (ftBmp_A == NULL)    // 파일 열기에 실패하면
    {
        free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제
        return 0;         // 프로그램 종료
    }
    unsigned char* filter_image;
    filter_image = malloc(size);    // 픽셀 데이터의 크기만큼 동적 메모리 할당
    
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, ftBmp_A);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, ftBmp_A);
    fwrite(gray_palette, sizeof(unsigned char)*GRAY_SCALE_PALETTE, 1, ftBmp_A);
    
    
    for(int i=0; i<height-2; i++){
        
        for(int j=0; j<width-2; j++){
            unsigned char buf_matrix[3][3]={0,};
            
            int index1, index2, index3, target, result;
            
            index1 = i*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index2 = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index3 = (i+2)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            target = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + (j+1)*GRAY_PIXEL_SIZE;
            
            memcpy(&buf_matrix[0][0], &image[index1], 3);
            memcpy(&buf_matrix[1][0], &image[index2], 3);
            memcpy(&buf_matrix[2][0], &image[index3], 3);
            
            result = matrix_product(buf_matrix, 3, 'A');
            filter_image[target] = (unsigned char)result;
            
            
        }
        
        
    }
    fwrite(filter_image, size, 1, ftBmp_A);


    ftBmp_B = fopen(fileNameB, "w");    // 결과 출력용 텍스트 파일 열기
    if (ftBmp_B == NULL)    // 파일 열기에 실패하면
    {
        free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제
        return 0;         // 프로그램 종료
    }
    
    
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, ftBmp_B);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, ftBmp_B);
    fwrite(gray_palette, sizeof(unsigned char)*GRAY_SCALE_PALETTE, 1, ftBmp_B);
    
    
    for(int i=0; i<height-2; i++){
        
        for(int j=0; j<width-2; j++){
            unsigned char buf_matrix[3][3]={0,};
            
            int index1, index2, index3, target, result;
            
            index1 = i*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index2 = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index3 = (i+2)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            target = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + (j+1)*GRAY_PIXEL_SIZE;
            
            memcpy(&buf_matrix[0][0], &image[index1], 3);
            memcpy(&buf_matrix[1][0], &image[index2], 3);
            memcpy(&buf_matrix[2][0], &image[index3], 3);
            
            result = matrix_product(buf_matrix, 3, 'B');
            filter_image[target] = (unsigned char)result;
            
            
        }
        
        
    }
    fwrite(filter_image, size, 1, ftBmp_B);

    ftBmp_C = fopen(fileNameC, "w");    // 결과 출력용 텍스트 파일 열기
    if (ftBmp_C == NULL)    // 파일 열기에 실패하면
    {
        free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제
        return 0;         // 프로그램 종료
    }
    
    
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, ftBmp_C);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, ftBmp_C);
    fwrite(gray_palette, sizeof(unsigned char)*GRAY_SCALE_PALETTE, 1, ftBmp_C);
    
    
    for(int i=0; i<height-2; i++){
        
        for(int j=0; j<width-2; j++){
            unsigned char buf_matrix[3][3]={0,};
            
            int index1, index2, index3, target, result;
            
            index1 = i*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index2 = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index3 = (i+2)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            target = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + (j+1)*GRAY_PIXEL_SIZE;
            
            memcpy(&buf_matrix[0][0], &image[index1], 3);
            memcpy(&buf_matrix[1][0], &image[index2], 3);
            memcpy(&buf_matrix[2][0], &image[index3], 3);
            
            result = matrix_product(buf_matrix, 3, 'C');
            filter_image[target] = (unsigned char)result;
            
            
        }
        
        
    }
    fwrite(filter_image, size, 1, ftBmp_C);


    ftBmp_D = fopen(fileNameD, "w");    // 결과 출력용 텍스트 파일 열기
    if (ftBmp_D == NULL)    // 파일 열기에 실패하면
    {
        free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제
        return 0;         // 프로그램 종료
    }
    
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, ftBmp_D);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, ftBmp_D);
    fwrite(gray_palette, sizeof(unsigned char)*GRAY_SCALE_PALETTE, 1, ftBmp_D);
    
    
    for(int i=0; i<height-2; i++){
        
        for(int j=0; j<width-2; j++){
            unsigned char buf_matrix[3][3]={0,};
            
            int index1, index2, index3, target, result;
            
            index1 = i*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index2 = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index3 = (i+2)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            target = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + (j+1)*GRAY_PIXEL_SIZE;
            
            memcpy(&buf_matrix[0][0], &image[index1], 3);
            memcpy(&buf_matrix[1][0], &image[index2], 3);
            memcpy(&buf_matrix[2][0], &image[index3], 3);
            
            result = matrix_product(buf_matrix, 3, 'D');
            filter_image[target] = (unsigned char)result;
            
            
        }
        
        
    }
    fwrite(filter_image, size, 1, ftBmp_D);


    ftBmp_E = fopen(fileNameE, "w");    // 결과 출력용 텍스트 파일 열기
    if (ftBmp_E == NULL)    // 파일 열기에 실패하면
    {
        free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제
        return 0;         // 프로그램 종료
    }
    
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, ftBmp_E);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, ftBmp_E);
    fwrite(gray_palette, sizeof(unsigned char)*GRAY_SCALE_PALETTE, 1, ftBmp_E);
    
    
    for(int i=0; i<height-2; i++){
        
        for(int j=0; j<width-2; j++){
            unsigned char buf_matrix[3][3]={0,};
            
            int index1, index2, index3, target, result;
            
            index1 = i*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index2 = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index3 = (i+2)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            target = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + (j+1)*GRAY_PIXEL_SIZE;
            
            memcpy(&buf_matrix[0][0], &image[index1], 3);
            memcpy(&buf_matrix[1][0], &image[index2], 3);
            memcpy(&buf_matrix[2][0], &image[index3], 3);
            
            result = matrix_product(buf_matrix, 3, 'E');
            filter_image[target] = (unsigned char)result;
            
            
        }
        
        
    }
    fwrite(filter_image, size, 1, ftBmp_E);


    ftBmp_F = fopen(fileNameF, "w");    // 결과 출력용 텍스트 파일 열기
    if (ftBmp_F == NULL)    // 파일 열기에 실패하면
    {
        free(image);      // 픽셀 데이터를 저장한 동적 메모리 해제
        return 0;         // 프로그램 종료
    }
    
    
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, ftBmp_F);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, ftBmp_F);
    fwrite(gray_palette, sizeof(unsigned char)*GRAY_SCALE_PALETTE, 1, ftBmp_F);
    
    
    for(int i=0; i<height-2; i++){
        
        for(int j=0; j<width-2; j++){
            unsigned char buf_matrix[3][3]={0,};
            
            int index1, index2, index3, target, result;
            
            index1 = i*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index2 = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            index3 = (i+2)*(width*GRAY_PIXEL_SIZE+padding) + j*GRAY_PIXEL_SIZE;
            target = (i+1)*(width*GRAY_PIXEL_SIZE+padding) + (j+1)*GRAY_PIXEL_SIZE;
            
            memcpy(&buf_matrix[0][0], &image[index1], 3);
            memcpy(&buf_matrix[1][0], &image[index2], 3);
            memcpy(&buf_matrix[2][0], &image[index3], 3);
            
            result = matrix_product(buf_matrix, 3, 'F');
            filter_image[target] = (unsigned char)result;
            
            
        }
        
        
    }
    fwrite(filter_image, size, 1, ftBmp_F);
    

    fclose(ftBmp_A);
    fclose(ftBmp_B);
    fclose(ftBmp_C);
    fclose(ftBmp_D);
    fclose(ftBmp_E);
    fclose(ftBmp_F);
    
    free(image);
    
    
    return 0;
}
