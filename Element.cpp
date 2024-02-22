#include "Element.h"

Element::Element() {

}

Element::~Element() {
	if (children) {
		delete[] children;
		children = nullptr;
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
	children = new Element *[children_count];
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


// Model Info

ModelInfo::ModelInfo() {

}

ModelInfo::~ModelInfo() {
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
}

void ModelInfo::set_triangle_count(uint32_t new_triangle_count) {
	triangle_count = new_triangle_count;
}

uint32_t ModelInfo::get_triangle_count() {
	return triangle_count;
}

void ModelInfo::prepare_control_vertices(uint32_t new_vertices_count) {
	// If we already HAVE control vertices. Then release the buffer.
	if (control_vertices) {
		delete[] control_vertices;
		control_vertices = nullptr;
	}

	control_vertices_count = new_vertices_count;

	// Allocate a buffer which can hold all of the pointers to our indices.
	control_vertices = new Vector4d[control_vertices_count];
	memset(control_vertices, 0.0, sizeof(Vector4d) * control_vertices_count);
}

Vector4d* ModelInfo::get_control_vertices() {
	return control_vertices;
}

uint32_t ModelInfo::get_control_verticies_count() {
	return control_vertices_count;
}

void ModelInfo::prepare_indicies(uint32_t new_indicies_count) {
	// If we already HAVE indicies. Then release the buffer.
	if (indices) {
		delete[] indices;
		indices = nullptr;
	}

	indicies_count = new_indicies_count;

	// Allocate a buffer which can hold all of the pointers to our indices.
	indices = new int[indicies_count];
	memset(indices, 0, sizeof(int32_t) * indicies_count);
}

int32_t * ModelInfo::get_indicies() {
	return indices;
}

uint32_t ModelInfo::get_indicies_count() {
	return indicies_count;
}

void ModelInfo::prepare_vertices(uint32_t new_vertices_count) {
	// If we already HAVE vertices. Then release the buffer.
	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
	}

	vertices_count = new_vertices_count;

	// Allocate a buffer which can hold all of the pointers to our vertices.
	vertices = new Vector4d[vertices_count];
	memset(vertices, 0, sizeof(Vector4d) * vertices_count);
}

Vector4d * ModelInfo::get_vertices() {
	return vertices;
}

uint32_t ModelInfo::get_vertices_count() {
	return vertices_count;
}

void ModelInfo::prepare_normals(uint32_t new_normals_count) {
	// If we already HAVE normals. Then release the buffer.
	if (normals) {
		delete[] normals;
		normals = nullptr;
	}

	normals_count = new_normals_count;

	// Allocate a buffer which can hold all of the pointers to our normals.
	normals = new Vector4d[normals_count];
	memset(normals, 0, sizeof(Vector4d) * normals_count);
}

Vector4d* ModelInfo::get_normals() {
	return normals;
}

uint32_t ModelInfo::get_normals_count() {
	return normals_count;
}