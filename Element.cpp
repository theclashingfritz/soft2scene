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
	if (node) {
		delete node;
		node = nullptr;
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

void Element::set_as_joint(bool is_joint) {
	joint = is_joint;
}

bool Element::is_joint() {
	return joint;
}

void Element::prepare_joint_linklist_node() {
	node = new JointLinkListNode;
}

JointLinkListNode *Element::get_joint_linklist_node() {
	return node;
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

void Element::set_array_position(int64_t index) {
	array_index = index;
}

int64_t Element::get_array_position() {
	return array_index;
}

void Element::set_parent_position(int64_t index) {
	parent_index = index;
}

int64_t Element::get_parent_position() {
	return parent_index;
}

void Element::prepare_children_indicies() {
	if (children_indexes) {
		delete[] children_indexes;
		children_indexes = nullptr;
	}

	children_indexes = new int64_t[children_count + 1];
	memset(children_indexes, NULL, sizeof(int64_t) * children_count);
}

int64_t *Element::get_children_indicies() {
	return children_indexes;
}

void Element::write(BinaryFile &file) {
	file.write_uint64(name.size());
	file.write(name);
	file.write(flags);
	file.write(matrix);
	file.write(pos);
	file.write(rot);
	file.write(scale);
	file.write(visibility);
	file.write(joint);
	file.write(array_index);
	file.write(parent_index);
	file.write(children_count);
	for (uint32_t i = 0; i < children_count; i++) {
		SIZE child_index = children_indexes[i];
		file.write(child_index);
	}
	if (flags & Element::InfoTypes::Mesh) {
		assert(mesh_info != nullptr);
		mesh_info->write(file);
	}
	if (joint) {
		node->write(file);
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
}

void MeshInfo::set_triangle_count(uint32_t new_triangle_count) {
	triangle_count = new_triangle_count;
}

uint32_t MeshInfo::get_triangle_count() {
	return triangle_count;
}

void MeshInfo::set_material_id(uint32_t id) {
    material_id = id;
}

uint32_t MeshInfo::get_material_id() {
    return material_id;
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

void MeshInfo::prepare_uv_coords(uint32_t new_uv_coords_count) {
	// Free arrays if they're already allocated.
	if (u_coords) {
		delete[] u_coords;
		u_coords = nullptr;
	}
	if (v_coords) {
		delete[] v_coords;
		v_coords = nullptr;
	}

	// Store our counts.
	uv_coords_count = new_uv_coords_count;

	// Allocate arrays for uv coordinates.
	u_coords = new float[uv_coords_count + 1];
	memset(u_coords, 0, sizeof(float) * uv_coords_count);

	v_coords = new float[uv_coords_count + 1];
	memset(v_coords, 0, sizeof(float) * uv_coords_count);
}

float *MeshInfo::get_u_coords() {
	return u_coords;
}

float *MeshInfo::get_v_coords() {
	return v_coords;
}

void MeshInfo::write(BinaryFile &file) {
	file.write(triangle_count);
    file.write(material_id);
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
	file.write(uv_coords_count);
	for (uint32_t i = 0; i < uv_coords_count; i++) {
		float &u_coord = u_coords[i];
		float &v_coord = v_coords[i];
		file.write(u_coord);
		file.write(v_coord);
	}
}


JointLinkListNode::~JointLinkListNode() {
	if (next) {
		delete[] next;
		next = nullptr;
	}
	if (next_indicies) {
		delete[] next_indicies;
		next_indicies = nullptr;
	}
}


void JointLinkListNode::prepare_next() {
	// Free arrays if they're already allocated.
	if (next) {
		delete[] next;
		next = nullptr;
	}

	next = new Element *[next_count + 1];
	memset(next, 0, sizeof(Element *) * next_count);
}

Element **JointLinkListNode::get_next() {
	return next;
}

void JointLinkListNode::set_next_count(SIZE count) {
	next_count = count;
}

SIZE JointLinkListNode::get_next_count() {
	return next_count;
}

void JointLinkListNode::set_previous(Element *element) {
	previous = element;
}

Element *JointLinkListNode::get_previous() {
	return previous;
}

void JointLinkListNode::prepare_next_indicies() {
	// Free arrays if they're already allocated.
	if (next_indicies) {
		delete[] next_indicies;
		next_indicies = nullptr;
	}

	next_indicies = new int64_t[next_count + 1];
	memset(next_indicies, 0, sizeof(int64_t) * next_count);
}

int64_t *JointLinkListNode::get_next_indicies() {
	return next_indicies;
}

void JointLinkListNode::set_previous_index(int64_t index) {
	previous_index = index;
}

int64_t JointLinkListNode::get_previous_index() {
	return previous_index;
}

void JointLinkListNode::write(BinaryFile &file) {
	file.write(next_count);
	for (SIZE i = 0; i < next_count; i++) {
		int64_t next_index = next_indicies[i];
		file.write(next_index);
	}
	file.write(previous_index);
}