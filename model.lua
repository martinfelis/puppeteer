require 'HeiMan.strict'
HeiMan = require 'HeiMan.HeiMan'

heiman = HeiMan (74, 1.74)
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
joint_base = {
	{ 0., 0., 0., 1., 0., 0.},
	{ 0., 0., 0., 0., 0., 1.},
	{ 0., 1., 0., 0., 0., 0.},
}

joints = {
	hip = joint_rot_y,
	knee = joint_rot_y,
	ankle = joint_rot_y,
	lumbar = joint_fixed,
	thorax = joint_fixed,
	shoulder= joint_rot_y,
	elbow = joint_rot_y,
	wrist = joint_fixed,
	neck = joint_rot_y
}

points = {
	{ name = "right_heel", body = "foot.r", point = heiman:get_point("foot_heel") },
	{ name = "right_hallux", body = "foot.r", point = heiman:get_point("foot_hallux") },
	{ name = "left_heel", body = "foot.l", point = heiman:get_point("foot_heel") },
	{ name = "left_hallux", body = "foot.l", point = heiman:get_point("foot_hallux") },
}

model = {
	gravity = { 0., 0., -9.81},

	configuration = {
		axis_front = { 1.,  0.,  0.},
		axis_right = { 0., -1.,  0.},
		axis_up    = { 0.,  0.,  1.}
	},

	points = points,

	constraint_sets = {
		RightFlat = {
			{	point = "right_heel", normal = {1., 0., 0.}},
			{	point = "right_heel", normal = {0., 0., 1.}},
			{	point = "right_hallux", normal = {0., 0., 1.}}
		},
		RightHalx = {
			{	point = "right_hallux", normal = {1., 0., 0.}},
			{	point = "right_hallux", normal = {0., 0., 1.}}
		},
		RightHalx = {
			{	point = "right_hallux", normal = {1., 0., 0.}},
			{	point = "right_hallux", normal = {0., 0., 1.}}
		},
		RightHalxLeftHeel = {
			{	point = "right_hallux", normal = {1., 0., 0.}},
			{	point = "right_hallux", normal = {0., 0., 1.}},
			{	point = "left_heel", normal = {1., 0., 0.}},
			{	point = "left_heel", normal = {0., 0., 1.}}
		},
		RightHalxLeftFlat = {
			{	point = "right_hallux", normal = {1., 0., 0.}},
			{	point = "right_hallux", normal = {0., 0., 1.}},
			{	point = "left_heel", normal = {1., 0., 0.}},
			{	point = "left_heel", normal = {0., 0., 1.}},
			{	point = "left_hallux", normal = {0., 0., 1.}},
		},
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
			}
		},
		-- right leg
		{
			name = "thigh.r",
			parent = "pelvis",
			body = bodies.thigh,
			joint = joints.hip,
			joint_frame = {
				r = heiman:get_joint_origin ("hip.r")
			},
			visuals = {
				heiman:get_visuals ("thigh", { 1.0, 0.1, 0.1} ),
			}
		},
		{
			name = "shank.r",
			parent = "thigh.r",
			body = bodies.shank,
			joint = joints.knee,
			joint_frame = {
				r = heiman:get_joint_origin ("knee.r")
			},
			visuals = {
				heiman:get_visuals ("shank", { 1.0, 0.2, 0.2}) ,
			}
		},
		{
			name = "foot.r",
			parent = "shank.r",
			body = bodies.foot,
			joint = joints.ankle,
			joint_frame = {
				r = heiman:get_joint_origin ("ankle.r")
			},
			visuals = {
				heiman:get_visuals ("foot", {1.0, 0.2, 0.2}) ,
			}
		},
		-- left leg
		{
			name = "thigh.l",
			parent = "pelvis",
			body = bodies.thigh,
			joint = joints.hip,
			joint_frame = {
				r = heiman:get_joint_origin ("hip.l")
			},
			visuals = {
				heiman:get_visuals ("thigh", { 0.1, 1.0, 0.1}) ,
			}
		},
		{
			name = "shank.l",
			parent = "thigh.l",
			body = bodies.shank,
			joint = joints.knee,
			joint_frame = {
				r = heiman:get_joint_origin ("knee.l")
			},
			visuals = {
				heiman:get_visuals ("shank", { 0.2, 1.0, 0.2}) ,
			}
		},
		{
			name = "foot.l",
			parent = "shank.l",
			body = bodies.foot,
			joint = joints.ankle,
			joint_frame = {
				r = heiman:get_joint_origin ("ankle.l")
			},
			visuals = {
				heiman:get_visuals ("foot", { 0.2, 1.0, 0.2}) ,
			}
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
			}
		},
		-- right arm
		{
			name = "upper_arm.r",
			parent = "upper_trunk",
			body = bodies.upper_arm,
			joint = joints.shoulder,
			joint_frame = {
				r = heiman:get_joint_origin ("shoulder.r", "upper_trunk")
			},
			visuals = {
				heiman:get_visuals ("upper_arm", { 1.0, 0.1, 0.1}) ,
			}
		},
		{
			name = "lower_arm.r",
			parent = "upper_arm.r",
			body = bodies.lower_arm,
			joint = joints.elbow,
			joint_frame = {
				r = heiman:get_joint_origin ("elbow.r")
			},
			visuals = {
				heiman:get_visuals ("lower_arm", { 1.0, 0.2, 0.2}),
			}

		},
		{
			name = "hand.r",
			parent = "lower_arm.r",
			body = bodies.hand,
			joint = joints.wrist,
			joint_frame = {
				r = heiman:get_joint_origin ("wrist.r")
			},
			visuals = {
				heiman:get_visuals ("hand", { 1.0, 0.3, 0.3}),
			}
		},
		-- left arm
		{
			name = "upper_arm.l",
			parent = "upper_trunk",
			body = bodies.upper_arm,
			joint = joints.shoulder,
			joint_frame = {
				r = heiman:get_joint_origin ("shoulder.l", "upper_trunk")
			},
			visuals = {
				heiman:get_visuals ("upper_arm", { 0.1, 1.0, 0.1}) ,
			}
		},
		{
			name = "lower_arm.l",
			parent = "upper_arm.l",
			body = bodies.lower_arm,
			joint = joints.elbow,
			joint_frame = {
				r = heiman:get_joint_origin ("elbow.l")
			},
			visuals = {
				heiman:get_visuals ("lower_arm", { 0.2, 1.0, 0.2}) ,
			}
		},
		{
			name = "hand.l",
			parent = "lower_arm.l",
			body = bodies.hand,
			joint = joints.wrist,
			joint_frame = {
				r = heiman:get_joint_origin ("wrist.l")
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
			}

		}
	}
}

return model
