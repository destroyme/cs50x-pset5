/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
 
#define _GNU_SOURCE // used for asprint()

#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <stdint.h>

int main (int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 1)
    {
        printf("Usage: ./recover\n");
        return 1;
    }
    
    // open the raw data file and create a new jpg pointer
    FILE* infile = fopen("card.raw", "r");
    FILE* jpgfileptr = fopen("000.jpg", "w");
   
    // checks on files
    if (infile == NULL || jpgfileptr == NULL)
    {
        printf("Could not open or write the file.\n");
        return 2;
    }
    
    // keep a counter for the amount of jpgs created and keep track of the name
    int jpgCounter = 0;
    char* jpgName;
    asprintf(&jpgName, "%0.3i.jpg", jpgCounter);
    
    // store the signatures in a 4 byte sized array
        // 0xff 0xd8 0xff 0xe0 (signature A)
        uint8_t sigA[4] = {0xff, 0xd8, 0xff, 0xe0}; 
        
        // 0xff 0xd8 0xff 0xe1 (signature B)
        uint8_t sigB[4] = {0xff, 0xd8, 0xff, 0xe1};
        
    // intialize a buffer with a size of 512 bytes, each the size of a byte
    uint8_t buffer[512];
        
    /*********************
    ****CHECK THE CARD****
    *********************/
    
    // loop reading through the raw data file
    while (fgetc(infile) != EOF)
    {
        fseek(infile, -1, SEEK_CUR);
        // store the data in the buffer
        fread(&buffer, sizeof(buffer), 1, infile);
        
        // if the signature is the same and it's the first jpg
        if  ((buffer[0] == sigA[0] && buffer[1] == sigA[1] && 
              buffer[2] == sigA[2] && (buffer[3] == sigA[3] || buffer[3] == sigB[3]))
              && jpgCounter == 0)
        {
                // write the whole buffer data into the ###.jpg
                fwrite(&buffer, sizeof(buffer), 1, jpgfileptr);
                // increase the jpg counter
                jpgCounter++;
        }
            
        // if there was another jpg open and you encountered another signature
        else if ((buffer[0] == sigA[0] && buffer[1] == sigA[1] && 
                  buffer[2] == sigA[2] && (buffer[3] == sigA[3] || buffer[3] == sigB[3]))
                  && jpgCounter > 0)
        {
                // close the previous jpg
                fclose(jpgfileptr);
                // increase the jpg counter
                jpgCounter++;
                // store the current jpg name
                asprintf(&jpgName, "%0.3i.jpg", jpgCounter - 1);
                // create a new jpg
                jpgfileptr = fopen(jpgName, "w");
                // write the whole buffer data into the ###.jpg
                fwrite(&buffer, sizeof(buffer), 1, jpgfileptr);
        }
        
        // if you are currently writing to a jpg
        else if (jpgCounter > 0)
        {
            // write the whole buffer data into the ###.jpg
            fwrite(&buffer, sizeof(buffer), 1, jpgfileptr);
        }
          
    }
    
    // close the data & jpg files
    fclose(infile);
    fclose(jpgfileptr);
    
    return 0;
}

