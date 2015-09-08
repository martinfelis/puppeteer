title = "Puppeteer Lua API"
project = "Puppeteer"

not_luadoc = true
add_language_extension(".cc", "c")
file = { "src", exclude = {
		"src/Animation.cc",
		"src/Camera.h",
		"src/Camera.cc",
		"src/fit_motion.cc",
		"src/FrameConfig.h",
		"src/GLWidget.cc",
		"src/MarkerData.cc",
		"src/Model.h",
		"src/Model.cc",
		"src/main.cc",
		"src/MeshVBO.h",
		"src/MeshVBO.cc",
		"src/ModelFitter.h",
		"src/ModelFitter.cc",
		"src/PuppeteerApp.cc",
		"src/Scene.cc",
		"src/Shader.cc",
		"src/SimpleMath/",
		"src/string_utils.h",
		"src/timer.h",
		"src/vtkChart/"
}}

format = "markdown"
