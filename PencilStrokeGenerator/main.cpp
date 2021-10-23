//
//  main.cpp
//  PencilStrokeGenerator
//
//  Created by 莫少华 on 2021/10/23.
//

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//add stroke color b with ratio alpha to background color b: b = b + a * alpha;
unsigned char add(unsigned char a, unsigned char b, float alpha)
{
    float ca = 1.0f - (float)a / (float)0xFF;
    float cb = 1.0f - (float)b / (float)0xFF;
    
    cb *= alpha;
    
    uint8_t cc = (1.0f - fmin(1.0f, ca + cb)) * 0xFF;
    return (unsigned char)cc;
}

int main(int argc, const char * argv[]) {
    char const* input = "/Users/moshaohua/Documents/others/input.png";
    char const* output = "/Users/moshaohua/Documents/others/output.png";
    int width, height, nrChannels;
    unsigned char* data = stbi_load(input, &width, &height, &nrChannels, 0);
    if(!data)
    {
        printf("data is nil.\n");
        return -1;
    }
    
    printf("load image with channels:%d\n", nrChannels);
    int max_index = width * height * nrChannels;
    unsigned char* image = new unsigned char(max_index);
    memset(image, 0xFF, max_index);
    
    // the (valid)pixel height of input stroke png
    int example_size = 16;
    
    // the strength of stroke that add to background image
    float blend_alpha = 0.5f;
    
    //control the density of strokes
    int step_range = 4;
    int step;
    for(int i = 0; i <= height + 1;)
    {
        step = 1 + step_range + rand() % step_range - step_range * 0.5f;
        int x_offset = rand() % width;
        for(int j=0;j<example_size;j++)
            for(int x=0;x<width;x++)
            {
                
                int y = i + j - example_size * 0.5f;
                if(y > -1 && y < height)
                {
                    int output_index = (y * width + x) * nrChannels;
                    int input_index = (j * width + (x + x_offset) % width) * nrChannels;
                    
                    //check index
                    if(input_index >= max_index || output_index >= max_index)
                        printf("id:%d, od:%d\n", input_index, output_index);
                    
                    for(int k = 0; k < nrChannels; k++)
                    {
                        //blend
                        unsigned char input_color = data[input_index + k];
                        unsigned char output_color = image[output_index + k];
                        
                        image[output_index + k] = add(output_color, input_color, blend_alpha);
                    }
                }
            }
        i += step;
    }
    
    int ret = stbi_write_png(output, width, height, nrChannels, image, width * nrChannels);
    if(!ret)
        printf("write png error code:%d\n", ret);

    printf("done.\n");
    return 0;
}
