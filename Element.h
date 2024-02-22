#pragma once
#include <stdint.h>
#include <string>

#include "Types.h"

class Element;

class ModelInfo {
	public:
		ModelInfo();
		~ModelInfo();

		void set_triangle_count(uint32_t new_triangle_count);
		uint32_t get_triangle_count();

		void prepare_control_vertices(uint32_t new_vertices_count);
		Vector4d *get_control_vertices();
		uint32_t get_control_verticies_count();

		void prepare_indicies(uint32_t new_indicies_count);
		int32_t *get_indicies();
		uint32_t get_indicies_count();

		void prepare_vertices(uint32_t new_vertices_count);
		Vector4d *get_vertices();
		uint32_t get_vertices_count();

		void prepare_normals(uint32_t new_normals_count);
		Vector4d *get_normals();
		uint32_t get_normals_count();

	private:
		// Total amount of triangles. 
		uint32_t triangle_count = 0;

		// Control Verticies
		Vector4d *control_vertices = nullptr;
		uint32_t control_vertices_count = 0;

		// Control Vertex Indices, This array maps from the
		// redundant cvertices array into the unique vertices array.
		// (control_vertices->vertices)
		int32_t *indices = nullptr;
		uint32_t indicies_count = 0;

		// Verticies
		Vector4d *vertices = nullptr;
		uint32_t vertices_count = 0;

		// Normals
		Vector4d *normals = nullptr;
		uint32_t normals_count = 0;

		// UVs
		int32_t u_repeat = 0;
		int32_t v_repeat = 0;
		float *u_coords = nullptr;
		float *v_coords = nullptr;
		float *u_scale = nullptr;
		float *v_scale = nullptr;
		float *u_offset = nullptr;
		float *v_offset = nullptr;
		char **texture_names = nullptr;
};

// This class is a replacement for the root in Softimage which is Model for some reason.
// It serves basicially the same purpose though.
class Element {
	public:
		Element();
		~Element();

		void set_parent(Element *new_parent);
		Element *get_parent();

		void set_name(const char *name);
		void set_name(std::string &new_name);
		std::string &get_name();

		void set_transformation_matrix(Matrix4f &new_matrix);
		Matrix4f &get_transformation_matrix();

		void set_position(Vector3f &new_position);
		Vector3f &get_position();

		void set_rotation(Vector3f &new_rotation);
		Vector3f &get_rotation();

		void set_scale(Vector3f &new_scale);
		Vector3f &get_scale();

		void set_visibility(bool new_visibility);
		bool get_visibility();

		void prepare_children(uint32_t amount);
		void set_child(uint32_t index, Element* child);
		Element *get_child(uint32_t index);
		uint32_t get_children_amount();

	public:
		// Model Info
		ModelInfo model_info;

	private:
		// Element Parent, Above us in the hierarchy.
		Element *parent = nullptr;

		// Elements, All of the children we have. 
		// All of them are directly below us in the hierarchy.
		Element **children = nullptr;
		uint32_t children_count = 0;

		// Element Name
		std::string name;

		// Transformation Matrix (Global Space)
		Matrix4f matrix = { 0.0f };

		// Transformations (Local Space)
		Vector3f pos = { 0.0f };
		Vector3f rot = { 0.0f };
		Vector3f scale = { 0.0f };

		// Visibility (Visible by default)
		bool visibility = true;
};