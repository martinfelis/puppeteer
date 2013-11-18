#include <assert.h>
#include <iostream>

#include "c3dfile.h"
#include "c3dutils.h"

#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main (int argc, char* argv[]) {
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " <filename.c3d> <list|info|marker_id>" << endl;
		exit(-1);
	}

	C3DFile c3dfile;
	assert(c3dfile.load (argv[1]));

	if (!strcmp("list", argv[2])) {
		std::vector<std::string> marker_list = c3dfile.point_label;
		size_t i;

		for (i = 0; i < marker_list.size(); i++) {
			cout << "Label[" << i << "] = " << marker_list[i] << endl;
		}
	} else if (!strcmp("info", argv[2])) {
		// print out some information of the file
		cout << "== Header ==" << endl;
		print_c3dheaderinfo (c3dfile.header);
		cout << "== Parameters ==" << endl;
		size_t i;
		ParameterInfo parameter_info;
		GroupInfo group_info;

		for (i = 0; i < c3dfile.param_infos.size(); i++) {
			parameter_info = c3dfile.param_infos.at(i);
			int group_id = parameter_info.group_id;
			int group_index = c3dfile.group_id_to_index_map[group_id];
			group_info = c3dfile.group_infos[group_index];
			cout << group_info.name << ":" << parameter_info.name << " (";
			switch (parameter_info.data_type) {
				case (1):
				case (-1):
					cout << "int8): " << static_cast<int>(parameter_info.char_data[0]) << endl;
					break;
				case (2):
					cout << "Sint16): " << parameter_info.int_data[0] << endl;
					break;
				case (4):
					cout << "float): " << parameter_info.float_data[0] << endl;
					break;
				default:
					cout << "invalid type: " << static_cast<int>(parameter_info.data_type) << ")" << endl;
					break;
			}
		}
	} else {
		FloatMarkerData marker_trajs = c3dfile.getMarkerTrajectories(argv[2]);

		size_t i;
		for (i = 0; i < marker_trajs.x.size(); i++) {
			cout << marker_trajs.x[i] << ",\t"
				<< marker_trajs.y[i] << ",\t"
				<< marker_trajs.z[i] << endl;
		}
	}

	std::string point_units = c3dfile.getParamString("POINT:UNITS");
	cout << "point units = " << point_units << endl;

	return 0;
}

