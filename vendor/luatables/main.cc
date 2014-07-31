/*
 * luatables
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 * 
 * (zlib license)
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */

#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>

#include "luatables.h"

using namespace std;

int main (int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <luafile>" << endl;
		exit(1);
	}

	LuaTable input (LuaTable::fromFile(argv[1]));

	if (input["name"].exists()) {
		cout << "Field 'name' found" << endl;
	} else {
		cout << "Field 'name' NOT found" << endl;
	}

	if (input["nested"]["boolean"].exists()) {
		cout << "Nested Boolean found!" << endl;
		cout << "Nested Boolean Value = " << input["nested"]["boolean"].getDefault<bool>(false) << endl;
	} else {
		cout << "Nested Boolean NOT found!" << endl;
	}

	if (input["some"]["nested"]["boolean"].exists()) {
		cout << "Boolean found!" << endl;
		cout << "Boolean Value = " << input["some"]["nested"]["boolean"].getDefault<bool>(false) << endl;
	} else {
		cout << "Boolean NOT found!" << endl;
	}

	cout << input["array"][1].getDefault<bool>(false) << endl;
	cout << input["array"][2].getDefault<bool>(false) << endl;
	cout << input["array"][3].getDefault<bool>(false) << endl;
	cout << input["array"][4].getDefault<bool>(false) << endl;

	return 0;
}
