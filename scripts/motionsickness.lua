function puppeteer.load()
	print ("Motion Sickness Script Loaded!")
end

local current_time = 0

function puppeteer.update (dt)
	current_time = current_time + dt
end

function puppeteer.draw ()
	local fov = 45. + 45 * (math.sin(current_time * 5) + 1.) / 2
	puppeteer.camera.setFieldOfView (fov)
end
