/*png_create_write_struct()：创建 PNG 写入结构。
png_create_info_struct()：创建 PNG 信息结构。
png_set_IHDR()：设置 PNG 文件的头部信息。
png_write_png()：写入 PNG 数据到文件。
png_destroy_write_struct()：销毁 PNG 写入结构。*/
#include <png.h>
#include <stdio.h>
#include <windows.h>
//#include <winsock2.h>
#include "screenshot.h"

//void write_data_to_socket(png_structp png, unsigned char *data, size_t length);

int savepng(BYTE *buffer,int ScreenWidth,int ScreenHeight,SOCKET socket_fd)
{
    png_structp PngImage;
    png_infop PngInfo;

    /*png_create_write_struct() 是 libpng 库中用于创建一个 PNG 写入结构的函数。
    * 这个结构包含所有写入 PNG 文件所需的信息，并控制写操作的行为。
    * png_structp png_create_write_struct(const char *png_ver, 
    * png_error_ptr error_fn, png_error_ptr warn_fn, void *error_ptr);
    * 返回值是png_struct结构体的指针，即png_structp类型
    * 1.png的版本号，通常是PNG_LIBPNG_VER_STRING，是个常量，标识当前库的版本号
    * 2.自定义的错误处理函数，可以为null
    * 3.自定义的警告处理函数，可以为null
    * 4.一个指向用户数据的指针，它会作为最后一个参数传递给错误或警告函数，可以为null
    * 成功返回png_structp类型指针，标识png写入结构，失败返回null
    * */
    PngImage = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    if(PngImage == NULL)
    {
        printf("png create write struct failed!\n");
    }
    else
    {
        printf("png create write struct successfully!\n");
    }

    /*png_create_info_struct() 是 libpng 库中的一个函数，用于创建 PNG 图像的描述信息结构。
    * 该结构用于存储图像的基本信息，如宽度、高度、颜色类型、位深度等。
    * png_infop png_create_info_struct(png_structp png);
    * 返回的类型是png_info结构体的指针，即png_infop
    * 1.指向一个png_struct的指针，表示写入的结构，通过png_create_write_struct创建
    * 成功返回指针，失败返回null
    */
    PngInfo = png_create_info_struct(PngImage);
    if(PngInfo == NULL)
    {
        printf("png create info failed!\n");
    }
    else
    {
        printf("pn create info successfully!\n");
    }

    png_set_write_fn(PngImage,&socket_fd,write_data_to_socket,NULL);

    /*png_set_IHDR 是 libpng 库中用于设置 PNG 文件头信息的一个函数。它定义了 PNG 
    * 图像的基本属性，比如宽度、高度、位深度、颜色类型等。
    * void png_set_IHDR(
    * 1.png_structp png_ptr,png_structp类型，指向创建的PNG写结构体,png_create_write_struct生成
    * 2.png_infop info_ptr,png_infop 类型，指向创建的 PNG 信息结构体。
    * 3.png_uint_32 width,图像的宽度，以像素为单位（png_uint_32）。
    * 4.png_uint_32 height,图像的高度，以像素为单位（png_uint_32）。
    * 5.int bit_depth,图像的位深度，常用值 8和16
    * 6.int color_type,图像的颜色类型，常用值：
    * --PNG_COLOR_TYPE_GRAY：灰度图像。
    * --PNG_COLOR_TYPE_RGB：RGB 图像。
    * --PNG_COLOR_TYPE_RGBA：RGBA 图像（支持透明度）。
    * --PNG_COLOR_TYPE_PALETTE：使用调色板的图像。
    * 7.int interlace_type,交错类型:
    * --PNG_INTERLACE_NONE无交错(常用)
    * --PNG_INTERLACE_ADAM7：Adam7 交错（渐进显示）。
    * 8.int compression_type,压缩方法（通常设置为默认值 PNG_COMPRESSION_TYPE_BASE）
    * 9.int filter_type滤波方法（通常设置为默认值 PNG_FILTER_TYPE_BASE）。
    * */
    png_set_IHDR(PngImage,PngInfo,ScreenWidth,ScreenHeight,8,PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);

    //为存储图像的每一行指针分配内存，每个数组元素指向图像的一行数据
    png_bytep *row_pointers = (png_bytep*)malloc(ScreenHeight * sizeof(png_bytep));
    if(!row_pointers)
    {
        printf("set png memory failed!\n");
        png_destroy_write_struct(&PngImage,&PngInfo);
        return -1;
    }
    else
    {
        printf("set png memory successfully!\n");
    }

    /*确定每一行的起始地址，因为bmp是连续的数据，所以需要确定每一行*/
    for(int y=0;y<ScreenHeight;++y)//用高度区分
    {
        /*buff是像素数据，是连续的，y *ScreenWidth * 3是计算y行的起始地址，*3是因为用rgb格式
        * y*yScreenWidth 是每行的宽度*/
        png_bytep row = buffer + y *ScreenWidth * 3;

        //buffer是BGR格式，交换R和B通道，把每个像素的rgb字节进行交换，就是一个像素一个像素的操作
        for(int x=0;x<ScreenWidth;++x)//开始计算第一行的所有像素
        {
            unsigned char temp = row[x*3];
            //定义临时变量，保存R值，一个像素是3个字节，所以*3，
            //x是每行的一个像素，所以当x=0是是第一个像素，*3是第一个值r
            row[x*3] = row[x*3+2];
            //把B值赋给R的原始地址，*3是r值，+2就是这个像素的第三个数据，即b值，+1是第二个就是g值
            row[x*3+2] = temp;//把临时变量里的R值赋给B值，完成交换
        }

        /*把交换好的行数据保存刀行指针的成员中，这里用高度-y是因为数据是反的，需要把最上层的换到最下层*/
        row_pointers[ScreenHeight-1-y] = row;
    }

    /*把像素数据的行设置到写入结构体中，它会告诉libpng每行数据的地址
    * void png_set_rows(png_structp png, png_infop info, png_bytep *row_pointers);
    * 1.写入结构体指针 2.信息结构体指针 3.行指针数组*/
    png_set_rows(PngImage,PngInfo,row_pointers);

    /*它会将 PNG 的所有部分（IHDR、数据行、IEND）写入文件。使用时需要准备好：
    * PNG 写结构（png_structp)、PNG 信息结构（png_infop）、图像数据。
    * void png_write_png(
    * 1.png_structp png_ptr,已创建的写入结构体
    * 2.png_infop info_ptr,已初始化的信息结构体
    * 3.int transforms,预处理选项，通常PNG_TRANSFORM_IDENTITY 表示无特殊变换
    * 4.void *params 通常为null，除非需要特定处理
    */
    png_write_png(PngImage,PngInfo,PNG_TRANSFORM_IDENTITY,NULL);

    free(row_pointers);
    png_destroy_write_struct(&PngImage,&PngInfo);

}

void write_data_to_socket(png_structp png, unsigned char *data, size_t length)
{
    int socket_fd = *((int*)png_get_io_ptr(png));//从png里面提取socket套接字
    //printf("sending chunk of size:%zu\n",length);

    ssize_t sent = send(socket_fd,data,length,0);
    if(sent == -1)
    {
        perror("send error");
    }

    //printf("chunk sent successfully,size:%zu\n",sent);
}
