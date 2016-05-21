#!/usr/bin/lua

matrix = require "HeiMan.matrix"
serialize = require "HeiMan.serialize"

function usage()
	print ("Usage: lua " .. arg[0] .. " <model.lua>")
	print ("Extracts markers from a modified HeiMan models so that they can be used in new HeiMan models.")
	os.exit()
end

if #arg ~= 1 then
	usage()
end

model = dofile (arg[1])

function find_frame (model, frame_name) 
	for i,frame in ipairs (model.frames) do
		if frame.name == frame_name then
			return i, frame
		end
	end

	return nil, "frame not found"
end

function get_markers (model)
	local markers = {}

	for i, frame in ipairs (model.frames) do
		if frame.markers then
			if not markers[frame.name] then
				markers[frame.name] = {}
			end
			for marker, coords in pairs (frame.markers) do
				markers[frame.name][marker] = coords
			end
		end
	end

	return markers
end

function calc_origin (model, frame_name)
	if frame_name == "ROOT" then
		return matrix({{0., 0., 0.}})
	end

	local index, frame = find_frame (model, frame_name)
	if not index then
		error ("Could not find frame " .. frame_name)
	end

	if frame.joint_frame and frame.joint_frame.r then
		return matrix({frame.joint_frame.r}) + calc_origin (model, frame.parent)
	end
end

function calc_point (model, point)
	if point == "ROOT" then
		return matrix({{0., 0., 0.}})
	end

	local index, frame = find_frame (model, point)
	if index then
		return calc_origin (model, point)
	else
		for i,p in ipairs (model.points) do
			local coords = matrix({p.point})
			if p.name == point then
				return calc_origin (model, p.body) + coords
			end
		end
	end
end

function calc_total_mass (model)
	local total_mass = 0.

	for i, frame in ipairs (model.frames) do
		if frame.body and frame.body.mass then
			total_mass = total_mass + frame.body.mass
		end
	end

	return total_mass
end

function calc_segment_length (model, segment_name)
	local endpoints = {
		Pelvis = { "MiddleTrunk", "ROOT" },
		Thigh_r = { "Thigh_r", "Shank_r" },
		Shank_r = { "Shank_r", "Foot_r" },
		Foot_r = { "right_toe", "right_heel"},
		Thigh_l = { "Thigh_l", "Shank_l" },
		Shank_l = { "Shank_l", "Foot_l" },
		Foot_l = { "left_toe", "left_heel"},
		MiddleTrunk = { "UpperTrunk", "MiddleTrunk" },
		UpperTrunk = { "Head", "UpperTrunk" },
		UpperArm_r = { "UpperArm_r", "LowerArm_r" },
		LowerArm_r = { "LowerArm_r", "Hand_r" },
		UpperArm_l = { "UpperArm_l", "LowerArm_l" },
		LowerArm_l = { "LowerArm_l", "Hand_l" },
	}

	if endpoints[segment_name] == nil then
		error ("Endpoints for segment " .. segment_name .. " not defined!")
	end

	return calc_point(model, endpoints[segment_name][1]) - calc_point(model, endpoints[segment_name][2])
end

function get_params (model)
	params = {}

	params["HipRightWidth"] = (calc_point(model, "Pelvis") - calc_point (model, "Thigh_r"))[1][2]
	params["HipLeftWidth"] = ( calc_point (model, "Thigh_l") - calc_point(model, "Pelvis"))[1][2]
	params["ThighRightLength"] = calc_segment_length(model, "Thigh_r"):transpose():len()
	params["ShankRightLength"] = calc_segment_length(model, "Shank_r"):transpose():len()
	params["FootRightLength"] = calc_segment_length(model, "Foot_r"):transpose():len()
	params["FootRightHeight"] = (calc_point (model, "Foot_r") - calc_point (model, "right_heel"))[1][3]
	params["FootRightPosteriorPoint"] = -(calc_point (model, "right_heel") - calc_point (model, "Foot_r"))[1][1]

	params["ThighLeftLength"] = calc_segment_length(model, "Thigh_r"):transpose():len()
	params["ShankLeftLength"] = calc_segment_length(model, "Shank_r"):transpose():len()
	params["FootLeftLength"] = calc_segment_length(model, "Foot_r"):transpose():len()
	params["FootLeftHeight"] = (calc_point (model, "Foot_l") - calc_point (model, "left_heel"))[1][3]
	params["FootLeftPosteriorPoint"] = -(calc_point (model, "left_heel") - calc_point (model, "Foot_l"))[1][1]

	params["LowerTrunkHeight"] = calc_segment_length(model, "Pelvis"):transpose():len()
	params["MiddleTrunkHeight"] = calc_segment_length(model, "MiddleTrunk"):transpose():len()
	params["UpperTrunkHeight"] = calc_segment_length(model, "UpperTrunk"):transpose():len()

	params["ShoulderRightHeight"] = (calc_origin(model, "UpperArm_r") - calc_origin(model, "UpperTrunk"))[1][3]
	params["ClaviculaRightLength"] = (calc_origin(model, "UpperTrunk") - calc_origin(model, "UpperArm_r"))[1][2]
	params["UpperArmRightLength"] = calc_segment_length(model, "UpperArm_r"):transpose():len()
	params["LowerArmRightLength"] = calc_segment_length(model, "LowerArm_r"):transpose():len()

	index, frame = find_frame (model, "Hand_r")
	params["HandRightLength"] = frame.visuals[1].dimensions[2]
	index, frame = find_frame (model, "Hand_l")
	params["HandLeftLength"] = frame.visuals[1].dimensions[2]

	params["ShoulderLeftHeight"] = (calc_origin(model, "UpperArm_l") - calc_origin(model, "UpperTrunk"))[1][3]
	params["ClaviculaLeftLength"] = (calc_origin(model, "UpperArm_l") - calc_origin(model, "UpperTrunk"))[1][2]
	params["UpperArmLeftLength"] = calc_segment_length(model, "UpperArm_l"):transpose():len()
	params["LowerArmLeftLength"] = calc_segment_length(model, "LowerArm_l"):transpose():len()
--	params["HandLeftLength"] = calc_segment_length(model, "Hand_l"):transpose():len()

--	params["NeckLength"] = calc_segment_length(model, "Hand_r"):transpose():len()
	local head_index, head_frame = find_frame (model, "Head")
	if not head_index or not head_frame.visuals or not head_frame.visuals[1] or not head_frame.visuals[1].dimensions then
		error ("Could not find 'Head' scaling information")
	end
	params["HeadLength"] = head_frame.visuals[1].dimensions[3]

	params["SuprasternaleHeight"] = params["ShoulderRightHeight"] * 0.5 + params["ShoulderLeftHeight"] * 0.5
	params["NeckLength"] = params["HeadLength"] - params["SuprasternaleHeight"]

	return params
end

segments = {
	"Pelvis",
	"Thigh_r",
	"Shank_r",
	"Foot_r",
	"Thigh_l",
	"Shank_l",
	"Foot_l",
	"MiddleTrunk",
	"UpperTrunk",
	"UpperArm_r",
	"LowerArm_r",
	"Hand_r",
	"UpperArm_l",
	"LowerArm_l",
	"Hand_l",
	"Head"
}

markers = get_markers(model)
print (serialize(markers))

