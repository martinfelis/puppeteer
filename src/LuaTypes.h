#ifndef MARKER_MODEL_LUA_TYPES
#define MARKER_MODEL_LUA_TYPES

#include <rbdl/rbdl_math.h>
#include <rbdl/addons/luamodel/luatables.h>

template<> RigidBodyDynamics::Math::Vector3d LuaTableNode::getDefault<RigidBodyDynamics::Math::Vector3d>(const RigidBodyDynamics::Math::Vector3d &default_value) { 
	RigidBodyDynamics::Math::Vector3d result = default_value;

	if (stackQueryValue()) {
		LuaTable vector_table = LuaTable::fromLuaState (luaTable->L);

		if (vector_table.length() != 3) {
			std::cerr << "LuaModel Error: invalid 3d vector!" << std::endl;
			abort();
		}
		
		result[0] = vector_table[1];
		result[1] = vector_table[2];
		result[2] = vector_table[3];
	}

	stackRestore();

	return result;
};

template<> RigidBodyDynamics::Math::SpatialVector LuaTableNode::getDefault<RigidBodyDynamics::Math::SpatialVector>(const RigidBodyDynamics::Math::SpatialVector &default_value) {
	RigidBodyDynamics::Math::SpatialVector result = default_value;

	if (stackQueryValue()) {
		LuaTable vector_table = LuaTable::fromLuaState (luaTable->L);
		
		if (vector_table.length() != 6) {
			std::cerr << "LuaModel Error: invalid 6d vector!" << std::endl;
			abort();
		}
		result[0] = vector_table[1];
		result[1] = vector_table[2];
		result[2] = vector_table[3];
		result[3] = vector_table[4];
		result[4] = vector_table[5];
		result[5] = vector_table[6];
	}

	stackRestore();

	return result;
}

template<> RigidBodyDynamics::Math::Matrix3d LuaTableNode::getDefault<RigidBodyDynamics::Math::Matrix3d>(const RigidBodyDynamics::Math::Matrix3d &default_value) {
	RigidBodyDynamics::Math::Matrix3d result = default_value;

	if (stackQueryValue()) {
		LuaTable vector_table = LuaTable::fromLuaState (luaTable->L);
		
		if (vector_table.length() != 3) {
			std::cerr << "LuaModel Error: invalid 3d matrix!" << std::endl;
			abort();
		}

		if (vector_table[1].length() != 3
				|| vector_table[2].length() != 3
				|| vector_table[3].length() != 3) {
			std::cerr << "LuaModel Error: invalid 3d matrix!" << std::endl;
			abort();
		}

		result(0,0) = vector_table[1][1];
		result(0,1) = vector_table[1][2];
		result(0,2) = vector_table[1][3];

		result(1,0) = vector_table[2][1];
		result(1,1) = vector_table[2][2];
		result(1,2) = vector_table[2][3];

		result(2,0) = vector_table[3][1];
		result(2,1) = vector_table[3][2];
		result(2,2) = vector_table[3][3];
	}

	stackRestore();

	return result;
}

template<> RigidBodyDynamics::Math::SpatialTransform LuaTableNode::getDefault<RigidBodyDynamics::Math::SpatialTransform>(const RigidBodyDynamics::Math::SpatialTransform &default_value) {
	RigidBodyDynamics::Math::SpatialTransform result = default_value;

	if (stackQueryValue()) {
		LuaTable vector_table = LuaTable::fromLuaState (luaTable->L);
	
		result.r = vector_table["r"].getDefault<RigidBodyDynamics::Math::Vector3d>(RigidBodyDynamics::Math::Vector3d::Zero(3));
		result.E = vector_table["E"].getDefault<RigidBodyDynamics::Math::Matrix3d>(RigidBodyDynamics::Math::Matrix3d::Identity (3,3));
	}

	stackRestore();

	return result;
}

template<> RigidBodyDynamics::Joint LuaTableNode::getDefault<RigidBodyDynamics::Joint>(const RigidBodyDynamics::Joint &default_value) {
	RigidBodyDynamics::Joint result = default_value;

	if (stackQueryValue()) {
		LuaTable vector_table = LuaTable::fromLuaState (luaTable->L);

		int joint_dofs = vector_table.length();

		switch (joint_dofs) {
			case 0: result = RigidBodyDynamics::Joint(RigidBodyDynamics::JointTypeFixed);
							break;
			case 1: result = RigidBodyDynamics::Joint(vector_table[1].get<RigidBodyDynamics::Math::SpatialVector>());
							break;
			case 2: result = RigidBodyDynamics::Joint(
									vector_table[1].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[2].get<RigidBodyDynamics::Math::SpatialVector>()
									);
							break;
			case 3: result = RigidBodyDynamics::Joint(
									vector_table[1].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[2].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[3].get<RigidBodyDynamics::Math::SpatialVector>()
									);
							break;
			case 4: result = RigidBodyDynamics::Joint(
									vector_table[1].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[2].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[3].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[4].get<RigidBodyDynamics::Math::SpatialVector>()
									);
							break;
			case 5: result = RigidBodyDynamics::Joint(
									vector_table[1].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[2].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[3].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[4].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[5].get<RigidBodyDynamics::Math::SpatialVector>()
									);
							break;
			case 6: result = RigidBodyDynamics::Joint(
									vector_table[1].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[2].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[3].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[4].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[5].get<RigidBodyDynamics::Math::SpatialVector>(),
									vector_table[6].get<RigidBodyDynamics::Math::SpatialVector>()
									);
							break;
			default:
							std::cerr << "Invalid number of DOFs for joint." << std::endl;
							abort();
		}
	}

	stackRestore();

	return result;
}

template<> RigidBodyDynamics::Body LuaTableNode::getDefault<RigidBodyDynamics::Body>(const RigidBodyDynamics::Body &default_value) {
	RigidBodyDynamics::Body result = default_value;

	if (stackQueryValue()) {
		LuaTable vector_table = LuaTable::fromLuaState (luaTable->L);

		double mass = 0.;
		RigidBodyDynamics::Math::Vector3d com (RigidBodyDynamics::Math::Vector3d::Zero(3));
		RigidBodyDynamics::Math::Matrix3d inertia (RigidBodyDynamics::Math::Matrix3d::Identity(3,3));

		mass = vector_table["mass"];
		com = vector_table["com"].getDefault<RigidBodyDynamics::Math::Vector3d>(com);
		inertia = vector_table["inertia"].getDefault<RigidBodyDynamics::Math::Matrix3d>(inertia);

		result = RigidBodyDynamics::Body (mass, com, inertia);
	}

	stackRestore();

	return result;
}

/* MARKER_MODEL_LUA_TYPES */
#endif
