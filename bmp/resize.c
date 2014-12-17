/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Resizes a BMP image.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cs50.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    // check if it's anything but an integer
    int a = 0;
    while (argv[1][a] != '\0')
    {
        if (isdigit(argv[1][a]) == 0)
        {
            printf("n has to be a number!\n");
            return 6;
        }
        else
        {
            a++;
        }
    }
    
    // remember n & assert n must be a positive integer less than or equal to a 100
    int n = atoi(argv[1]);
    
    if (n > 100 || n <= 0)
    {
        printf("n must be a positive integer less than or equal to 100\n");
        return 5;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w+");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER then copy it to the resized bf
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    BITMAPFILEHEADER resizedbf = bf;

    // read infile's BITMAPINFOHEADER and copy it to the resized bi
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    BITMAPINFOHEADER resizedbi = bi;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine padding for infile scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // update file and info header of the resized image
    resizedbi.biWidth = bi.biWidth * n;
    resizedbi.biHeight = bi.biHeight * n;
    
    // determine padding for resized image scanlines
    int resizedpadding =  ((4 - (resizedbi.biWidth * sizeof(RGBTRIPLE))) % 4) % 4;
    
    // update Size and ImageSize of the resized image
    resizedbf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (resizedbi.biWidth * abs(resizedbi.biHeight) * 3) + (abs(resizedbi.biHeight) * resizedpadding);
    resizedbi.biSizeImage = resizedbi.biWidth * abs(resizedbi.biHeight) * 3 + (abs(resizedbi.biHeight) * resizedpadding);

    // write updated outfile's BITMAPFILEHEADER
    fwrite(&resizedbf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write updated outfile's BITMAPINFOHEADER
    fwrite(&resizedbi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // create an array of size biWidth to build vertically 
    RGBTRIPLE vrow[bi.biWidth];

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in infile's scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // store the pixel in an array to copy to a vertical row
            vrow[j] = triple;
            
            // write each pixel n times
            for (int k = 0; k < n; k++)
            {
                // write RGB triple to outfile
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int i = 0; i < resizedpadding; i++)
        {
            fputc(0x00, outptr);
        }
        
        // iterate over pixels outfile's new vertical scanline if n > 1
        if (n > 1)
        {
            // create n-1 columns since first column was made before
            for (int i = 0; i < n-1; i++)
            {
                // begin to write across the ith scanline which is the size of the original
                for (int j = 0; j < bi.biWidth; j++)
                {
                    // write each pixel n times
                    for (int k = 0; k < n; k++)
                    {
                        // read RGB triple from array and store it in outfile
                        fwrite(&vrow[j], sizeof(vrow[j]), 1, outptr);
                    }
                }
                
                // skip over padding, if any
                fseek(outptr, resizedpadding, SEEK_CUR);

                // then add it back (to demonstrate how)
                for (int j = 0; j < resizedpadding; j++)
                {
                    fputc(0x00, outptr);
                }
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
