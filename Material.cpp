#include "Material.h"


Material::Material() : name("") {

}

Material::Material(const char *material_name) : name(material_name) {

}

Material::~Material() {
    if (textures) {
        delete[] textures;
        textures = nullptr;
    }
}

Material::Material(const Material &other) {
    set_name(other.get_name());
    set_shade_model(other.get_shade_model());
    set_diffuse_source(other.get_diffuse_source());
    set_transparency_source(other.get_transparency_source());
    set_static_blur(other.has_static_blur());
    set_ambieance(other.get_ambieance());
    set_diffuse(other.get_diffuse());
    set_specular(other.get_specular());
    set_specular_decay(other.get_specular_decay());
    set_reflection(other.get_reflection());
    set_refractive_index(other.get_refractive_index());
    set_transparency(other.get_transparency());
    set_blur_decay(other.get_blur_decay());
    set_blur_width(other.get_blur_width());
    
    set_texture_count(other.get_texture_count());
    Texture *textures = prepare_textures();
    Texture *other_textures = other.get_textures();
    for (auto i = 0; i < texture_count; i++) {
        textures[i] = other_textures[i];
    }
}

void Material::set_name(std::string new_name) {
    name = new_name;
}

std::string Material::get_name() const {
	return name;
}

void Material::set_shade_model(Material::ShadingModel model) {
	shade_model = model;
}

Material::ShadingModel Material::get_shade_model() const {
	return shade_model;
}

void Material::set_diffuse_source(Material::ComponentSource source) {
	diffuse_source = source;
}

Material::ComponentSource Material::get_diffuse_source() const {
	return diffuse_source;
}

void Material::set_transparency_source(Material::ComponentSource source) {
	transparency_source = source;
}

Material::ComponentSource Material::get_transparency_source() const {
	return transparency_source;
}

void Material::set_static_blur(bool blur) {
	static_blur = blur;
}

bool Material::has_static_blur() const {
	return static_blur;
}

void Material::set_ambieance(Vector3f &amb) {
	ambieance = amb;
}

Vector3f Material::get_ambieance() const {
	return ambieance;
}

void Material::set_diffuse(Vector3f &diff) {
	diffuse = diff;
}

Vector3f Material::get_diffuse() const {
	return diffuse;
}

void Material::set_specular(Vector3f &spec) {
	specular = spec;
}

Vector3f Material::get_specular() const {
	return specular;
}

void Material::set_specular_decay(float decay) {
	specular_decay = decay;
}

float Material::get_specular_decay() const {
	return specular_decay;
}

void Material::set_reflection(float value) {
	reflection = value;
}

float Material::get_reflection() const {
	return reflection;
}

void Material::set_refractive_index(float index) {
	refractive_index = index;
}

float Material::get_refractive_index() const {
	return refractive_index;
}

void Material::set_transparency(float value) {
	transparency = value;
}

float Material::get_transparency() const {
	return transparency;
}

void Material::set_blur_decay(float decay) {
	blur_decay = decay;
}

float Material::get_blur_decay() const {
	return blur_decay;
}

void Material::set_blur_width(float width) {
	blur_width = width;
}

float Material::get_blur_width() const {
	return blur_width;
}

void Material::set_texture_count(uint32_t count) {
	texture_count = count;
}

uint32_t Material::get_texture_count() const {
	return texture_count;
}

Texture *Material::prepare_textures() {
    if (textures) {
        delete[] textures;
        textures = nullptr;
    }
    textures = new Texture[texture_count + 1];
    return textures;
}

Texture *Material::get_textures() const {
    return textures;
}

void Material::write(BinaryFile &file) {
	file.write_uint64(name.size());
	file.write(name);
    file.write(shade_model);
    file.write(diffuse_source);
    file.write(transparency_source);
    file.write(ambieance);
    file.write(diffuse);
    file.write(specular);
    file.write(specular_decay);
    file.write(reflection);
    file.write(refractive_index);
    file.write(transparency);
    file.write(blur_decay);
    file.write(blur_width);
    file.write(static_blur);
    
	file.write(texture_count);
	for (auto i = 0; i < texture_count; i++) {
		Texture &texture = textures[i];
		texture.write(file);
	}
}