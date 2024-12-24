#pragma once
#include <stdint.h>
#include <string>

#include "BinaryFile.h"
#include "Texture.h"
#include "Types.h"

class Material {
	public:
		enum ShadingModel : uint8_t {
			CONSTANT = 1,
			LAMBERT = 3,
			PHONG = 4,
			BLINN = 5,
			SHADOW = 6,
			VERTEX_COLOR = 7,
		};

		enum ComponentSource : uint8_t {
			MATERIAL = 1,
			VERTICIES = 2,
			BOTH = 3,
		};
        
        Material();
		Material(const char *material_name);
		~Material();
        
		Material(const Material &other);

        void set_name(std::string new_name);
		std::string get_name() const;

		void set_shade_model(ShadingModel model);
		ShadingModel get_shade_model() const;

		void set_diffuse_source(ComponentSource source);
		ComponentSource get_diffuse_source() const;

		void set_transparency_source(ComponentSource source);
		ComponentSource get_transparency_source() const;

		void set_static_blur(bool blur);
		bool has_static_blur() const;

		void set_ambieance(Vector3f &amb);
		Vector3f get_ambieance() const;

		void set_diffuse(Vector3f &diff);
		Vector3f get_diffuse() const;

		void set_specular(Vector3f &spec);
		Vector3f get_specular() const;

		void set_specular_decay(float decay);
		float get_specular_decay() const;

		void set_reflection(float value);
		float get_reflection() const;

		void set_refractive_index(float index);
		float get_refractive_index() const;

		void set_transparency(float value);
		float get_transparency() const;

		void set_blur_decay(float decay);
		float get_blur_decay() const;

		void set_blur_width(float width);
		float get_blur_width() const;

		void set_texture_count(uint32_t count);
		uint32_t get_texture_count() const;
        Texture *prepare_textures();
        Texture *get_textures() const;
        
        void write(BinaryFile &file);

	private:
		std::string name;

		// Shading Model of the material.
		// 
		// Notes:
		//   SAA_materialGetShadingModel returns SAA_SHM_VERTEXCOLOR if the material diffuse and / or transparency is not from material. 
		//   This is to ensure compatibility with existing plug-ins that rely on this functionality.
		// 
		//   SAA_materialSetShadingModel can accept SAA_SHM_VERTEXCOLOR as the given shading model but it will emulate the old behavior 
		//   by affecting the material as such:
		//       set both diffuse and transparency sources to Vertex
		//       set shading model to Constant
		ShadingModel shade_model = CONSTANT;

		ComponentSource diffuse_source = MATERIAL;

		ComponentSource transparency_source = MATERIAL;

		// Enables or disables static blur.
		bool static_blur = false;

		// RGB color value for surface areas shaded with ambient light on Lambert-, Blinn-, or Phong-shaded models.
		Vector3f ambieance = { 0.0f };

		// RGB color value for surface areas that are diffusely illuminated.
		Vector3f diffuse = { 0.0f };

		// RGB color value for the specular highlight on a Phong- or Blinn-shaded model.
		Vector3f specular = { 0.0f };

		// Controls the spread of the specular decay over the model surface (for Phong and Blinn-shaded models).
		// Values range from 0 to 300.
		float specular_decay = 0.0f;

		// A value from 0 to 1: a value of 1 represents complete reflectivity.
		float reflection = 0.0f;

		// Controls the bending of light rays through a transparent material.
		float refractive_index = 0.0f;

		// A value from 0 to 1: a value of 1 represents complete transparency.
		float transparency = 0.0f;

		float blur_decay = 0.0f;
		float blur_width = 0.0f;

        // Textures
		uint32_t texture_count = 0;
        Texture *textures = nullptr;

	// Temporaries
	private:
		// This only exists for sanity purposes in soft2scene. It's not needed otherwise.
		bool initialized = false;
};