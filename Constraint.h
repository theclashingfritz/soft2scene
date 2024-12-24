#pragma once
#include <stdint.h>

#include "BinaryFile.h"
#include "Element.h"
#include "Types.h"

class PositionLimits;

class Constraint {
	public:
		enum Type: uint8_t {
			INVALID,
			POSITION,
			ORIENTATION,
			SCALE,
			POSITION_LIMIT,
			ROTATION_LIMIT,
            UP_VCT,
		};

	public:
		Constraint(Type ctype);
		~Constraint();

		Type get_type();
        
        void set_active(bool is_active);
        bool get_active();

		void set_passive_element(Element *element);
		Element *get_passive_element();

		void prepare_active_elements(uint32_t count);
		Element **get_active_elements();

		uint32_t get_active_elements_count();

		void set_passive_element_index(SIZE index);
		SIZE get_passive_element_index();

		void prepare_active_element_indicies();
		SIZE *get_active_element_indicies();

		// Constraint Attribute Functions
		PositionLimits *get_position_limits();

		// Read/Write Functions
		void write(BinaryFile &file);

	private:
		// The type of constraint this is, INVALID means there is no valid type set.
		Type type = INVALID;
        
        // If the constraint is activated.
        bool active = true;

		// The 'passive' element, This is the target of the constraint and what the constraints, well constrain.
		Element *passive_element = nullptr;

		// The 'active' elements, The are the elements that the constraint will constrain the passive element (target) from. 
		Element **active_elements = nullptr;
		uint32_t active_elements_count = 0;

		// Constraint Attributes
		PositionLimits *position_limits = nullptr;

		// Read/Write specfic information
		SIZE passive_element_index = 0;

		SIZE *active_element_indicies = nullptr;
};

class PositionLimits {
	public:
		enum ShapeType : uint8_t {
			SPHERE,
			BOX,
		};

		PositionLimits() {};
		~PositionLimits() {};

		void set_coordinate_system(CoordinateSystem system);
		CoordinateSystem get_coordinate_system();

		void set_shape_type(ShapeType type);
		ShapeType get_shape_type();

		void set_damping(float width, float strength);
		float get_damping_width();
		float get_damping_strength();

		void set_radius(float rad);
		float get_radius();

		void set_max_pos(float x, float y, float z);
		Vector3f &get_max_pos();

		void set_min_pos(float x, float y, float z);
		Vector3f &get_min_pos();

		void set_max_pos_active(bool active_x, bool active_y, bool active_z);
		Vector3b &get_max_pos_active();

		void set_min_pos_active(bool active_x, bool active_y, bool active_z);
		Vector3b &get_min_pos_active();

		void write(BinaryFile &file);

	private:
		CoordinateSystem coordinate_system = CoordinateSystem::GLOBAL;

		ShapeType shape_type = SPHERE;

		float damping_width = 0.0f;
		float damping_strength = 0.0f;

		float radius = 0.0f;

		Vector3f max_pos = { 0.0f };
		Vector3f min_pos = { 0.0f };

		Vector3b max_pos_active = { false };
		Vector3b min_pos_active = { false };
};