--[
--  Segment end points for the daLeva data:
--    head         : VERT - CERV
--    trunk        : CERV - MIDH
--    thigh        : HJC  - KJC
--    shank        : KJC  - AJC
--    foot         : HEEL - TTIP
--    upper_arm    : SJC  - EJC
--    lower_arm    : EJC  - WJC
--    hand         : WJC  - MET3
--    lower_trunk  : MIDH - OMPH,
--   	middle_trunk : OMPH - XYPH,
--   	upper_trunk  : XYPH - CERV,
--]

-- matrix = require ("HeiMan.matrix")

local function newHeiMan (model_mass, model_height)
	local heiman = {

		parameters = {
			mass = model_mass,
			height = model_height,

			heel_posterior_point_rel      = -(0.05 / 1.741), -- estimate of the amount the foot extends along the negative saggital axis
			foot_width_relative_to_height = 0.055, -- Source: Winter "Biomechanics and Motor Control...", 2009
			foot_length_rel               = 0.25 / 1.741, -- TODO guess
			shoulder_width_rel            = 0.2401, -- Source: Dumas, Cheze, Verriest, 2007
			hip_width_rel                 = 0.093, -- Source: Dumas, Cheze, Verriest, 2007

			segment_lengths_rel = {
				trunk        = 0.3055,
				thigh        = 0.2425,
				shank        = 0.2529,
				foot         = 0.0182,
				upper_arm    = 0.1618,
				lower_arm    = 0.1544,
				hand         = 0.0495,
				head         = 0.1395,
				lower_trunk  = 0.1457 / 1.741,
				middle_trunk = 0.2155 / 1.741,
				upper_trunk  = 0.2421 / 1.741,
			},

			segment_masses_rel = {
				trunk        = 0.4346,
				thigh        = 0.1416,
				shank        = 0.0433,
				foot         = 0.0137,
				upper_arm    = 0.0271,
				lower_arm    = 0.0162,
				hand         = 0.0061,
				head         = 0.0694,
				lower_trunk  = 0.1117,
				middle_trunk = 0.1633,
				upper_trunk  = 0.1596,
			},

			segment_com_rel = {
				trunk        = {0., 0., 0.5138},
				thigh        = {0., 0., -0.4095},
				shank        = {0., 0., -0.4458},
				foot         = {0.4, 0., -0.4415}, -- TODO x coordinate is a wild guess
				upper_arm    = {0., 0., -0.5772},
				lower_arm    = {0., 0., -0.4574},
				hand         = {0., 0., -0.7900},
				head         = {0., 0., 0.5002},
				lower_trunk  = {0., 0., 0.6115},
				middle_trunk = {0., 0., 0.4502},
				upper_trunk  = {0., 0., 0.5066}
			},

			segment_gyration_radii_rel = {
				trunk        = { 0.328, 0.306, 0.169},
				thigh        = { 0.329, 0.329, 0.149},
				shank        = { 0.255, 0.249, 0.103},
				foot         = { 0.257, 0.245, 0.124},
				upper_arm    = { 0.285, 0.269, 0.158},
				lower_arm    = { 0.276, 0.265, 0.121}, 
				hand         = { 0.628, 0.513, 0.401}, 
				head         = { 0.303, 0.315, 0.261}, 
				lower_trunk  = { 0.615, 0.551, 0.587}, 
				middle_trunk = { 0.482, 0.383, 0.468}, 
				upper_trunk  = { 0.505, 0.320, 0.465}, 
			}
		}
	}

	-- refreshes variables that are dependent on parameters. This function
	-- has to be called whenever a value in self.parameters has changed.
	function heiman:refresh()
		local segment_lengths_rel = self.parameters.segment_lengths_rel

		self.joint_loc_rel = {
			hip      = {0., - self.parameters.hip_width_rel * 0.5, 0.},
			knee     = { 0., 0., - segment_lengths_rel.thigh},
			ankle    = { 0., 0., - segment_lengths_rel.shank},
			lumbar   = { 0., 0., segment_lengths_rel.lower_trunk},
			thorax   = { 0., 0., segment_lengths_rel.middle_trunk},
			shoulder = { 0., - self.parameters.shoulder_width_rel * 0.5, segment_lengths_rel.upper_trunk},
			elbow    = { 0., 0., - segment_lengths_rel.upper_arm },
			wrist    = { 0., 0., - segment_lengths_rel.lower_arm },
			neck     = { 0., 0., segment_lengths_rel.upper_trunk }
		}
	end

	function heiman:create_segment (segment_name)
		local segment = {}
		segment.length = self.parameters.segment_lengths_rel[segment_name] * self.parameters.height
		
		if segment_name == "foot" then
			segment.length = self.parameters.foot_length_rel * self.parameters.height
		end

		local com = {
			self.parameters.segment_com_rel[segment_name][1] * segment.length,
			self.parameters.segment_com_rel[segment_name][2] * segment.length,
			self.parameters.segment_com_rel[segment_name][3] * segment.length
		}

		local gyration_radii = self.parameters.segment_gyration_radii_rel[segment_name]
		local model_mass = self.parameters.mass

		segment.name = segment_name
		segment.com = com
		segment.mass = self.parameters.segment_masses_rel[segment_name] * model_mass

		-- absolute radii of gyration
		local gr = {
			gyration_radii[1] * segment.length,
			gyration_radii[2] * segment.length,
			gyration_radii[3] * segment.length
		}

		segment.inertia = {
			{ gr[1], 0., 0., },
			{ 0., gr[2], 0., },
			{ 0., 0., gr[3]  }
		}

		--print ("create_segment name = '" .. segment_name .. "' mass = " .. segment.mass .. " com: ", com[1], com[2], com[3], " length = " .. segment.length, "gr = ", gr[1], gr[2], gr[3])

		return segment
	end

	function heiman:create_visual (segment_name, color, mesh_name)
		color = color or {1., 0., 0.}
		mesh_name = mesh_name or "meshes/unit_cube.obj"

		local visual = {}
		local parameters = self.parameters
		local height = self.parameters.height
		local default_width = height * 0.08

		if parameters.segment_lengths_rel[segment_name] == nil then
			error ("Error: cannot create visual for segment '" .. segment_name .. "'!'")
		end

		visual = {
			name = segment_name,
			dimensions = { 
				default_width,
				default_width,
				parameters.segment_lengths_rel[segment_name] * height},
				color = color,
				mesh_center = { 0, 0, -parameters.segment_lengths_rel[segment_name] * height * 0.5},
				src = mesh_name,
			}
		return visual
	end

	function heiman:get_visuals (segment_name, color)
		color = color or {1., 0., 0.}
		local parameters = self.parameters
		local height = self.parameters.height

		local visual = self:create_visual (segment_name, color)

		if segment_name == "lower_trunk" then
			visual.dimensions[2] = parameters.hip_width_rel * height * 2.4
			visual.mesh_center[3] = parameters.segment_lengths_rel.lower_trunk * height * 0.5
		elseif segment_name == "middle_trunk" then
			visual.dimensions[2] = parameters.hip_width_rel * height * 2.6 
			visual.mesh_center[3] = parameters.segment_lengths_rel.middle_trunk * height * 0.5
		elseif segment_name == "upper_trunk" then
			visual.dimensions[2] = parameters.hip_width_rel * height * 2.8 
			visual.mesh_center[3] = parameters.segment_lengths_rel.upper_trunk * height * 0.5
		elseif segment_name == "foot" then
			visual.dimensions[1] = parameters.foot_length_rel * height
			visual.dimensions[2] = parameters.foot_width_relative_to_height * height * 2.
			visual.mesh_center[1] = - parameters.heel_posterior_point_rel * height
		elseif segment_name == "head" then
			visual.mesh_center[3] = parameters.segment_lengths_rel.head * height * 0.5
		end

		return visual
	end

	function heiman:get_point (point_name)
		local x, y, z
		if (point_name == "foot_heel") then
			local visual = self:get_visuals ("foot")
			x = visual.mesh_center[1] - visual.dimensions[1] * 0.5
			y = 0. 
			z = visual.mesh_center[3] - visual.dimensions[3] * 0.5

		elseif (point_name == "foot_hallux") then
			local visual = self:get_visuals ("foot")
			x = visual.mesh_center[1] + visual.dimensions[1] * 0.5
			y = 0. 
			z = visual.mesh_center[3] - visual.dimensions[3] * 0.5
		else
			error ("Unknown point '" .. point_name .. "'!")
		end

		return {x, y, z}
	end

	function heiman:get_segment (segment_name)
		-- print ("heiman:get_segment (" .. segment_name .. ")")
		local segment = self:create_segment (segment_name)
		return segment
	end

	function heiman:get_joint_origin (joint_name, parent_segment_name)
		-- print ("heiman:get_joint_origin (" .. joint_name .. ", " .. tostring(parent_segment_name) .. ")")
		parent_segment_name = parent_segment_name or "unknown"
		local side_identifier = ""
		local full_joint_name = joint_name

		if string.find(joint_name, "%.") then
			side_identifier = string.lower(string.sub(joint_name, string.find(joint_name, "%.") + 1, -1))
			joint_name = string.sub(joint_name, 1, string.find(joint_name, "%.") - 1)
		end

		if joint_name == "pelvis" then
			local height = self.parameters.height
			local segment_lengths_rel = self.parameters.segment_lengths_rel

			return {
				0,
				0,
				0,
			}
		end

		-- warning: we have to copy the values otherwise we modify joint_loc_rel!
		local joint_loc = self.joint_loc_rel[joint_name]
		local joint_origin_rel = {joint_loc[1], joint_loc[2], joint_loc[3]}

		if joint_name == "neck" then
			if parent_segment_name == "unknown" then
				error ("You need to specify the parent segment name for the joint " .. joint_name .. "!")
			elseif parent_segment_name == "trunk" then
				-- we need to add heights of lower and upper trunk
				joint_origin_rel[3] = joint_origin_rel[3] + (segment_lengths_rel.lower_trunk + segment_lengths_rel.middle_trunk)
			elseif parent_segment_name == "upper_trunk" then
				-- no action required
			else
				error ("You need to specify joint 'neck' relative to either 'trunk' or 'upper_trunk'")
			end
		elseif joint_name == "shoulder" then
			if parent_segment_name == "unknown" then
				error ("You need to specify the parent segment name for the joint " .. joint_name .. "!")
			elseif parent_segment_name == "trunk" then
				joint_origin_rel[3] = joint_origin_rel[3] + (segment_lengths_rel.lower_trunk + segment_lengths_rel.middle_trunk)
			elseif parent_segment_name == "upper_trunk" then
				-- no action required
			else
				error ("You need to specify joint 'neck' relative to either 'trunk' or 'upper_trunk'")
			end
		end

		if side_identifier == "l" then
			joint_origin_rel[2] = -joint_origin_rel[2]
		end

		local joint_origin_abs = {
			joint_origin_rel[1] * self.parameters.height,
			joint_origin_rel[2] * self.parameters.height,
			joint_origin_rel[3] * self.parameters.height}

		-- print ("joint '" .. full_joint_name .. " origin = " .. joint_origin_abs[1] .. ", " .. joint_origin_abs[2] .. ", " .. joint_origin_abs[3])

		return joint_origin_abs
	end

	heiman:refresh()

	return heiman;
end

return newHeiMan
