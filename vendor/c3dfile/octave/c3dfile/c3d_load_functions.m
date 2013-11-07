function c3d_load_functions()

addpath ("./");
curpath = pwd;
octfile = strcat (pwd, "/", "c3d_init.oct");

autoload (              "setval", octfile);
autoload (              "getval", octfile);
autoload (            "c3d_open", octfile);
autoload (   "c3d_get_marker_id", octfile);
autoload ( "c3d_get_marker_list", octfile);
autoload ("c3d_get_marker_trajs", octfile);

