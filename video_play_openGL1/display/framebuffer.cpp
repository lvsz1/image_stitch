#include "framebuffer.h"

int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;


void framebuffer_init(const char* device)
{
    // Open the file for reading and writing
    fbfd = open(device, O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
}


void framebuffer_display(const unsigned char *frame, int width, int height, int startx, int starty)
{
    int x, y, location;
//    vinfo.xoffset = startx;
//    vinfo.yoffset = starty;

    for ( y = 0; y < height; y++ )
        for ( x = 0; x < width; x++ )
        {
            location = (x+startx) * (vinfo.bits_per_pixel/8) +
                     (y+starty) * finfo.line_length;

            if ( vinfo.bits_per_pixel == 32 ) {
                //rgb32 bgra
                *(fbp + location ) 		= *frame;frame++; // Some blue
                *(fbp + location + 1)	= *frame;frame++; // A little green
                *(fbp + location + 2) 	= *frame;frame++; //A lot of red//frame[480*y+x+2];
                *(fbp + location + 3) = 0; // No transparency
            }
            else { //assume 16bpp
                unsigned short int b = (unsigned short int)*frame / 8;frame++;
                unsigned short int g = (unsigned short int)*frame / 4;frame++;  // A little green
                unsigned short int r = (unsigned short int)*frame / 8;frame++;  // A lot of red
                unsigned short int t = r << 11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }

        }
}

void framebuffer_close()
{
    munmap(fbp, screensize);
    close(fbfd);
}
