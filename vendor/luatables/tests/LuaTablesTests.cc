#include <UnitTest++.h>

#include <assert.h>
#include <iostream>
#include <algorithm>

#include "luatables.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

using namespace std;

TEST ( TestSimpleExisting ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { value = false }");

	CHECK (ltable["value"].exists());	
}

TEST ( TestSimpleNotExisting ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { value = false }");

	CHECK (!ltable["value_not_existing"].exists());	
}

TEST ( TestNestedExisting ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { nested = { value = true }}");

	CHECK (ltable["nested"]["value"].exists());	
}

TEST ( TestNestedNotExisting ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { nested = { value = true }}");

	CHECK (!ltable["nested"]["value_not_existing"].exists());	
}

TEST ( TestSimpleLength ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { value = {1., 2., 3., 4., 5. }}");

	CHECK_EQUAL (5u, ltable["value"].length());	
}

TEST ( TestSimpleTrueBool ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = true }");

	CHECK (ltable["myvalue"].getDefault<bool>(false));
}

TEST ( TestSimpleFalseBool ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = false }");

	CHECK (!ltable["myvalue"].getDefault<bool>(false));
}

TEST ( TestGlobalValueExpression ) {
	LuaTable ltable = LuaTable::fromLuaExpression (" global_value = true");

	CHECK (ltable["global_value"].exists());
}

TEST ( TestSimpleSetBool ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = false }");

	ltable["newvalue"].set(true);
	CHECK (ltable["newvalue"].exists());

	CHECK (ltable["newvalue"].getDefault<bool>(false));
}

TEST ( TestSimpleSetDouble ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = false }");

	ltable["newvalue"].set(1.23);
	CHECK (ltable["newvalue"].exists());

	CHECK_EQUAL (1.23, ltable["newvalue"].getDefault<double>(0.));
}

TEST ( TestSimpleSetString ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = false }");

	ltable["newvalue"].set<string>("Hello");
	CHECK (ltable["newvalue"].exists());

	CHECK_EQUAL (string("Hello"), ltable["newvalue"].getDefault<std::string>(""));
}

TEST ( TestNestedSetBool ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { }");

	CHECK (!ltable["nested"]["newvalue"].exists());
	CHECK (!ltable["nested"]["newvalue"].getDefault<bool>(false));

	ltable["nested"]["newvalue"].set(true);
	CHECK (ltable["nested"]["newvalue"].exists());
	CHECK (ltable["nested"]["newvalue"].getDefault<bool>(false));
}

TEST ( TestGlobalNestedSetBool ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("");

	CHECK (!ltable["nested"]["newvalue"].exists());
	CHECK (!ltable["nested"]["newvalue"].getDefault<bool>(false));

	ltable["nested"]["newvalue"].set(true);
	CHECK (ltable["nested"]["newvalue"].exists());
	CHECK (ltable["nested"]["newvalue"].getDefault<bool>(false));
}

TEST ( TestGlobalNestedExistingSetBool ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("existing_table = { newvalue = \"bla\"}");

	CHECK (ltable["existing_table"]["newvalue"].exists());
	CHECK (ltable["existing_table"]["newvalue"].getDefault<bool>(false));

	ltable["existing_table"]["newvalue"].set(false);
	CHECK (ltable["existing_table"]["newvalue"].exists());
	CHECK (!ltable["existing_table"]["newvalue"].getDefault<bool>(false));
}

TEST ( TestSimpleGetDouble ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = 42.}");

	CHECK_EQUAL (42., ltable["myvalue"].getDefault<double>(0.));
}

TEST ( TestSimpleGetString ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = \"somestring\"}");

	CHECK_EQUAL (string("somestring"), ltable["myvalue"].getDefault<std::string>(""));
}

TEST ( TestSimpleRemove ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { value = false }");

	CHECK (ltable["value"].exists());	
	ltable["value"].remove();
	CHECK (!ltable["value"].exists());	
}

TEST ( TestGlobalRemove ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("value = false");

	CHECK (ltable["value"].exists());	
	ltable["value"].remove();
	CHECK (!ltable["value"].exists());	
}

TEST ( TestSerialize ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { value1 = \"yoyo\",\n value2 = true}");

	ltable["baem"]["nested"]["somevalue"].set<string>("hello");

	string serialized = ltable.serialize();

	string reference = "return {\n\
  value2 = true,\n\
  baem = {\n\
    nested = {\n\
      somevalue = \"hello\",\n\
    },\n\
  },\n\
  value1 = \"yoyo\",\n\
}";

	CHECK_EQUAL (reference, serialized);
}

TEST ( TestOrderedSerialize ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { value2 = \"yoyo\",\n value1 = true}");

	string serialized = ltable.orderedSerialize();

	string reference = "return {\n\
  value1 = true,\n\
  value2 = \"yoyo\",\n\
}";

	CHECK_EQUAL (reference, serialized);
}

TEST ( TestOrderedSerializeHoles ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { [2] = \"yoyo\",\n [1] = true}");
	ltable[4].set<string>("hello");
	string serialized = ltable.orderedSerialize();

	// TODO: properly align lists with holes
	string reference = "return {\n\
  [1] = true,\n\
  [2] = \"yoyo\",\n\
  [4] = \"hello\",\n\
}";

	CHECK_EQUAL (reference, serialized);
}
TEST ( TestTemplatedGet ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = true }");

	CHECK (ltable["myvalue"].getDefault<bool>(true));
}

TEST ( TestTemplatedSet ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = true }");
	
	ltable["newvalue"].set<bool>(true);
	CHECK (ltable["newvalue"].getDefault<bool>(false));
}

TEST ( TestTemplatedSetAssignment ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = true }");
	
	ltable["newvalue"] = 123.;
	CHECK_EQUAL (123., ltable["newvalue"].getDefault<double>(0.));

	ltable["another_value"] = 5123.;
	CHECK (ltable["another_value"] == 5123.);
	CHECK (5123. == ltable["another_value"]);
	CHECK (5123.5 != ltable["another_value"]);
}

TEST ( TestGetKeys ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("sometable = {}\n\
sometable.fred = \"happy\"\n\
sometable[2] = 3.\n\
sometable[4] = 9.\n\
sometable.joe = \"sad\"\n\
return { nested = sometable }");

	vector<LuaKey> keys = ltable["nested"].keys();

	sort (keys.begin(), keys.end());

	CHECK_EQUAL (2, keys[0].int_value);
	CHECK_EQUAL (4, keys[1].int_value);
	CHECK_EQUAL (string("fred"), keys[2].string_value);
	CHECK_EQUAL (string("joe"), keys[3].string_value);
}

struct CustomType {
	string name;
	double age;
	bool drunken;
	double position[3];
};

template<> CustomType LuaTableNode::getDefault<CustomType>(const CustomType &default_value) {
	CustomType result = default_value;

	if (exists()) {
		LuaTable custom_table = stackQueryTable();
		
		result.name = custom_table["name"].get<std::string>();
		result.age = custom_table["age"];
		result.drunken = custom_table["drunken"];
		result.position[0] = custom_table["position"][1];
		result.position[1] = custom_table["position"][2];
		result.position[2] = custom_table["position"][3];
	}

	stackRestore();

	return result;
}

template<> void LuaTableNode::set<CustomType>(const CustomType &value) {
	LuaTable custom_table = stackCreateLuaTable();

	// set the fields of the custom type
	custom_table["name"] = value.name;
	custom_table["age"] = value.age;
	custom_table["drunken"] = value.drunken;
	custom_table["position"][1] = value.position[0];
	custom_table["position"][2] = value.position[1];
	custom_table["position"][3] = value.position[2];

	// restore the stack
	stackRestore();
}

TEST ( TestSetCustomType ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = true }");

	CustomType agent007;
	agent007.name = "James Bond";
	agent007.age = 42.;
	agent007.drunken = true;
	agent007.position[0] = 123.;
	agent007.position[1] = 456.;
	agent007.position[2] = 789.;
	ltable["agents"][7] = agent007;

	CHECK (std::string("James Bond") == ltable["agents"][7]["name"].getDefault<std::string>("unknown"));
	CHECK (42. == ltable["agents"][7]["age"]);
	CHECK (true == ltable["agents"][7]["drunken"]);
	CHECK (123. == ltable["agents"][7]["position"][1]);
	CHECK (456. == ltable["agents"][7]["position"][2]);
	CHECK (789. == ltable["agents"][7]["position"][3]);
}

TEST ( TestGetCustomType ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { myvalue = true }");

	CustomType agent007;
	agent007.name = "James Bond";
	agent007.age = 42.;
	agent007.drunken = true;
	agent007.position[0] = 123.;
	agent007.position[1] = 456.;
	agent007.position[2] = 789.;
	ltable["agents"][123] = agent007;

	CustomType other_agent = ltable["agents"][123];

	CHECK_EQUAL (agent007.name, other_agent.name);
	CHECK_EQUAL (agent007.age, other_agent.age);
	CHECK_EQUAL (agent007.drunken, other_agent.drunken);
	CHECK (agent007.position[0] == other_agent.position[0]);
	CHECK (agent007.position[1] == other_agent.position[1]);
	CHECK (agent007.position[2] == other_agent.position[2]);
}

struct Vector3 {
	double data[3];
};

template<> Vector3 LuaTableNode::getDefault<Vector3>(const Vector3 &default_value) {
	Vector3 result = default_value;

	if (stackQueryValue()) {
		LuaTable custom_table = LuaTable::fromLuaState (luaTable->L);

		result.data[0] = custom_table[1];
		result.data[1] = custom_table[2];
		result.data[2] = custom_table[3];
	}

	stackRestore();

	return result;
}

template<> void LuaTableNode::set<Vector3>(const Vector3 &value) {
	LuaTable custom_table = stackCreateLuaTable();

	// set the fields of the custom type
	custom_table[1] = value.data[0];
	custom_table[2] = value.data[1];
	custom_table[3] = value.data[2];

	// restore the stack
	stackRestore();
}

TEST ( TestGetVec3 ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { vector = { 1., 2., 3.} }");

	Vector3 vector = ltable["vector"];

	CHECK_EQUAL (1., vector.data[0]);
	CHECK_EQUAL (2., vector.data[1]);
	CHECK_EQUAL (3., vector.data[2]);
}

TEST ( TestSetVec3 ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { vector = { 1., 2., 3.} }");

	Vector3 vector;
	vector.data[0] = -1.;
	vector.data[1] = -2.;
	vector.data[2] = -3.;

	ltable["othervector"] = vector;
	Vector3 othervector = ltable["othervector"];

	CHECK_EQUAL (-1., othervector.data[0]);
	CHECK_EQUAL (-2., othervector.data[1]);
	CHECK_EQUAL (-3., othervector.data[2]);
}

TEST ( TestTableLength ) {
	LuaTable ltable = LuaTable::fromLuaExpression ("return { 1., 2., 3., 4., 5., 6., 7., 8., 9., 0. }");

	CHECK_EQUAL (10., ltable.length());
}
