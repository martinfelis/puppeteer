--[
--  Segment end points for the daLeva data:
--    trunk        : CERV - MIDH
--    thigh        : HJC  - KJC
--    shank        : KJC  - AJC
--    foot         : HEEL - TTIP
--    upper_arm    : SJC  - EJC
--    lower_arm    : EJC  - WJC
--    hand         : WJC  - DAC3
--    lower_trunk  : MIDH - OMPH,
--   	middle_trunk : OMPH - XYPH,
--   	upper_trunk_alt  : XYPH - CERV,
--   	upper_trunk  : XYPH - SUPRASTERNALE,
--   	neck         : SUPRASTERNALE - GONION
--    head         : VERT - GONION
--]

-- matrix = require ("HeiMan.matrix")

local function calc_inertia_moments (radii, length, mass)
	return { 
		{(radii[1] * length)^2 * mass, 0., 0.},
		{0., (radii[2] * length)^2 * mass, 0.},
		{0., 0., (radii[3] * length)^2 * mass}}
end

local function newHeiMan (initial_mass, initial_height)
	local heiman = {}

	heiman.parameters = {
		LowerTrunkMass    = 0.1117 * initial_mass,
		ThighRightMass    = 0.1416 * initial_mass,
		ShankRightMass    = 0.0433 * initial_mass,
		FootRightMass     = 0.0137 * initial_mass,
		ThighLeftMass     = 0.1416 * initial_mass,
		ShankLeftMass     = 0.0433 * initial_mass,
		FootLeftMass      = 0.0137 * initial_mass,
		MiddleTrunkMass   = 0.1633 * initial_mass,
		UpperTrunkMass    = 0.1596 * initial_mass,
		UpperTrunkAltMass = 0.1596 * initial_mass,
		UpperArmRightMass = 0.0271 * initial_mass,
		LowerArmRightMass = 0.0162 * initial_mass,
		HandRightMass     = 0.0061 * initial_mass,
		UpperArmLeftMass  = 0.0271 * initial_mass,
		LowerArmLeftMass  = 0.0162 * initial_mass,
		HandLeftMass      = 0.0061 * initial_mass,
		NeckMass          = 0.0067 * initial_mass,
		HeadMass          = 0.0694 * initial_mass,

		ClaviculaLeftLength     = 0.1200 * initial_height, -- Source: Dumas, et.al 2007
		ClaviculaRightLength    = 0.1200 * initial_height, -- Source: Dumas, et.al 2007
		FootLeftHeight          = 0.0468 * initial_height, -- guess
		FootLeftLength          = 0.1462 * initial_height, -- guess
		FootLeftPosteriorPoint  = 0.0400 * initial_height, -- guess
		FootRightHeight         = 0.0468 * initial_height, -- guess
		FootRightLength         = 0.1462 * initial_height, -- guess
		FootRightPosteriorPoint = 0.0400 * initial_height, -- guess
		HandLeftLength          = 0.1879 / 1.741 * initial_height,
		HandRightLength         = 0.1879 / 1.741 * initial_height,
		HeadLength              = 0.2033 / 1.741 * initial_height,
		HipLeftWidth            = 0.0461 * initial_height, -- Source: Dumas, et.al 2007
		HipRightWidth           = 0.0461 * initial_height, -- Source: Dumas, et.al 2007
		LowerArmLeftLength      = 0.2689 / 1.741 * initial_height,
		LowerArmRightLength     = 0.2689 / 1.741 * initial_height,
		LowerTrunkHeight        = 0.1457 / 1.741 * initial_height,
		MiddleTrunkHeight       = 0.2155 / 1.741 * initial_height,
		NeckLength              = 0.1110 / 1.741 * initial_height,
		ShankLeftLength         = 0.4403 / 1.741 * initial_height,
		ShankRightLength        = 0.4403 / 1.741 * initial_height,
		ShoulderLeftHeight      = 0.1707 / 1.741 * initial_height, -- Suprasternale
		ShoulderRightHeight     = 0.1707 / 1.741 * initial_height, -- Suprasternale
		SuprasternaleHeight     = 0.1707 / 1.741 * initial_height,
		ThighLeftLength         = 0.4222 / 1.741 * initial_height,
		ThighRightLength        = 0.4222 / 1.741 * initial_height,
		UpperArmLeftLength      = 0.2817 / 1.741 * initial_height,
		UpperArmRightLength     = 0.2817 / 1.741 * initial_height,
		UpperTrunkHeight        = 0.1707 / 1.741 * initial_height,
		UpperTrunkAltHeight     = 0.2421 / 1.741 * initial_height,
	}

	function heiman:update_segments()
		local segments = {
			Pelvis        = { mass = self.parameters["LowerTrunkMass"],    com = { 0., 0., (1. - 0.6116) * self.parameters["LowerTrunkHeight"]}, inertia = calc_inertia_moments ({0.615, 0.551, 0.587}, self.parameters["LowerTrunkHeight"], self.parameters["LowerTrunkMass"]) },

			ThighRight    = { mass = self.parameters["ThighRightMass"],    com = { 0., 0., -0.4095 * self.parameters["ThighRightLength"]}, inertia = calc_inertia_moments ({0.329, 0.329, 0.149}, self.parameters["ThighRightLength"], self.parameters["ThighRightMass"]) },
			ShankRight    = { mass = self.parameters["ShankRightMass"],    com = { 0., 0., -0.4458 * self.parameters["ShankRightLength"]}, inertia = calc_inertia_moments ({0.255, 0.249, 0.103}, self.parameters["ShankRightLength"], self.parameters["ShankRightMass"]) },
			FootRight     = { mass = self.parameters["FootRightMass"],     com = { 0., 0., -0.4415 * self.parameters["FootRightHeight"]}, inertia = calc_inertia_moments ({0.257, 0.245, 0.124}, self.parameters["FootRightLength"], self.parameters["FootRightMass"]) },

			ThighLeft     = { mass = self.parameters["ThighLeftMass"],     com = { 0., 0., -0.4095 * self.parameters["ThighLeftLength"]}, inertia = calc_inertia_moments ({0.329, 0.329, 0.149}, self.parameters["ThighLeftLength"], self.parameters["ThighLeftMass"]) },
			ShankLeft     = { mass = self.parameters["ShankLeftMass"],     com = { 0., 0., -0.4458 * self.parameters["ShankLeftLength"]}, inertia = calc_inertia_moments ({0.255, 0.249, 0.103}, self.parameters["ShankLeftLength"], self.parameters["ShankLeftMass"]) },
			FootLeft      = { mass = self.parameters["FootLeftMass"],      com = { 0., 0., -0.4415 * self.parameters["FootLeftHeight"]}, inertia = calc_inertia_moments ({0.257, 0.245, 0.124}, self.parameters["FootLeftLength"], self.parameters["FootLeftMass"]) },

			MiddleTrunk   = { mass = self.parameters["MiddleTrunkMass"],   com = { 0., 0., (1. - 0.4502) * self.parameters["MiddleTrunkHeight"]}, inertia = calc_inertia_moments ({0.482, 0.383, 0.468}, self.parameters["MiddleTrunkHeight"], self.parameters["MiddleTrunkMass"]) },
			UpperTrunk    = { mass = self.parameters["UpperTrunkMass"],    com = { 0., 0., (1. - 0.2999) * self.parameters["UpperTrunkHeight"]}, inertia = calc_inertia_moments ({0.716, 0.454, 0.659}, self.parameters["UpperTrunkHeight"], self.parameters["UpperTrunkMass"]) },
			UpperTrunkAlt    = { mass = self.parameters["UpperTrunkAltMass"],    com = { 0., 0., (1. - 0.5066) * self.parameters["UpperTrunkAltHeight"]}, inertia = calc_inertia_moments ({0.505, 0.320, 0.465}, self.parameters["UpperTrunkAltHeight"], self.parameters["UpperTrunkAltMass"]) },

			ClaviculaRight = { mass = 0., com = { 0., 0., 0.}, inertia = calc_inertia_moments ({0., 0., 0.}, 0., 0.)},
			UpperArmRight = { mass = self.parameters["UpperArmRightMass"], com = { 0., 0., -0.5772 * self.parameters["UpperArmRightLength"]}, inertia = calc_inertia_moments ({0.285, 0.269, 0.158}, self.parameters["UpperArmRightLength"], self.parameters["UpperArmRightMass"]) },
			LowerArmRight = { mass = self.parameters["LowerArmRightMass"], com = { 0., 0., -0.4574 * self.parameters["LowerArmRightLength"]}, inertia = calc_inertia_moments ({0.276, 0.265, 0.121}, self.parameters["LowerArmRightLength"], self.parameters["LowerArmRightMass"]) },
			HandRight     = { mass = self.parameters["HandRightMass"],     com = { 0., 0., -0.3624 * self.parameters["HandRightLength"]}, inertia = calc_inertia_moments ({0.288, 0.235, 0.184}, self.parameters["HandRightLength"], self.parameters["HandRightMass"]) },

			ClaviculaLeft = { mass = 0., com = { 0., 0., 0.}, inertia = calc_inertia_moments ({0., 0., 0.}, 0., 0.)},
			UpperArmLeft  = { mass = self.parameters["UpperArmLeftMass"],  com = { 0., 0., -0.5772 * self.parameters["UpperArmLeftLength"]}, inertia = calc_inertia_moments ({0.285, 0.269, 0.158}, self.parameters["UpperArmLeftLength"], self.parameters["UpperArmLeftMass"]) },
			LowerArmLeft  = { mass = self.parameters["LowerArmLeftMass"],  com = { 0., 0., -0.4574 * self.parameters["LowerArmLeftLength"]}, inertia = calc_inertia_moments ({0.276, 0.265, 0.121}, self.parameters["LowerArmLeftLength"], self.parameters["LowerArmLeftMass"]) },
			HandLeft      = { mass = self.parameters["HandLeftMass"],      com = { 0., 0., -0.3624 * self.parameters["HandLeftLength"]}, inertia = calc_inertia_moments ({0.288, 0.235, 0.184}, self.parameters["HandLeftLength"], self.parameters["HandLeftMass"]) },

			Neck          = { mass = self.parameters["NeckMass"],          com = { 0., 0.,  0.5000 * self.parameters["NeckLength"]}, inertia = calc_inertia_moments ({0.276, 0.265, 0.121}, self.parameters["NeckLength"], self.parameters["NeckMass"]) },
			Head          = { mass = self.parameters["HeadMass"],          com = { 0., 0., (1. - 0.5976) * self.parameters["HeadLength"]}, inertia = calc_inertia_moments ({0.362, 0.376, 0.312}, self.parameters["HeadLength"], self.parameters["HeadMass"]) },
		}

		self.segments = segments
	end

	function heiman:update_joints()
		local rot_yxz = {
			{ 0., 1., 0., 0., 0., 0.},
			{ 1., 0., 0., 0., 0., 0.},
			{ 0., 0., 1., 0., 0., 0.},
		}
		local freeflyer = {
			{ 0., 0., 0., 1., 0., 0.},
			{ 0., 0., 0., 0., 1., 0.},
			{ 0., 0., 0., 0., 0., 1.},
			{ 0., 1., 0., 0., 0., 0.},
			{ 1., 0., 0., 0., 0., 0.},
			{ 0., 0., 1., 0., 0., 0.},
		}
		local rot_y = {
			{ 0., 1., 0., 0., 0., 0.},
		}

		local joints = {
			Pelvis        = { translation = {     0.,     0.,    0. },                                 dofs = freeflyer },

			HipRight      = { translation = {     0.,  -self.parameters["HipRightWidth"],    0.   },   dofs = rot_yxz }, -- parent: Pelvis
			KneeRight     = { translation = {     0.,  0.,    -self.parameters["ThighRightLength"]},   dofs = rot_y },   -- parent: ThighRight
			AnkleRight    = { translation = {     0.,  0.,    -self.parameters["ShankRightLength"]},   dofs = rot_yxz }, -- parent: ShankRight

			HipLeft       = { translation = {     0.,  self.parameters["HipLeftWidth"],    0.    },    dofs = rot_yxz }, -- parent: Pelvis
			KneeLeft      = { translation = {     0.,  0.,    -self.parameters["ThighLeftLength"] },   dofs = rot_y },   -- parent: ThighLeft
			AnkleLeft     = { translation = {     0.,  0.,    -self.parameters["ShankLeftLength"] },   dofs = rot_yxz }, -- parent: ShankLeft

			LumbarJoint   = { translation = { 0., 0., self.parameters["LowerTrunkHeight"]},            dofs = rot_yxz }, -- parent: Pelvis
			ThoraxJoint   = { translation = { 0., 0., self.parameters["MiddleTrunkHeight"]},           dofs = {} },      -- parent: MiddleTrunk

			ClaviculaRight = { translation = {0., 0., self.parameters["ShoulderRightHeight"]},         dofs = {} }, -- parent: UpperTrunkAlt
			ShoulderRight = { translation = { 0., -self.parameters["ClaviculaRightLength"], 0.},       dofs = rot_yxz }, -- parent: ClavculaRight
			ElbowRight    = { translation = { 0., 0., -self.parameters["UpperArmRightLength"]},        dofs = rot_y },   -- parent: UpperArmRight
			WristRight    = { translation = { 0., 0., -self.parameters["LowerArmRightLength"]},        dofs = {} },      -- parent: LowerArmRight

			ClaviculaLeft = { translation = { 0., 0., self.parameters["ShoulderLeftHeight"]},          dofs = {} }, -- parent: UpperTrunkAlt
			ShoulderLeft  = { translation = { 0., self.parameters["ClaviculaLeftLength"], 0.},         dofs = rot_yxz }, -- parent: ClaviculaLeft
			ElbowLeft     = { translation = { 0., 0., -self.parameters["UpperArmLeftLength"]},         dofs = rot_y },   -- parent: UpperArmLeft
			WristLeft     = { translation = { 0., 0., -self.parameters["LowerArmLeftLength"]},         dofs = {} },      -- parent: LowerArmLeft

			Neck          = { translation = {0., 0., self.parameters["SuprasternaleHeight"]},          dofs = {} }, -- parent: UpperTrunkAlt
			Head          = { translation = {0., 0., self.parameters["NeckLength"]},                   dofs = rot_yxz }, -- parent: Neck
		}

		self.joints = joints
	end

	function heiman:update_visuals()
		local upper_leg_width = 0.12
		local upper_leg_depth = 0.15

		local lower_leg_width = 0.1
		local lower_leg_depth = 0.13

		local pelvis_depth = 0.18
		local pelvis_width = (self.parameters["HipRightWidth"] + self.parameters["HipLeftWidth"]) + upper_leg_width

		local clavicula_depth = 0.06
		local clavicula_height = 0.06

		local upper_arm_width = 0.09
		local upper_arm_depth = 0.12

		local lower_arm_width = 0.08
		local lower_arm_depth = 0.10

		local hand_width = 0.04
		local hand_depth = 0.10

		local head_width = 0.18
		local head_depth = 0.20
		local head_length_scaling = 1.1

		local neck_width = 0.07
		local neck_depth = 0.07

		local upper_trunk_width = self.parameters["ClaviculaRightLength"] + self.parameters["ClaviculaLeftLength"] * 0.7
		local upper_trunk_depth = pelvis_depth * 1.3

		local middle_trunk_width = pelvis_width * 0.5 + upper_trunk_width * 0.5
		local middle_trunk_depth = pelvis_depth * 1.2

		local visuals = {
			Pelvis        = { color = { 0.2, 0.2, 0.9}, dimensions = { pelvis_depth, pelvis_width, self.parameters["LowerTrunkHeight"]}, mesh_center = { 0., -self.parameters["HipRightWidth"] + 0.5 * (pelvis_width - upper_leg_width) , self.parameters["LowerTrunkHeight"] * 0.5, }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },

			ThighRight    = { color = { 1., 0.1, 0.1},  dimensions = { upper_leg_depth, upper_leg_width, self.parameters["ThighRightLength"]}, mesh_center = { 0., 0.,  -self.parameters["ThighRightLength"] * 0.5 }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },
			ShankRight    = { color = { 0.9, 0.3, 0.3}, dimensions = { lower_leg_depth, lower_leg_width, self.parameters["ShankRightLength"]}, mesh_center = { 0., 0., -self.parameters["ShankRightLength"] * 0.5 }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },
			FootRight     = { color = { 1., 0.1, 0.1},  dimensions = { self.parameters["FootRightLength"], lower_leg_width, self.parameters["FootRightHeight"]}, mesh_center = { self.parameters["FootRightLength"] * 0.5 - self.parameters["FootRightPosteriorPoint"], 0., -self.parameters["FootRightHeight"] * 0.5, }, geometry = { capsule = { radius = 0.09, length = 0.9, segments=48 } } },

			ThighLeft     = { color = { 0.1, 1., 0.1},  dimensions = { upper_leg_depth, upper_leg_width, self.parameters["ThighLeftLength"]}, mesh_center = { 0., 0.,  -self.parameters["ThighLeftLength"] * 0.5 }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },
			ShankLeft     = { color = { 0.3, 0.9, 0.3}, dimensions = { lower_leg_depth, lower_leg_width, self.parameters["ShankLeftLength"]}, mesh_center = { 0., 0., -self.parameters["ShankLeftLength"] * 0.5 }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },
			FootLeft      = { color = { 0.1, 1., 0.1},  dimensions = { self.parameters["FootLeftLength"], lower_leg_width, self.parameters["FootLeftHeight"]}, mesh_center = { self.parameters["FootLeftLength"] * 0.5 - self.parameters["FootLeftPosteriorPoint"], 0., -self.parameters["FootLeftHeight"] * 0.5, }, geometry = { capsule = { radius = 0.09, length = 0.9, segments=48 } } },

			MiddleTrunk   = { color = { 0.3, 0.3, 1.},  dimensions = { middle_trunk_depth, middle_trunk_width, self.parameters["MiddleTrunkHeight"]}, mesh_center = { 0., 0., 0.5 * self.parameters["MiddleTrunkHeight"], }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },
			UpperTrunk    = { color = { 0.1, 0.1, 1.},  dimensions = { upper_trunk_depth, upper_trunk_width, self.parameters["UpperTrunkHeight"]}, mesh_center = { 0., 0., 0.5 * self.parameters["UpperTrunkHeight"], }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },
			UpperTrunkAlt = { color = { 0.1, 0.1, 1.},  dimensions = { upper_trunk_depth, upper_trunk_width, self.parameters["UpperTrunkAltHeight"]}, mesh_center = { 0., 0., 0.5 * self.parameters["UpperTrunkAltHeight"], }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },

			ClaviculaRight = { color = { 0.3, 0.3, 1.}, dimensions = { clavicula_depth, clavicula_height, self.parameters["ClaviculaRightLength"]}, rotate = { axis = { 1., 0., 0.}, angle = 90.}, mesh_center = { 0., - 0.5 * self.parameters["ClaviculaLeftLength"], 0.}, geometry = { capsule = { radius = 0.09, length = 0.9 } }},
			UpperArmRight = { color = { 1., 0.1, 0.1},  dimensions = { upper_arm_depth, upper_arm_width, self.parameters["UpperArmRightLength"]}, mesh_center = { 0., 0., - 0.5 * self.parameters["UpperArmRightLength"], }, geometry = { capsule = { radius = 0.12, length = 0.88 } } },
			LowerArmRight = { color = { 1., 0.3, 0.3},  dimensions = { lower_arm_depth, lower_arm_width, self.parameters["LowerArmRightLength"]}, mesh_center = { 0., 0., - 0.5 * self.parameters["LowerArmRightLength"], }, geometry = { capsule = { radius = 0.12, length = 0.88 } } },
			HandRight     = { color = { 1., 0.1, 0.1},  dimensions = { hand_depth, hand_width, self.parameters["HandRightLength"]}, mesh_center = { 0., 0., - 0.5 * self.parameters["HandRightLength"], }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },

			ClaviculaLeft = { color = { 0.3, 0.3, 1.},  dimensions = { clavicula_depth, clavicula_height, self.parameters["ClaviculaLeftLength"]}, rotate = { axis = { 1., 0., 0.}, angle = 90.}, mesh_center = { 0., 0.5 * self.parameters["ClaviculaLeftLength"], 0.}, geometry = { capsule = { radius = 0.09, length = 0.9 } }},
			UpperArmLeft  = { color = { 0.1, 1., 0.1},  dimensions = { upper_arm_depth, upper_arm_width, self.parameters["UpperArmLeftLength"]}, mesh_center = { 0., 0., - 0.5 * self.parameters["UpperArmLeftLength"], }, geometry = { capsule = { radius = 0.12, length = 0.88 } } },
			LowerArmLeft  = { color = { 0.3, 0.9, 0.3}, dimensions = { lower_arm_depth, lower_arm_width, self.parameters["LowerArmLeftLength"]}, mesh_center = { 0., 0., - 0.5 * self.parameters["LowerArmLeftLength"], }, geometry = { capsule = { radius = 0.12, length = 0.88 } } },
			HandLeft      = { color = { 0.1, 1., 0.1},  dimensions = { hand_depth, hand_width, self.parameters["HandLeftLength"]}, mesh_center = { 0., 0., - 0.5 * self.parameters["HandLeftLength"], }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },

			Neck          = { color = { 0.2, 0.2, 0.9}, dimensions = { neck_depth, neck_width, self.parameters["NeckLength"]}, mesh_center = { 0., 0., 0.5 * self.parameters["NeckLength"], }, geometry = { capsule = { radius = 0.09, length = 0.9 } } },
			Head          = { color = { 0.1, 0.1, 0.9}, dimensions = { head_depth, head_width, self.parameters["HeadLength"] * head_length_scaling}, mesh_center = { 0., 0., (1 - 0.5 * head_length_scaling) * self.parameters["HeadLength"], }, geometry = { capsule = { radius = 0.20, length = 0.8 } } },
		}

		self.visuals = visuals
	end

	function heiman:create_model ()
		self:update_segments()
		self:update_joints()
		self:update_visuals()

		local model = {}
		model.parameters = self.parameters

		local joint_visual = { color = { 0.7, 0.7, 0.7}, dimensions = { 0.08, 0.08, 0.08}, geometry = { sphere = { radius = 1. } } }
		joint_visual = nil

		local function create_frame (name, parent, segment, joint, visual) 
			local frame = {}

			frame.name = name
			frame.parent = parent
			frame.body = segment
			frame.joint = joint.dofs
			frame.joint_frame = { r = joint.translation }

			assert (visual ~= nil, "No visual defined for frame '" .. name .. "'!")
			frame.visuals = { 
				visual,
				joint_visual
			}

			return frame
		end

		model.gravity = { 0., 0., -9.81 }
		model.configuration = {
			axis_front = { 1.,  0.,  0.},
			axis_right = { 0., -1.,  0.},
			axis_up    = { 0.,  0.,  1.}
		}
		model.frames = {
			create_frame ( "Pelvis", "ROOT", self.segments["Pelvis"], self.joints["Pelvis"], self.visuals["Pelvis"]),

			create_frame ( "ThighRight", "Pelvis", self.segments["ThighRight"], self.joints["HipRight"], self.visuals["ThighRight"]),
			create_frame ( "ShankRight", "ThighRight", self.segments["ShankRight"], self.joints["KneeRight"], self.visuals["ShankRight"]),
			create_frame ( "FootRight", "ShankRight", self.segments["FootRight"], self.joints["AnkleRight"], self.visuals["FootRight"]),

			create_frame ( "ThighLeft", "Pelvis", self.segments["ThighLeft"], self.joints["HipLeft"], self.visuals["ThighLeft"]),
			create_frame ( "ShankLeft", "ThighLeft", self.segments["ShankLeft"], self.joints["KneeLeft"], self.visuals["ShankLeft"]),
			create_frame ( "FootLeft", "ShankLeft", self.segments["FootLeft"], self.joints["AnkleLeft"], self.visuals["FootLeft"]),

			create_frame ( "MiddleTrunk", "Pelvis", self.segments["MiddleTrunk"], self.joints["LumbarJoint"], self.visuals["MiddleTrunk"]),
			create_frame ( "UpperTrunk", "MiddleTrunk", self.segments["UpperTrunk"], self.joints["ThoraxJoint"], self.visuals["UpperTrunk"]),
--			create_frame ( "UpperTrunkAlt", "MiddleTrunk", self.segments["UpperTrunkAlt"], self.joints["ThoraxJoint"], self.visuals["UpperTrunkAlt"]),

			create_frame ( "ClaviculaRight", "UpperTrunk", self.segments["ClaviculaRight"], self.joints["ClaviculaRight"], self.visuals["ClaviculaRight"]),
			create_frame ( "UpperArmRight", "ClaviculaRight", self.segments["UpperArmRight"], self.joints["ShoulderRight"], self.visuals["UpperArmRight"]),
			create_frame ( "LowerArmRight", "UpperArmRight", self.segments["LowerArmRight"], self.joints["ElbowRight"], self.visuals["LowerArmRight"]),
			create_frame ( "HandRight", "LowerArmRight", self.segments["HandRight"], self.joints["WristRight"], self.visuals["HandRight"]),

			create_frame ( "ClaviculaLeft", "UpperTrunk", self.segments["ClaviculaLeft"], self.joints["ClaviculaLeft"], self.visuals["ClaviculaLeft"]),
			create_frame ( "UpperArmLeft", "ClaviculaLeft", self.segments["UpperArmLeft"], self.joints["ShoulderLeft"], self.visuals["UpperArmLeft"]),
			create_frame ( "LowerArmLeft", "UpperArmLeft", self.segments["LowerArmLeft"], self.joints["ElbowLeft"], self.visuals["LowerArmLeft"]),
			create_frame ( "HandLeft", "LowerArmLeft", self.segments["HandLeft"], self.joints["WristLeft"], self.visuals["HandLeft"]),

			create_frame ( "Neck", "UpperTrunk", self.segments["Neck"], self.joints["Neck"], self.visuals["Neck"]),
			create_frame ( "Head", "Neck", self.segments["Head"], self.joints["Head"], self.visuals["Head"]),
		}

		return model
	end

	return heiman;
end

return newHeiMan
