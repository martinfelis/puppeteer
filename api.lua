boxmesh = MeshVBOCube(2., 2., 1)

function init()
end

function draw()
	graphics.clear(Color and depth)

	framebuffer = graphics:newFrameBuffer()
	framebuffer:enableDepth()

	shader = graphics:newShader()
	shader:compile()

	camera = graphics:newCamera()
	camera:lookAt (...)
	camera:rotate()
	camera:setPosition()
	camera:setDirection()
	camera:setFov()
	camera:setOrthographic (state)
	camera:updateFromMouse
	camera:getViewMatrix()
	camera:getProjectionMatrix()

	transform = graphics:newTransform()
	transform:push()
	transform:identity()
	transform:rotate()
	transform:scale()
	transform:translate()
	transform:getMatrix()
	transform:pop()

	graphics:setCamera(camera)
	graphics:render (mesh, transform:getModelViewMatrix(), <shader>)

	mouse_world_pos, mouse_dir = graphics:getMouseRay()
	mousepos = graphics:getMouseScreenPos()

	graphics:setFont (filename, size)
	graphics:printScreen(x, y, str)
	graphics:print (x, y, str)
end
