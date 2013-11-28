require 'HeiMan.strict'
HeiMan = require 'HeiMan.HeiMan'

-- create actual HeiMan model
heiman = HeiMan (74, 1.79)
heiman:refresh()

bodies = {
	pelvis = heiman:get_segment ("lower_trunk"),
	middle_trunk = heiman:get_segment ("middle_trunk"),
	upper_trunk = heiman:get_segment ("upper_trunk"),
	head = heiman:get_segment ("head"),
	thigh = heiman:get_segment ("thigh"),
	shank = heiman:get_segment ("shank"),
	foot = heiman:get_segment ("foot"),
	upper_arm = heiman:get_segment ("upper_arm"),
	lower_arm = heiman:get_segment ("lower_arm"),
	hand = heiman:get_segment ("hand"),
}

joint_fixed = {}
joint_rot_y = {
	{ 0., 1., 0., 0., 0., 0.},
}
joint_rot_yxz = {
	{ 0., 1., 0., 0., 0., 0.},
	{ 0., 0., 1., 0., 0., 0.},
	{ 1., 0., 0., 0., 0., 0.},
}
joint_base = {
	{ 0., 0., 0., 1., 0., 0.},
	{ 0., 0., 0., 0., 1., 0.},
	{ 0., 0., 0., 0., 0., 1.},
	{ 0., 1., 0., 0., 0., 0.},
	{ 0., 0., 1., 0., 0., 0.},
	{ 1., 0., 0., 0., 0., 0.},
}

joints = {
	hip = joint_rot_yxz,
	knee = joint_rot_y,
	ankle = joint_rot_yxz,
	lumbar = joint_rot_yxz,
	thorax = joint_fixed,
	shoulder= joint_rot_yxz,
	elbow = joint_rot_y,
	wrist = joint_fixed,
	neck = joint_rot_yxz
}

model = {
	gravity = { 0., 0., -9.81},

	configuration = {
		axis_front = { 1.,  0.,  0.},
		axis_right = { 0., -1.,  0.},
		axis_up    = { 0.,  0.,  1.}
	},

	frames = {
		{
			name = "pelvis",
			parent = "ROOT",
			body = bodies.pelvis,
			joint = joint_base,
			joint_frame = {
				r = heiman:get_joint_origin ("pelvis")
			},
			visuals = {
				heiman:get_visuals ("lower_trunk", { 0.2, 0.2, 0.9})
			},
		},
		-- right leg
		{
			name = "thigh_r",
			parent = "pelvis",
			body = bodies.thigh,
			joint = joints.hip,
			joint_frame = {
				r = heiman:get_joint_origin ("hip_r")
			},
			visuals = {
				heiman:get_visuals ("thigh", { 1.0, 0.1, 0.1} ),
			}
		},
		{
			name = "shank_r",
			parent = "thigh_r",
			body = bodies.shank,
			joint = joints.knee,
			joint_frame = {
				r = heiman:get_joint_origin ("knee_r")
			},
			visuals = {
				heiman:get_visuals ("shank", { 1.0, 0.2, 0.2}) ,
			},
		},
		{
			name = "foot_r",
			parent = "shank_r",
			body = bodies.foot,
			joint = joints.ankle,
			joint_frame = {
				r = heiman:get_joint_origin ("ankle_r")
			},
			visuals = {
				heiman:get_visuals ("foot", {1.0, 0.2, 0.2}) ,
			},
		},
		-- left leg
		{
			name = "thigh_l",
			parent = "pelvis",
			body = bodies.thigh,
			joint = joints.hip,
			joint_frame = {
				r = heiman:get_joint_origin ("hip_l")
			},
			visuals = {
				heiman:get_visuals ("thigh", { 0.1, 1.0, 0.1}) ,
			}
		},
		{
			name = "shank_l",
			parent = "thigh_l",
			body = bodies.shank,
			joint = joints.knee,
			joint_frame = {
				r = heiman:get_joint_origin ("knee_l")
			},
			visuals = {
				heiman:get_visuals ("shank", { 0.2, 1.0, 0.2}) ,
			},
		},
		{
			name = "foot_l",
			parent = "shank_l",
			body = bodies.foot,
			joint = joints.ankle,
			joint_frame = {
				r = heiman:get_joint_origin ("ankle_l")
			},
			visuals = {
				heiman:get_visuals ("foot", { 0.2, 1.0, 0.2}) ,
			},
		},
		-- trunk
		{
			name = "middle_trunk",
			parent = "pelvis",
			body = bodies.middle_trunk,
			joint = joints.lumbar,
			joint_frame = {
				r = heiman:get_joint_origin ("lumbar")
			},
			visuals = {
				heiman:get_visuals ("middle_trunk", { 0.2, 1.0, 0.2}) ,
			}
		},
		{
			name = "upper_trunk",
			parent = "middle_trunk",
			body = bodies.upper_trunk,
			joint = joints.thorax,
			joint_frame = {
				r = heiman:get_joint_origin ("thorax")
			},
			visuals = {
				heiman:get_visuals ("upper_trunk", { 0.2, 1.0, 0.2}) ,
			},
		},
		-- right arm
		{
			name = "upper_arm_r",
			parent = "upper_trunk",
			body = bodies.upper_arm,
			joint = joints.shoulder,
			joint_frame = {
				r = heiman:get_joint_origin ("shoulder_r", "upper_trunk")
			},
			visuals = {
				heiman:get_visuals ("upper_arm", { 1.0, 0.1, 0.1}) ,
			}	
		},
		{
			name = "lower_arm_r",
			parent = "upper_arm_r",
			body = bodies.lower_arm,
			joint = joints.elbow,
			joint_frame = {
				r = heiman:get_joint_origin ("elbow_r")
			},
			visuals = {
				heiman:get_visuals ("lower_arm", { 1.0, 0.2, 0.2}),
			},
		},
		{
			name = "hand_r",
			parent = "lower_arm_r",
			body = bodies.hand,
			joint = joints.wrist,
			joint_frame = {
				r = heiman:get_joint_origin ("wrist_r")
			},
			visuals = {
				heiman:get_visuals ("hand", { 1.0, 0.3, 0.3}),
			}
		},
		-- left arm
		{
			name = "upper_arm_l",
			parent = "upper_trunk",
			body = bodies.upper_arm,
			joint = joints.shoulder,
			joint_frame = {
				r = heiman:get_joint_origin ("shoulder_l", "upper_trunk")
			},
			visuals = {
				heiman:get_visuals ("upper_arm", { 0.1, 1.0, 0.1}) ,
			}
		},
		{
			name = "lower_arm_l",
			parent = "upper_arm_l",
			body = bodies.lower_arm,
			joint = joints.elbow,
			joint_frame = {
				r = heiman:get_joint_origin ("elbow_l")
			},
			visuals = {
				heiman:get_visuals ("lower_arm", { 0.2, 1.0, 0.2}) ,
			},
		},
		{
			name = "hand_l",
			parent = "lower_arm_l",
			body = bodies.hand,
			joint = joints.wrist,
			joint_frame = {
				r = heiman:get_joint_origin ("wrist_l")
			},
			visuals = {
				heiman:get_visuals ("hand", { 0.3, 1.0, 0.3}) ,
			}
		},
		-- head
		{
			name = "head",
			parent = "upper_trunk",
			body = bodies.head,
			joint = joints.neck,
			joint_frame = {
				 r = heiman:get_joint_origin ("neck", "upper_trunk")
			},
			visuals = {
				heiman:get_visuals ("head", { 0.1, 0.1, 1.0}) ,
			},
		}
	}
}

return model
