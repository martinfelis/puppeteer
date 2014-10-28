/// Puppeteer Lua Scripting Module.
// @module puppeteer

#include "Scripting.h"
#include "MarkerData.h"

#include <errno.h>

#include <iostream>
#include <cstdio>
#include <fstream>

using namespace std;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

PuppeteerApp *app_ptr = NULL;

static void stack_print (const char *file, int line, lua_State *L) {
	int stack_top = lua_gettop(L);
	cout << file << ":" << line << ": stack size: " << lua_gettop(L) << endl;;
	for (unsigned int i = 1; i < lua_gettop(L) + 1; i++) {
		cout << file << ":" << line << ": ";
		cout << i << ": ";
		if (lua_istable (L, i))
			cout << " table" << endl;
		else if (lua_isnumber (L, i))
			cout << " number: " << lua_tonumber (L, i) << endl;
		else if (lua_isuserdata (L, i)) {
			void* userdata = (void*) lua_touserdata (L, i);
			cout << " userdata (" << userdata << ")" << endl;
		} else if (lua_isstring (L, i))
			cout << " string: " << lua_tostring(L, i) << endl;
		else if (lua_isfunction (L, i))
			cout << " function" << endl;
		else
			cout << " unknown: " << lua_typename (L, lua_type (L, i)) << endl;
	}
	assert (lua_gettop(L) == stack_top);
}
#define print_stack_types(L) stack_print ("rbdl-lua.cc", __LINE__, L)

void print_table (lua_State *L) {
	int stack_top = lua_gettop(L);
	
	if (!lua_istable(L, lua_gettop(L))) {
		cerr << "Error: expected table on top of stack but got " << lua_typename(L, lua_type(L, lua_gettop(L))) << endl;
		if (lua_isstring(L, lua_gettop(L))) {
			cout << " string value = " << lua_tostring(L, lua_gettop(L)) << endl;
		}
		print_stack_types (L);
		abort();
	}

	cout << "table = {" << endl;
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		cout << lua_tostring (L, -2) << ": ";
		if (lua_istable (L, -1))
			cout << " table" << endl;
		else if (lua_isnumber (L, -1))
			cout << " number: " << lua_tonumber (L, -1) << endl;
		else if (lua_isuserdata (L, -1)) {
			void* userdata = (void*) lua_touserdata (L, -1);
			cout << " userdata (" << userdata << ")" << endl;
		} else if (lua_isstring (L, -1))
			cout << " string: " << lua_tostring(L, -1) << endl;
		else if (lua_isfunction (L, -1))
			cout << " function" << endl;
		else
			cout << " unknown: " << lua_typename (L, lua_type (L, -1)) << endl;
		lua_pop(L, 1);
	}
	cout << "}" << endl;
	assert (lua_gettop(L) == stack_top);
}

static bool file_exists (const char* filename) {
	bool result = false;
	ifstream test_file (filename);
	if (test_file)
		result = true;
	
	test_file.close();

	return result;
}

void register_functions (lua_State *L);

void scripting_init (PuppeteerApp *app, const char* init_filename) {
	app_ptr = app;

	app->L = luaL_newstate();
	luaL_openlibs(app->L);

	lua_newtable (app->L);
	lua_setglobal(app->L, "puppeteer");

	register_functions (app->L);

	if (file_exists (init_filename)) {
		if (!luaL_loadfile(app->L, init_filename)) {
			lua_call (app->L, 0, 0);
		} else {
			cerr << "Error running " << init_filename << ": " << lua_tostring(app->L, lua_gettop(app->L)) << endl;
			// pop the error message and ignore it
			lua_pop (app->L, 1);
			abort();
		}
	}

	assert (lua_gettop(app->L) == 0);
}

void scripting_load (lua_State *L, const int argc, char* argv[]) {
	assert (lua_gettop(L) == 0);
	lua_getglobal (L, "puppeteer");
	if (lua_istable(L, 1)) {
		lua_getfield (L, 1, "load");
		if (lua_isfunction(L, 2)) {
			lua_createtable (L, argc, 0);
			for (int i = 0; i < argc; i ++) {
				lua_pushnumber (L, i + 1);

				double numvalue = 0.;
				if (sscanf (argv[i], "%lf", &numvalue) == 1) 
					lua_pushnumber (L, numvalue);
				else
					lua_pushstring (L, argv[i]);
				lua_settable (L, 3);
			}
			lua_call (L, 1, 0);
			lua_pop(L,1);
		} else {
			lua_pop(L,2);
		}
	}
	assert (lua_gettop(L) == 0);
}

/***
 * Update function that gets called every frame before it is drawn. 
 * @function puppeteer.update(dt)
 * @param dt the elapsed time in seconds since the last drawing update
*/
void scripting_update (lua_State *L, float dt) {
	assert (lua_gettop(L) == 0);
	assert (L);
	lua_getglobal (L, "puppeteer");
	if (lua_istable(L, 1)) {
		lua_getfield (L, 1, "update");
		if (lua_isfunction(L, 2)) {
			lua_pushnumber(L, dt);
			lua_call (L, 1, 0);
		} else {
			lua_pop(L,1);
		}
	}
	lua_pop (L, 1);

	assert (lua_gettop(L) == 0);
}

/***
 * Issues the scene to be drawn 
 * @function puppeteer.draw()
 */
void scripting_draw (lua_State *L) {
	assert (lua_gettop(L) == 0);
	lua_getglobal (L, "puppeteer");
	if (lua_istable(L, 1)) {
		lua_getfield (L, 1, "draw");
		if (lua_isfunction(L, 2)) {
			lua_call (L, 0, 0);
		} else {
			lua_pop(L,1);
		}
	}
	lua_pop (L, 1);
	assert (lua_gettop(L) == 0);
}

/***
 * Closes puppeteer
 * @function puppeteer.draw()
 */
void scripting_quit (lua_State *L) {
	assert (lua_gettop(L) == 0);
	lua_getglobal (L, "puppeteer");
	if (lua_istable(L, 1)) {
		lua_getfield (L, 1, "quit");
		if (lua_isfunction(L, 2)) {
			lua_call (L, 0, 0);
		} else {
			lua_pop(L,1);
		}
	}
	lua_pop (L, 1);
	assert (lua_gettop(L) == 0);

	app_ptr = NULL;
}

Vector3f l_checkvector3f (lua_State *L, int index) {
	luaL_checktype (L, lua_gettop(L), LUA_TTABLE);

	Vector3f result;
	for (int i = 0; i < 3; i++) {
		lua_rawgeti (L, index, i+1);
		result[i] = luaL_checknumber (L, lua_gettop(L));
		lua_pop(L, 1);
	}

	return result;
}

void l_pushvector3f (lua_State *L, const Vector3f &vec) {
	lua_createtable(L, 3, 0);

	for (int i = 0; i < 3; i++) {
		lua_pushnumber (L, vec[i]);
		lua_rawseti (L, -2, i+1);
	}
}

SimpleMath::GL::Quaternion l_checkquaternion (lua_State *L, int index) {
	luaL_checktype (L, lua_gettop(L), LUA_TTABLE);

	SimpleMath::GL::Quaternion result;
	for (int i = 0; i < 4; i++) {
		lua_rawgeti (L, index, i+1);
		result[i] = luaL_checknumber (L, lua_gettop(L));
		lua_pop(L, 1);
	}

	return result;
}

Transformation l_checktransformation (lua_State *L, int index) {
	luaL_checktype (L, index, LUA_TTABLE);

	SimpleMath::GL::Quaternion rotation;
	Vector3f translation;

	lua_getfield (L, index, "translation");
	if (!lua_isnil(L, lua_gettop(L))) {
		translation = l_checkvector3f (L, lua_gettop(L));
	}
	lua_pop(L, 1);

	lua_getfield (L, index, "rotation");
	if (!lua_isnil(L, lua_gettop(L))) {
		rotation = l_checkquaternion (L, lua_gettop(L));
	}
	lua_pop(L, 1);

	return Transformation (translation, rotation);
}

//
// MarkerData
//

///
// @function puppeteer.mocap_data.getFirstFrame
static int mocap_data_getFirstFrame (lua_State *L) {
	MarkerData* marker_data = app_ptr->markerData;
	if (!marker_data)
		luaL_error (L, "No motion capture file loaded!");

	lua_pushnumber (L, static_cast<double>(marker_data->getFirstFrame()));

	return 1;
}

///
// @function puppeteer.mocap_data.getLastFrame
static int mocap_data_getLastFrame (lua_State *L) {
	MarkerData* marker_data = app_ptr->markerData;

	if (!marker_data)
		luaL_error (L, "No motion capture file loaded!");

	lua_pushnumber (L, static_cast<double>(marker_data->getLastFrame()));

	return 1;
}

///
// @function puppeteer.mocap_data.setCurrentFrameNumber
// @param frame_number
static int mocap_data_setCurrentFrameNumber (lua_State *L) {
	MarkerData* marker_data = app_ptr->markerData;
	
	int frame_number = luaL_checkinteger(L, 1);

	if (!marker_data)
		luaL_error (L, "No motion capture file loaded!");

	marker_data->setCurrentFrameNumber (frame_number);

	return 0;
}

/// Computes the bounding box that encloses the motion capture data.
// @function puppeteer.mocap_data.calcDataBoundingBox
// @return bbox_min
// @return bbox_max
//
// Can be useful for positioning the camera such that the whole trial can
// be seen without moving the camera.
static int mocap_data_calcDataBoundingBox (lua_State *L) {
	MarkerData* marker_data = app_ptr->markerData;

	if (!marker_data)
		luaL_error (L, "No motion capture file loaded!");

	Vector3f min, max;
	marker_data->calcDataBoundingBox (min, max);
	l_pushvector3f (L, min);
	l_pushvector3f (L, max);

	return 2;
}

///
// @function puppeteer.mocap_data.clearMarkers
static int mocap_data_clearMarkers (lua_State *L) {
	MarkerData* marker_data = app_ptr->markerData;

	if (!marker_data)
		luaL_error (L, "No motion capture file loaded!");

	marker_data->clearMarkers();

	return 0;
}

///
// @function puppeteer.mocap_data.enableMarker
// @param marker_name
// @param color
static int mocap_data_enableMarker (lua_State *L) {
	MarkerData* marker_data = app_ptr->markerData;

	if (!marker_data)
		luaL_error (L, "No motion capture file loaded!");

	string marker_name = luaL_checkstring (L, 1);
	Vector3f color = l_checkvector3f (L, 2);

	marker_data->enableMarker (marker_name.c_str(), color);

	return 0;
}

///
// @function puppeteer.mocap_data.getMarkerPosition
// @param marker_name
// @return current marker position 
static int mocap_data_getMarkerCurrentPosition (lua_State *L) {
	MarkerData* marker_data = app_ptr->markerData;

	if (!marker_data)
		luaL_error (L, "No motion capture file loaded!");

	const char* marker_name = luaL_checkstring (L, 1);
	Vector3f position = marker_data->getMarkerCurrentPosition (marker_name);

	l_pushvector3f (L, position);

	return 1;
}

static const struct luaL_Reg puppeteer_mocap_data_f[] = {
	{ "getFirstFrame", mocap_data_getFirstFrame},
	{ "getLastFrame", mocap_data_getLastFrame},
	{ "setCurrentFrameNumber", mocap_data_setCurrentFrameNumber},
	{ "calcDataBoundingBox", mocap_data_calcDataBoundingBox},
	{ "clearMarkers", mocap_data_clearMarkers},
	{ "enableMarker", mocap_data_enableMarker},
	{ "getMarkerCurrentPosition", mocap_data_getMarkerCurrentPosition},
	{ NULL, NULL}
};

///
// @function puppeteer.loadModel
// @param filename
static int puppeteer_loadModel (lua_State *L) {
	string filename = luaL_checkstring (L, 1);

	app_ptr->loadModelFile (filename.c_str());	

	return 0;
}

///
// @function puppeteer.loadMarkerData
// @param filename
static int puppeteer_loadMarkerData (lua_State *L) {
	string filename = luaL_checkstring (L, 1);

	app_ptr->loadMocapFile (filename.c_str());	

	return 0;
}

///
// @function puppeteer.saveScreenShot 
// @param filename
// @param width
// @param height
// @param enable_alpha
static int puppeteer_saveScreenShot (lua_State *L) {
	string filename = luaL_checkstring (L, 1);
	int width = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	bool alpha = false;
	if (lua_gettop(L) >= 4 )
		alpha = lua_toboolean (L, 4);

	app_ptr->saveScreenShot (filename.c_str(), width, height, alpha);

	return 0;
}

///
// @function puppeteer.getCurrentTime()
static int puppeteer_getCurrentTime (lua_State *L) {
	lua_pushnumber (L, app_ptr->getCurrentTime());

	return 1;
}

///
// @function puppeteer.setCurrentTime(time_in_seconds)
static int puppeteer_setCurrentTime (lua_State *L) {
	double time_argument = luaL_checknumber (L, 1);
	app_ptr->setCurrentTime (time_argument);
	lua_pushnumber (L, app_ptr->getCurrentTime());

	return 1;
}

static const struct luaL_Reg puppeteer_f[] = {
	{ "loadModel", puppeteer_loadModel},
	{ "loadMarkerData", puppeteer_loadMarkerData},
	{ "saveScreenShot", puppeteer_saveScreenShot},
	{ "getCurrentTime", puppeteer_getCurrentTime},
	{ "setCurrentTime", puppeteer_setCurrentTime},
	{ NULL, NULL}
};

///
// @function puppeteer.camera.setPointOfInterest
// @param pos
static int camera_setPointOfInterest (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	Vector3f vec = l_checkvector3f (L, 1);
	app_ptr->glWidget->camera.poi = vec;
	app_ptr->glWidget->camera.updateSphericalCoordinates();

	return 0;
}

///
// @function puppeteer.camera.getPointOfInterest
// @return pos
static int camera_getPointOfInterest (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	l_pushvector3f (L, app_ptr->glWidget->camera.poi);
	app_ptr->glWidget->camera.updateSphericalCoordinates();

	return 1;
}

///
// @function puppeteer.camera.setEyePosition
// @param pos
static int camera_setEyePosition (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	Vector3f vec = l_checkvector3f (L, 1);
	app_ptr->glWidget->camera.eye = vec;
	app_ptr->glWidget->camera.updateSphericalCoordinates();

	return 0;
}

///
// @function puppeteer.camera.getEyePosition
// @return pos
static int camera_getEyePosition (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	l_pushvector3f(L, app_ptr->glWidget->camera.eye);

	return 1;
}

///
// @function puppeteer.camera.setUpVector
// @param pos
static int camera_setUpVector (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	Vector3f vec = l_checkvector3f (L, 1);
	app_ptr->glWidget->camera.up = vec;
	app_ptr->glWidget->camera.updateSphericalCoordinates();

	return 0;
}

///
// @function puppeteer.camera.getUpVector
// @return pos
static int camera_getUpVector (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	l_pushvector3f(L, app_ptr->glWidget->camera.up);

	return 1;
}

///
// @function puppeteer.camera.setFieldOfView
// @param fov
static int camera_setFieldOfView (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	double fov = luaL_checknumber (L, 1);
	app_ptr->glWidget->camera.fov = static_cast<float>(fov);

	return 0;
}

///
// @function puppeteer.camera.getFieldOfView
// @return fov
static int camera_getFieldOfView (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	lua_pushnumber (L, app_ptr->glWidget->camera.fov);

	return 1;
}

///
// @function puppeteer.camera.setOrthographic
// @param true/false
static int camera_setOrthographic (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	app_ptr->glWidget->camera.orthographic = lua_toboolean (L, 1);

	return 0;
}

static const struct luaL_Reg puppeteer_camera_f[] = {
	{ "setPointOfInterest", camera_setPointOfInterest},
	{ "getPointOfInterest", camera_getPointOfInterest},
	{ "setEyePosition", camera_setEyePosition},
	{ "getEyePosition", camera_getEyePosition},
	{ "setUpVector", camera_setUpVector},
	{ "getUpVector", camera_getUpVector},
	{ "setFieldOfView", camera_setFieldOfView},
	{ "getFieldOfView", camera_getFieldOfView},
	{ "setOrthographic", camera_setOrthographic},
	{ NULL, NULL}
};

///
// @function puppeteer.scene.enableLighting
// @param true/false
static int scene_enableLighting (lua_State *L) {
	if (!app_ptr->glWidget)
		luaL_error (L, "Puppeteer not yet initialized!");

	app_ptr->scene->lightingEnabled = lua_toboolean(L, 1);

	return 0;
}

static const struct luaL_Reg puppeteer_scene_f[] = {
	{ "enableLighting", scene_enableLighting},
	{ NULL, NULL}
};


void register_functions (lua_State *L){
	lua_getglobal(L, "puppeteer");

	int puppeteer_table = lua_gettop(L);

	luaL_register(L, NULL, puppeteer_f);

	lua_newtable (L); // puppeteer.camera
	luaL_register (L, NULL, puppeteer_camera_f);
	lua_setfield (L, puppeteer_table, "camera");

	lua_newtable (L); // puppeteer.mocap_data
	luaL_register (L, NULL, puppeteer_mocap_data_f);
	lua_setfield (L, puppeteer_table, "mocap_data");

	lua_newtable (L); // puppeteer.scene
	luaL_register (L, NULL, puppeteer_scene_f);
	lua_setfield (L, puppeteer_table, "scene");

	lua_pop (L, 1); // puppeteer

	assert (lua_gettop(L) == 0);
}
