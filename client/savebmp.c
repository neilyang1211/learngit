#include <stdio.h>
#include "screenshot.h"
#include <windows.h>

int savebmp(BYTE *buffer,int ScreenWidth,int Screenheight)
{
    //定义bmp文件头
    BITMAPFILEHEADER fileHeader = {0};
    fileHeader.bfType = 0x4D42;//bmp必须这么些，是bm的十六进制
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//前置部分大小
    fileHeader.bfSize = fileHeader.bfOffBits + ScreenWidth * Screenheight * 3;//文件大小

    //定义bmp信息头
    BITMAPINFOHEADER infoHeader = {0};
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);//信息头大小
    infoHeader.biWidth = ScreenWidth;
    infoHeader.biHeight = Screenheight;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = ScreenWidth * Screenheight * 3;//图像大小

    //打开空白文件
    FILE *file = fopen("F:\\c\\remote\\client\\screenshot.bmp","wb");
    if(!file)
    {
        printf("open file faile!\n");
        return -1;
    }
    else
    {
        printf("open file successfully!\n");
    }

    //把文件头和信息头写入空白文件
    fwrite(&fileHeader,sizeof(BITMAPFILEHEADER),1,file);
    fwrite(&infoHeader,sizeof(BITMAPINFOHEADER),1,file);

    //把像素数据也写入
    fwrite(buffer,1,ScreenWidth * Screenheight * 3,file);

    //写入完成，关闭文件
    fclose(file);

    return 1;
}