#pragma once
#include <stdint.h>
#include <string>

#include "BinaryFile.h"
#include "Types.h"

class Texture {
    public:
        Texture();
        Texture(const char *path);
        ~Texture();
        
        void set_filepath(std::string path);
        std::string &get_filepath();
        
        void set_u_scale(float scale);
        float get_u_scale();
        
        void set_v_scale(float scale);
        float get_v_scale();
        
        void set_u_offset(float offset);
        float get_u_offset();
        
        void set_v_offset(float offset);
        float get_v_offset();
        
        void set_u_repeat(int32_t repeat);
        int32_t get_u_repeat();
        
        void set_v_repeat(int32_t repeat);
        int32_t get_v_repeat();
        
        void set_transparency(float value);
        float get_transparency();
        
        void set_uv_swap(bool swap);
        bool get_uv_swap();
        
        void set_uv_wrap(bool wrap);
        bool get_uv_wrap();
        
        void write(BinaryFile &file);
        
    private:
        std::string filepath;
        
        float u_scale = 0.0f;
        float v_scale = 0.0f;
        
		float u_offset = 0.0f;
		float v_offset = 0.0f;
        
        int32_t u_repeat = 1;
        int32_t v_repeat = 1;
        
        // Controls the transparency effect of the Effect Value Alpha Channel and RGB Intensity options. Values range from -1 to 1.
        float transparency = 1.0f;
        
        // If true, the U and V directions of the texture coordinates are exchanged.
        bool uv_swap = false;
        
        // Controls whether a texture is wrapped around the UV seams (where the UV coordinates wrap from 1 back to 0).
        bool uv_wrap = false;
};