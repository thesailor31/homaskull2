#include "lodepng.c"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define eps 40


typedef struct point{
    int x, y;
} pt;


char* load_png_file(const char *filename, int *w, int *h) {
    unsigned char *image = NULL;
    int error = lodepng_decode32_file(&image, w, h, filename);
    if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }

    return (image);
}

void floodFill(unsigned char* image, int x, int y, int newColor1, int newColor2, int newColor3, int oldColor, int w, int h) {
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};
    pt* ar = malloc(w * h * 4 * sizeof(pt));
    long top = 0;
    ar[top++] = (pt){x, y};
    while(top > 0) {
        pt p = ar[--top];
        if(p.x < 0 || p.x >= w || p.y < 0 || p.y >= h)
            continue;
        int resultIndex = (p.y * w + p.x) * 4;
        if(image[resultIndex] > oldColor)
            continue;
        image[resultIndex] = newColor1;
        image[resultIndex + 1] = newColor2;
        image[resultIndex + 2] = newColor3;
        for(int i = 0; i < 4; i++) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];
            int nIndex = (ny * w + nx) * 4;
            if(nx > 0 && nx < w && ny > 0 && ny < h && image[nIndex] <= oldColor) {
                ar[top++] = (pt){nx, ny};
            }
        }
    }
    free(ar);
}

void ScharrFilter(unsigned char *image, int w, int h) {
    int x, y, dy, dx, i;
    int gx[3][3] = {{9, 0, -9},
                    {15, 0, -15},
                    {9, 0, -9}};
    int gy[3][3] = {{9,  15,  9},
                    {0,  0,  0},
                    {-9, -15, -9}};
    unsigned char *result = malloc(w * h * 4 * sizeof(unsigned char));

    for (y = 1; y < h - 1; y++) {
        for (x = 1; x < w - 1; x++) {
            int sumX = 0, sumY = 0;
            for (dy = -1; dy <= 1; dy++) {
                for (dx = -1; dx <= 1; dx++) {
                    int index = ((y + dy) * w + (x + dx)) * 4;
                    int gray = (image[index] + image[index + 1] + image[index + 2]) / 10;
                    sumX += gx[dy + 1][dx + 1] * gray;
                    sumY += gy[dy + 1][dx + 1] * gray;
                }
            }
            int mg = sqrt(sumX * sumX + sumY * sumY);
            if (mg > 255) mg = 255;
            if (mg < 0) mg = 0;

            int resultIndex = (y * w + x) * 4;
            result[resultIndex] = (unsigned char) mg;
            result[resultIndex + 1] = (unsigned char) mg;
            result[resultIndex + 2] = (unsigned char) mg;
            result[resultIndex + 3] = image[resultIndex + 3];
        }
    }

    for (i = 0; i < w * h * 4; i++) {
        image[i] = result[i];
    }
    free(result);
}



void colorComponents(unsigned char *image, int w, int h) {
    int x, y;
    for (y = 1; y < h - 1; y++) {
        for (x = 1; x < w - 1; x++) {
            if (image[4 * (y * w + x)] < eps) {
                floodFill(image, x, y, rand() % (255 - eps * 2) + eps * 2, \
                rand() % (255 - eps * 2) + eps * 3, \
                rand() % (255 - eps * 3) + eps * 5, eps, w, h);

            }
        }
    }
    char *output_filename = "end.png";
    lodepng_encode32_file(output_filename, image, w, h);
}


int main() {
    int w = 0, h = 0, x, y, dy, dx;
    char *filename = "pic.png";
    char *image = load_png_file(filename, &w, &h);
    ScharrFilter(image, w, h);
    colorComponents(image, w, h);
    free(image);
    return 0;
}
