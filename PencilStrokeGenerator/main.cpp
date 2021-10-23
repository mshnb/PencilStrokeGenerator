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
    std::string input = "/Users/moshaohua/Documents/others/input.png";
    std::string outputpath = "/Users/moshaohua/Documents/others/";
    int width, height, nrChannels;
    unsigned char* data = stbi_load(input.c_str(), &width, &height, &nrChannels, 0);
    if(!data)
    {
        printf("data is nil.\n");
        return -1;
    }
    
    printf("load image with channels:%d\n", nrChannels);
    int max_index = width * height * nrChannels;
    unsigned char* image = new unsigned char(max_index);
    
    //more biger, generate more strokes
    int max_density_level = 6;
    memset(image, 0xFF, max_index);
    
    // the (valid)pixel height of input stroke png
    int example_size = 16;
    
    // the strength of stroke that add to background image
    float blend_alpha_list[6] = {0.5f, 0.25f, 0.3f, 0.4f, 0.35f, 0.3f};
    //control the density of strokes
    int step_range_list[6] = {4, 4, 6, 6, 8, 10};
    
    for(int density_level = 0; density_level < max_density_level; density_level++)
    {
        //control the num of new strokes
        float blend_alpha = blend_alpha_list[density_level];
        int step_range = step_range_list[density_level];

        for(int i = 0; i <= height + 1;)
        {
            int step = 1 + step_range + rand() % step_range - step_range * 0.5f;
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
        
        std::string output = outputpath + "stroke" + std::to_string(density_level) + ".png";
        stbi_write_png(output.c_str(), width, height, nrChannels, image, width * nrChannels);
    }

    stbi_image_free(data);
    delete image;
    
    printf("done.\n");
    return 0;
}
