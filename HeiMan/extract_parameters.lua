#!/usr/bin/lua

matrix = require "HeiMan.matrix"

SegmentNames = {
	Pelvis = "Pelvis",
	ThighRight = "ThighRight",
	ShankRight = "ShankRight",
	FootRight = "FootRight",
	ThighLeft = "ThighLeft",
	ShankLeft = "ShankLeft",
	FootLeft = "FootLeft",
	MiddleTrunk = "MiddleTrunk",
	UpperTrunk = "UpperTrunk",
	UpperArmRight = "UpperArmRight",
	LowerArmRight = "LowerArmRight",
	HandRight = "HandRight",
	UpperArmLeft = "UpperArmLeft",
	LowerArmLeft = "LowerArmLeft",
	HandLeft = "HandLeft",
	Neck = "Neck",
	Head = "Head",
}

function usage()
	print ("Usage: lua " .. arg[0] .. " <model.lua>")
	print ("Extracts parameters for the new HeiMan model from old HeiMan models.")
	os.exit()
end

if #arg ~= 1 then
	usage()
end

model = dofile (arg[1])

frame_indices = {}
segment_positions = {}

for i,frame in ipairs (model.frames) do
	if frame.name then
		frame_indices[frame.name] = i
	end

	local joint_r = matrix({{0., 0., 0.}})
	local parent_pos = matrix({{0., 0., 0.}})

	if frame and frame.joint_frame and frame.joint_frame.r then
		joint_r = matrix({frame.joint_frame.r})
	end

	if frame.parent ~= "ROOT" then
		if not segment_positions[frame.parent] then
			error ("Could not find position of parent frame " .. frame.parent .. "!")
		end
		parent_pos = segment_positions[frame.parent]
	end

	segment_positions[frame.name] = parent_pos + joint_r
end

function calc_length_params ()
	local result = {}
	local frames = model.frames

	local PelvisPos = segment_positions[SegmentNames.Pelvis]
	local HipRightPos = segment_positions[SegmentNames.ThighRight]
	local KneeRightPos = segment_positions[SegmentNames.ShankRight]
	local AnkleRightPos = segment_positions[SegmentNames.FootRight]
	local HipLeftPos = segment_positions[SegmentNames.ThighLeft]
	local KneeLeftPos = segment_positions[SegmentNames.ShankLeft]
	local AnkleLeftPos = segment_positions[SegmentNames.FootLeft]
	local LumbarJointPos = segment_positions[SegmentNames.MiddleTrunk]
	local ThoraxJointPos = segment_positions[SegmentNames.UpperTrunk]

	local ShoulderRightPos = segment_positions[SegmentNames.UpperArmRight]
	local ElbowRightPos = segment_positions[SegmentNames.LowerArmRight]
	local WristRightPos = segment_positions[SegmentNames.HandRight]
	local ShoulderLeftPos = segment_positions[SegmentNames.UpperArmLeft]
	local ElbowLeftPos = segment_positions[SegmentNames.LowerArmLeft]
	local WristLeftPos = segment_positions[SegmentNames.HandLeft]
	local NeckPos = segment_positions[SegmentNames.Neck]
	local HeadPos = segment_positions[SegmentNames.Head]

	-- Right Leg
	result["HipRightWidth"] = (PelvisPos - HipRightPos)[1][2]
	result["ThighRightLength"] = (HipRightPos - KneeRightPos)[1][3]
	result["ShankRightLength"] = (KneeRightPos - AnkleRightPos)[1][3]

	result["FootRightLength"] = frames[frame_indices[SegmentNames.FootRight]].visuals[1].dimensions[1]
	result["FootRightHeight"] = frames[frame_indices[SegmentNames.FootRight]].visuals[1].dimensions[3]
	foot_visual = frames[frame_indices[SegmentNames.FootRight]].visuals[1]
	result["FootRightPosteriorPoint"] = foot_visual.dimensions[1] * 0.5 - foot_visual.mesh_center[1]
	
	-- Left Leg
	result["HipLeftWidth"] = (HipLeftPos - PelvisPos)[1][2]
	result["ThighLeftLength"] = (HipLeftPos - KneeLeftPos)[1][3]
	result["ShankLeftLength"] = (KneeLeftPos - AnkleLeftPos)[1][3]

	result["FootLeftLength"] = frames[frame_indices[SegmentNames.FootLeft]].visuals[1].dimensions[1]
	result["FootLeftHeight"] = frames[frame_indices[SegmentNames.FootLeft]].visuals[1].dimensions[3]
	foot_visual = frames[frame_indices[SegmentNames.FootLeft]].visuals[1]
	result["FootLeftPosteriorPoint"] = foot_visual.dimensions[1] * 0.5 - foot_visual.mesh_center[1]
	
	result["LowerTrunkHeight"] = (LumbarJointPos - PelvisPos)[1][3]	
	result["MiddleTrunkHeight"] = (ThoraxJointPos - LumbarJointPos)[1][3]	

	-- Right Arm
	result["ClaviculaRightLength"] = (ThoraxJointPos - ShoulderRightPos)[1][2]
	result["ShoulderRightHeight"] = (ShoulderRightPos - ThoraxJointPos)[1][3]
	result["UpperArmRightLength"] = (ShoulderRightPos - ElbowRightPos)[1][3]
	result["LowerArmRightLength"] = (ElbowRightPos - WristRightPos)[1][3]
	result["HandRightLength"] = frames[frame_indices[SegmentNames.HandRight]].visuals[1].dimensions[3]

	-- Left Arm
	result["ClaviculaLeftLength"] = (ShoulderLeftPos - ThoraxJointPos)[1][2]
	result["ShoulderLeftHeight"] = (ShoulderLeftPos - ThoraxJointPos)[1][3]
	result["UpperArmLeftLength"] = (ShoulderLeftPos - ElbowLeftPos)[1][3]
	result["LowerArmLeftLength"] = (ElbowLeftPos - WristLeftPos)[1][3]
	result["HandLeftLength"] = frames[frame_indices[SegmentNames.HandLeft]].visuals[1].dimensions[3]

	result["UpperTrunkHeight"] = (NeckPos - ThoraxJointPos)[1][3]
	result["SuprasternaleHeight"] = (NeckPos - ThoraxJointPos)[1][3]
	result["NeckLength"] = (HeadPos - NeckPos)[1][3]

	-- HeadLength
	result["HeadLength"] = frames[frame_indices[SegmentNames.Head]].visuals[1].dimensions[3]

	return result
end

length_params = calc_length_params (segment_positions)

--print ("Parameters:")
-- params = get_params (model)
keys = {}
for k,v in pairs(length_params) do
	table.insert (keys, k)
end
table.sort (keys)
for i,k in ipairs (keys) do
	print ("heiman.parameters[\"" .. k .. "\"] = " .. length_params[k])
end

