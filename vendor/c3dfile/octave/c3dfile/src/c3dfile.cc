#include <c3dfile.h>
#include <c3dutils.h>

#include <octave/oct.h>
#include <octave/parse.h>
#include <octave/str-vec.h>

static bool c3dfile_opened = false;
C3DFile c3dfile;

DEFUN_DLD(c3d_get_marker_id, args, nargout, 
		"c3d_get_marker_id (<marker_name>)") {
	octave_value_list retval;

	if (!c3dfile_opened) {
		octave_stdout << "You must call c3d_open before calling this function!\n";
		return retval;
	}

	charMatrix ch = args(0).char_matrix_value();
	std::string marker_name = ch.row_as_string(0);

	int marker_id = c3dfile.label_point_map[marker_name];

	retval(0) = marker_id;

	return retval;
}

DEFUN_DLD(c3d_get_marker_list, args, nargout, 
		"c3d_get_marker_list ()") {
	octave_value_list retval;

	if (!c3dfile_opened) {
		octave_stdout << "You must call c3d_open before calling this function!\n";
		return retval;
	}

	std::vector<std::string> marker_list = c3dfile.point_label;

	int i;
	string_vector result (marker_list.size());

	for (i = 0; i < marker_list.size(); i++) {
		result(i) = marker_list.at(i);
	}

	retval(0) = result;

	return retval;
}

DEFUN_DLD(c3d_get_marker_trajs, args, nargout, 
		"c3d_get_marker_trajs (<marker name>)") {
	int nargin = args.length();
	octave_value_list retval;

	if (!c3dfile_opened) {
		octave_stdout << "You must call c3d_open before calling this function!\n";
		return retval;
	}

	if (nargin != 1) {
		octave_stdout << "No marker name given!\n";
		return retval;
	}

	if (!args(0).is_char_matrix()) {
		octave_stdout << "string argument required!" << "\n";
		return retval;
	}

	charMatrix ch = args(0).char_matrix_value();
	std::string marker_name = ch.row_as_string(0);

	FloatMarkerData traj_data = c3dfile.getMarkerTrajectories(marker_name.c_str());
	unsigned int rows = traj_data.x.size();

	Matrix trajs(rows, 3);

	octave_idx_type i,j;
	for (i = 0; i < rows; i++) {
		trajs(i,0) = traj_data.x.at(i);
		trajs(i,1) = traj_data.y.at(i);
		trajs(i,2) = traj_data.z.at(i);
	}

	retval(0) = trajs;

	return retval;
}

DEFUN_DLD (c3d_open, args, nargout, "Opens a c3d file") {
	int nargin = args.length();
	octave_value_list retval;

	if (nargin < 1) {
		octave_stdout << "Usage: c3d_open (<filename>)" << "\n";
		return retval;
	}

	if (!args(0).is_char_matrix()) {
		octave_stdout << "string argument required!" << "\n";
		return retval;
	}

	charMatrix ch = args(0).char_matrix_value();

	std::string filename = ch.row_as_string(0);

	if (c3dfile.load (filename.c_str())) {
		c3dfile_opened = true;
		octave_stdout << "File " << filename << " loaded successfully.\n";
	} else {
		octave_stdout << "Error opening " << filename << ".\n";
	}

	return retval;
}

DEFUN_DLD (c3d_init, args, nargout, "Initializes c3dread") {
	int nargin = args.length();
	octave_value_list retval;

	int parse_status = 0;
	retval = eval_string("c3d_load_functions()", false, parse_status);

	return retval;
}
