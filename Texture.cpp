#include "Texture.h"

Texture::Texture() : filepath("") {
    
}

Texture::Texture(const char *path) : filepath(path) {
    
}

Texture::~Texture() {
    
}

void Texture::set_filepath(std::string path) {
    filepath = path;
}

std::string &Texture::get_filepath() {
    return filepath;
}

void Texture::set_u_scale(float scale) {
    u_scale = scale;
}

float Texture::get_u_scale() {
    return u_scale;
}

void Texture::set_v_scale(float scale) {
    v_scale = scale;
}

float Texture::get_v_scale() {
    return v_scale;
}

void Texture::set_u_offset(float offset) {
    u_offset = offset;
}

float Texture::get_u_offset() {
    return u_offset;
}

void Texture::set_v_offset(float offset) {
    v_offset = offset;
}

float Texture::get_v_offset() {
    return v_offset;
}

void Texture::set_u_repeat(int32_t repeat) {
    u_repeat = repeat;
}

int32_t Texture::get_u_repeat() {
    return u_repeat;
}

void Texture::set_v_repeat(int32_t repeat) {
    v_repeat = repeat;
}

int32_t Texture::get_v_repeat() {
    return v_repeat;
}

void Texture::set_transparency(float value) {
    transparency = value;
}

float Texture::get_transparency() {
    return transparency;
}

void Texture::set_uv_swap(bool swap) {
    uv_swap = swap;
}

bool Texture::get_uv_swap() {
    return uv_swap;
}

void Texture::set_uv_wrap(bool wrap) {
    uv_wrap = wrap;
}

bool Texture::get_uv_wrap() {
    return uv_wrap;
}

void Texture::write(BinaryFile &file) {
	file.write_uint64(filepath.size());
	file.write(filepath);
    file.write(uv_swap);
    file.write(uv_wrap);
    file.write(u_scale);
    file.write(v_scale);
    file.write(u_offset);
    file.write(v_offset);
    file.write(u_repeat);
    file.write(v_repeat);
    file.write(transparency);
}