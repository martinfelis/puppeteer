function serialize_islist (t)
	local itemcount = 0
	local last_type = nil
	for k,v in pairs(t) do
		itemcount = itemcount + 1
		if last_type == nil then
			last_type = type(v)
		end

		if type(v) ~= last_type or (type(v) ~= "string" and type(v) ~= "number" and type(v) ~= "boolean") then
			return false
		end
	
		last_type = type(v)
	end

	if itemcount ~= #t then
		return false
	end

	return true
end

function serialize (o, tabs)
  local result = ""
  
  if tabs == nil then
    tabs = ""
  end

  if type(o) == "number" then
    result = result .. tostring(o)
  elseif type(o) == "boolean" then
    result = result .. tostring(o)
  elseif type(o) == "string" then
    result = result .. string.format("%q", o)
	elseif type(o) == "table" and serialize_islist(o) then
		result = result .. "{"
		for i,v in ipairs(o) do
			result = result .. " " .. tostring(v) .. ","
		end
		result = result .. "}"
  elseif type(o) == "table" then
    if o.dont_serialize_me then
      return "{}"
    end
    result = result .. "{\n"
    for k,v in pairs(o) do
      if type(v) ~= "function" then
        -- make sure that numbered keys are properly are indexified
        if type(k) == "number" then
				  if type(v) == "number" then
	          result = result .. " " .. tostring(v) .. ","
					else
	          result = result .. tabs .. "  " .. serialize(v, tabs .. "  ") .. ",\n"
					end
        else
          result = result .. tabs .. "  " .. k .. " = " .. serialize(v, tabs .. "  ") .. ",\n"
        end
      end
    end
    result = result .. tabs .. "}"
  else
    print ("ignoring stuff" .. type(o) )
  end
  return result
end

return serialize
