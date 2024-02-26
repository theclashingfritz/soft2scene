#include "Constraint.h"

#include <assert.h>

Constraint::Constraint(Constraint::Type ctype) {
	type = ctype;
	switch (type) {
		case POSITION_LIMIT:
			position_limits = new PositionLimits;
			break;
		default:
			break;
	}
}

Constraint::~Constraint() {
	if (active_elements) {
		delete[] active_elements;
		active_elements = nullptr;
	}
	if (active_element_indicies) {
		delete[] active_element_indicies;
		active_element_indicies = nullptr;
	}

	// Constraint Attributes

	if (position_limits) {
		delete position_limits;
		position_limits = nullptr;
	}
}

Constraint::Type Constraint::get_type() {
	return type;
}

void Constraint::set_passive_element(Element *element) {
	assert(element != nullptr);
	passive_element = element;
}

Element *Constraint::get_passive_element() {
	return passive_element;
}

void Constraint::prepare_active_elements(uint32_t count) {
	if (count <= 0) { return; }

	// Free the buffer if it's already in use.
	if (active_elements) {
		delete[] active_elements;
		active_elements = nullptr;
	}

	active_elements_count = count;
	active_elements = new Element *[active_elements_count + 1];
	memset(active_elements, 0, sizeof(Element *) * active_elements_count);
}

Element **Constraint::get_active_elements() {
	assert(active_elements != nullptr);
	return active_elements;
}

uint32_t Constraint::get_active_elements_count() {
	return active_elements_count;
}

void Constraint::set_passive_element_index(SIZE index) {
	passive_element_index = index;
}

SIZE Constraint::get_passive_element_index() {
	return passive_element_index;
}

void Constraint::prepare_active_element_indicies() {
	if (active_elements_count <= 0) { return; }

	// Free the buffer if it's already in use.
	if (active_element_indicies) {
		delete[] active_element_indicies;
		active_element_indicies = nullptr;
	}

	active_element_indicies = new SIZE[active_elements_count + 1];
	memset(active_element_indicies, 0, sizeof(SIZE) * active_elements_count);
}

SIZE *Constraint::get_active_element_indicies() {
	assert(active_element_indicies != nullptr);
	return active_element_indicies;
}

PositionLimits *Constraint::get_position_limits() {
	assert(position_limits != nullptr);
	return position_limits;
}

void Constraint::write(BinaryFile &file) {
	file.write(type);
	file.write(passive_element_index);
	file.write(active_elements_count);
	assert(active_element_indicies != nullptr);
	for (uint32_t i = 0; i < active_elements_count; i++) {
		SIZE active_element_index = active_element_indicies[i];
		file.write(active_element_index);
	}
	if (position_limits) {
		position_limits->write(file);
	}
}

void PositionLimits::set_coordinate_system(CoordinateSystem system) {
	coordinate_system = system;
}

CoordinateSystem PositionLimits::get_coordinate_system() {
	return coordinate_system;
}

void PositionLimits::set_shape_type(PositionLimits::ShapeType type) {
	shape_type = type;
}

PositionLimits::ShapeType PositionLimits::get_shape_type() {
	return shape_type;
}

void PositionLimits::set_damping(float width, float strength) {
	damping_width = width;
	damping_strength = strength;
}

float PositionLimits::get_damping_width() {
	return damping_width;
}

float PositionLimits::get_damping_strength() {
	return damping_strength;
}

void PositionLimits::set_radius(float rad) {
	radius = rad;
}

float PositionLimits::get_radius() {
	return radius;
}

void PositionLimits::set_max_pos(float x, float y, float z) {
	max_pos.x = x;
	max_pos.y = y;
	max_pos.z = z;
}

Vector3f &PositionLimits::get_max_pos() {
	return max_pos;
}

void PositionLimits::set_min_pos(float x, float y, float z) {
	min_pos.x = x;
	min_pos.y = y;
	min_pos.z = z;
}

Vector3f &PositionLimits::get_min_pos() {
	return min_pos;
}

void PositionLimits::set_max_pos_active(bool active_x, bool active_y, bool active_z) {
	max_pos_active.x = active_x;
	max_pos_active.y = active_y;
	max_pos_active.z = active_z;
}

Vector3b &PositionLimits::get_max_pos_active() {
	return max_pos_active;
}

void PositionLimits::set_min_pos_active(bool active_x, bool active_y, bool active_z) {
	min_pos_active.x = active_x;
	min_pos_active.y = active_y;
	min_pos_active.z = active_z;
}

Vector3b &PositionLimits::get_min_pos_active() {
	return min_pos_active;
}

void PositionLimits::write(BinaryFile &file) {
	file.write(coordinate_system); // Coordinate System
	file.write(shape_type); // Shape Type

	// Damping
	file.write(damping_width);
	file.write(damping_strength);

	file.write(radius); // Radius
	file.write(max_pos); // Maximum Values
	file.write(min_pos); // Minimum Values
	file.write(max_pos_active); // Maximum Values Active
	file.write(min_pos_active); // Minimum Values Active
}