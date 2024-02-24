#include "Element.h"

Element::Element() {

}

Element::~Element() {
	if (children) {
		delete[] children;
		children = nullptr;
	}
	if (children_indexes) {
		delete[] children_indexes;
		children_indexes = nullptr;
	}
	if (mesh_info) {
		delete mesh_info;
		mesh_info = nullptr;
	}
}

void Element::set_parent(Element *new_parent) {
	parent = new_parent;
}

Element *Element::get_parent() {
	return parent;
}

void Element::set_name(const char *new_name) {
	name = std::string(new_name);
}

void Element::set_name(std::string &new_name) {
	name = new_name;
}

std::string &Element::get_name() {
	return name;
}

void Element::set_transformation_matrix(Matrix4f &new_matrix) {
	// Just straight out copy the memory. No good reason not to do this.
	memcpy_s(&matrix, sizeof(Matrix4f), &new_matrix, sizeof(Matrix4f));
}

Matrix4f &Element::get_transformation_matrix() {
	return matrix;
}

void Element::set_position(Vector3f &new_position) {
	pos.x = new_position.x;
	pos.y = new_position.y;
	pos.z = new_position.z;
}

Vector3f &Element::get_position() {
	return pos;
}

void Element::set_rotation(Vector3f &new_rotation) {
	rot.x = new_rotation.x;
	rot.y = new_rotation.y;
	rot.z = new_rotation.z;
}

Vector3f &Element::get_rotation() {
	return rot;
}

void Element::set_scale(Vector3f &new_scale) {
	scale.x = new_scale.x;
	scale.y = new_scale.y;
	scale.z = new_scale.z;
}

Vector3f &Element::get_scale() {
	return scale;
}

void Element::set_visibility(bool new_visibility) {
	visibility = new_visibility;
}

bool Element::get_visibility() {
	return visibility;
}

void Element::prepare_children(uint32_t amount) {
	// If we already HAVE children. Then release the buffer.
	if (children) { 
		delete[] children;
		children = nullptr;
	}

	// Set the new amount of children.
	children_count = amount;

	// Allocate a buffer which can hold all of the pointers to our children.
	children = new Element *[children_count + 1];
	memset(children, NULL, sizeof(Element *) * children_count);
}

void Element::set_child(uint32_t index, Element *child) {
	// Don't allow a buffer overflow!
	if (index >= children_count) { return; }

	children[index] = child;
}

Element *Element::get_child(uint32_t index) {
	// Don't allow a buffer overflow!
	if (index >= children_count) { return nullptr; }

	return children[index];
}

uint32_t Element::get_children_amount() {
	return children_count;
}

void Element::set_flags(uint64_t nflags) {
	flags = nflags;
}

uint64_t Element::get_flags() {
	return flags;
}

void Element::set_array_position(size_t index) {
	array_index = index;
}

size_t Element::get_array_position() {
	return array_index;
}

void Element::set_parent_position(size_t index) {
	parent_index = index;
}

size_t Element::get_parent_position() {
	return parent_index;
}

void Element::prepare_children_indexes() {
	if (children_indexes) {
		delete[] children_indexes;
		children_indexes = nullptr;
	}

	children_indexes = new size_t[children_count + 1];
	memset(children_indexes, NULL, sizeof(size_t) * children_count);
}

size_t *Element::get_children_indexes() {
	return children_indexes;
}

void Element::write(BinaryFile &file) {
	file.write(name.size());
	file.write(name);
	file.write(flags);
	file.write(matrix);
	file.write(pos);
	file.write(rot);
	file.write(scale);
	file.write(visibility);
	file.write(array_index);
	file.write(parent_index);
	file.write(children_count);
	for (uint32_t i = 0; i < children_count; i++) {
		size_t child_index = children_indexes[i];
		file.write(child_index);
	}
	if (flags & Element::InfoTypes::Mesh) {
		assert(mesh_info != nullptr);
		mesh_info->write(file);
	}
}


// Model Info

MeshInfo::MeshInfo() {

}

MeshInfo::~MeshInfo() {
	if (control_vertices) {
		delete[] control_vertices;
		control_vertices = nullptr;
	}
	if (indices) {
		delete[] indices;
		indices = nullptr;
	}
	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
	}
	if (normals) {
		delete[] normals;
		normals = nullptr;
	}
	if (u_coords) {
		delete[] u_coords;
		u_coords = nullptr;
	}
	if (v_coords) {
		delete[] v_coords;
		v_coords = nullptr;
	}
	if (u_scales) {
		delete[] u_scales;
		u_scales = nullptr;
	}
	if (v_scales) {
		delete[] v_scales;
		v_scales = nullptr;
	}
	if (u_offsets) {
		delete[] u_offsets;
		u_offsets = nullptr;
	}
	if (v_offsets) {
		delete[] v_offsets;
		v_offsets = nullptr;
	}
	if (texture_names) {
		delete[] texture_names;
		texture_names = nullptr;
	}
}

void MeshInfo::set_triangle_count(uint32_t new_triangle_count) {
	triangle_count = new_triangle_count;
}

uint32_t MeshInfo::get_triangle_count() {
	return triangle_count;
}

void MeshInfo::prepare_control_vertices(uint32_t new_vertices_count) {
	// If we already HAVE control vertices. Then release the buffer.
	if (control_vertices) {
		delete[] control_vertices;
		control_vertices = nullptr;
	}

	control_vertices_count = new_vertices_count;

	// Allocate a buffer which can hold all of the pointers to our indices.
	control_vertices = new Vector4d[control_vertices_count + 1];
	for (uint32_t i = 0; i < control_vertices_count; i++) {
		Vector4d &control_vertex = control_vertices[i];
		control_vertex.x = 0.0;
		control_vertex.y = 0.0;
		control_vertex.z = 0.0;
		control_vertex.w = 0.0;
	}
}

Vector4d* MeshInfo::get_control_vertices() {
	return control_vertices;
}

uint32_t MeshInfo::get_control_verticies_count() {
	return control_vertices_count;
}

void MeshInfo::prepare_indicies(uint32_t new_indicies_count) {
	// If we already HAVE indicies. Then release the buffer.
	if (indices) {
		delete[] indices;
		indices = nullptr;
	}

	indicies_count = new_indicies_count;

	// Allocate a buffer which can hold all of the pointers to our indices.
	indices = new int[indicies_count + 1];
	memset(indices, 0, sizeof(int32_t) * indicies_count);
}

int32_t * MeshInfo::get_indicies() {
	return indices;
}

uint32_t MeshInfo::get_indicies_count() {
	return indicies_count;
}

void MeshInfo::prepare_vertices(uint32_t new_vertices_count) {
	// If we already HAVE vertices. Then release the buffer.
	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
	}

	vertices_count = new_vertices_count;

	// Allocate a buffer which can hold all of the pointers to our vertices.
	vertices = new Vector4d[vertices_count + 1];
	memset(vertices, 0, sizeof(Vector4d) * vertices_count);
}

Vector4d * MeshInfo::get_vertices() {
	return vertices;
}

uint32_t MeshInfo::get_vertices_count() {
	return vertices_count;
}

void MeshInfo::prepare_normals(uint32_t new_normals_count) {
	// If we already HAVE normals. Then release the buffer.
	if (normals) {
		delete[] normals;
		normals = nullptr;
	}

	normals_count = new_normals_count;

	// Allocate a buffer which can hold all of the pointers to our normals.
	normals = new Vector4d[normals_count + 1];
	memset(normals, 0, sizeof(Vector4d) * normals_count);
}

Vector4d* MeshInfo::get_normals() {
	return normals;
}

uint32_t MeshInfo::get_normals_count() {
	return normals_count;
}

void MeshInfo::prepare_uvs_and_textures(uint32_t new_texture_count, uint32_t tri_count) {
	// Free arrays if they're already allocated.
	if (u_coords) {
		delete[] u_coords;
		u_coords = nullptr;
	}
	if (v_coords) {
		delete[] v_coords;
		v_coords = nullptr;
	}
	if (u_scales) {
		delete[] u_scales;
		u_scales = nullptr;
	}
	if (v_scales) {
		delete[] v_scales;
		v_scales = nullptr;
	}
	if (u_offsets) {
		delete[] u_offsets;
		u_offsets = nullptr;
	}
	if (v_offsets) {
		delete[] v_offsets;
		v_offsets = nullptr;
	}
	if (texture_names) {
		delete[] texture_names;
		texture_names = nullptr;
	}

	// Store our texture count;
	texture_count = new_texture_count;

	// Allocate arrays for uv coordinates.
	uv_coords_count = tri_count * texture_count * 3;
	u_coords = new float[uv_coords_count + 1];
	v_coords = new float[uv_coords_count + 1];

	// Allocate arrays of texture info.
	uv_scales_count = tri_count;
	u_scales = new float[uv_scales_count + 1];
	v_scales = new float[uv_scales_count + 1];

	uv_offsets_count = tri_count;
	u_offsets = new float[uv_offsets_count + 1];
	v_offsets = new float[uv_offsets_count + 1];

	texture_names_count = tri_count;
	texture_names = new char *[texture_names_count + 1];
	memset(texture_names, 0, sizeof(char *) * texture_names_count);
}

int32_t &MeshInfo::get_u_repeat() {
	return u_repeat;
}

int32_t &MeshInfo::get_v_repeat() {
	return v_repeat;
}

float *MeshInfo::get_u_coords() {
	return u_coords;
}

float *MeshInfo::get_v_coords() {
	return v_coords;
}

float *MeshInfo::get_u_scale() {
	return u_scales;
}

float *MeshInfo::get_v_scale() {
	return v_scales;
}

float *MeshInfo::get_u_offset() {
	return u_offsets;
}

float *MeshInfo::get_v_offset() {
	return v_offsets;
}

void MeshInfo::set_texture_name(uint32_t index, const char *name) {
	// Don't allow a buffer overflow!
	if (index >= texture_names_count) { return; }

	texture_names[index] = (char *)name;
}

char **MeshInfo::get_texture_names() {
	return texture_names;
}

void MeshInfo::write(BinaryFile &file) {
	file.write(triangle_count);
	file.write(control_vertices_count);
	for (uint32_t i = 0; i < control_vertices_count; i++) {
		Vector4d &control_vertex = control_vertices[i];
		file.write(control_vertex);
	}
	file.write(indicies_count);
	for (uint32_t i = 0; i < indicies_count; i++) {
		int32_t &index = indices[i];
		file.write(index);
	}
	file.write(vertices_count);
	for (uint32_t i = 0; i < vertices_count; i++) {
		Vector4d &vertex = vertices[i];
		file.write(vertex);
	}
	file.write(normals_count);
	for (uint32_t i = 0; i < normals_count; i++) {
		Vector4d &normal = normals[i];
		file.write(normal);
	}
	file.write(u_repeat);
	file.write(v_repeat);
	file.write(uv_coords_count);
	for (uint32_t i = 0; i < uv_coords_count; i++) {
		float &u_coord = u_coords[i];
		float &v_coord = v_coords[i];
		file.write(u_coord);
		file.write(v_coord);
	}
	file.write(uv_scales_count);
	for (uint32_t i = 0; i < uv_scales_count; i++) {
		float &u_scale = u_scales[i];
		float &v_scale = v_scales[i];
		file.write(u_scale);
		file.write(v_scale);
	}
	file.write(uv_offsets_count);
	for (uint32_t i = 0; i < uv_offsets_count; i++) {
		float &u_offset = u_offsets[i];
		float &v_offset = v_offsets[i];
		file.write(u_offset);
		file.write(v_offset);
	}
	file.write(texture_count);
	file.write(texture_names_count);
	for (uint32_t i = 0; i < texture_names_count; i++) {
		char *&texture_name = texture_names[i];
		if (texture_name == nullptr) { continue; }
		file.write(strlen(texture_name));
		file.write(texture_name);
	}
}