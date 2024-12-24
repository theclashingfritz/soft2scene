#pragma once
#include <stdint.h>
#include <string>

#include "BinaryFile.h"
#include "Types.h"

class Element;

class MeshInfo {
	public:
		MeshInfo();
		~MeshInfo();

		void set_triangle_count(uint32_t new_triangle_count);
		uint32_t get_triangle_count();
        
        void set_material_id(uint32_t id);
        uint32_t get_material_id();

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

		void prepare_uv_coords(uint32_t new_uv_coords_count);

		float *get_u_coords();
		float *get_v_coords();

		void write(BinaryFile &file);

	private:
		// Total amount of triangles. 
		uint32_t triangle_count = 0;
        
        // Index of our Material in the global array.
        uint32_t material_id = 0;

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

        // UV Coords
		float *u_coords = nullptr;
		float *v_coords = nullptr;
		uint32_t uv_coords_count = 0;
};

class JointLinkListNode {
	public:
		JointLinkListNode() {};
		~JointLinkListNode();

		void prepare_next();
		Element **get_next();

		void set_next_count(SIZE count);
		SIZE get_next_count();

		void set_previous(Element *element);
		Element *get_previous();

		void prepare_next_indicies();
		int64_t *get_next_indicies();

		void set_previous_index(int64_t index);
		int64_t get_previous_index();

		void write(BinaryFile &file);

	private:
		Element **next = nullptr;
		SIZE next_count = 0;

		Element *previous = nullptr;

		// Read/Write
		int64_t *next_indicies = nullptr;
		int64_t previous_index = -1;
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

		void set_as_joint(bool is_joint);
		bool is_joint();

		void prepare_joint_linklist_node();
		JointLinkListNode *get_joint_linklist_node();

		void prepare_children(uint32_t amount);
		void set_child(uint32_t index, Element* child);
		Element *get_child(uint32_t index);
		uint32_t get_children_amount();

		// Model Info
		MeshInfo *mesh_info = nullptr;

		// Write/Read
		enum InfoTypes {
			Mesh = 1 << 0,
		};

		void set_flags(uint64_t nflags);
		uint64_t get_flags();

		void set_array_position(int64_t index);
		int64_t get_array_position();

		void set_parent_position(int64_t index);
		int64_t get_parent_position();

		void prepare_children_indicies();
		int64_t *get_children_indicies();

		void write(BinaryFile &file);

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

		// If our Element is infact a joint!
		bool joint = false;

		// Our joint link list node, We only have this if we are infact a joint.
		JointLinkListNode *node = nullptr;

		// Write/Read Info
		// Used for resolving children and parents of Element array.

		uint64_t flags = 0;

		// Used to resolve position in Element array. 
		int64_t array_index = 0;

		// Position of parent in Element array.
		int64_t parent_index = 0;

		// Array of indexes to all of our children.
		int64_t *children_indexes = nullptr;
};