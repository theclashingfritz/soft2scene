#pragma once
#include <stdint.h>
#include <string>

#include "Types.h"

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
		void set_child(uint32_t index, Element *child);
		Element *get_child(uint32_t index);
		uint32_t get_children_amount();

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