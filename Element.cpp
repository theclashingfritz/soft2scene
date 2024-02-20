#include "Element.h"

Element::Element() {

}

Element::~Element() {

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