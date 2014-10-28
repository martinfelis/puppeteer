title = "Puppeteer Lua API"
project = "Puppeteer"

not_luadoc = true
add_language_extension(".cc", "c")
file = { "src", exclude = {
		"src/Animation.cc",
		"src/Camera.cc",
		"src/fit_motion.cc",
		"src/GLWidget.cc",
		"src/MarkerData.cc",
		"src/Model.cc",
		"src/main.cc",
		"src/MeshVBO.cc",
		"src/ModelFitter.cc",
		"src/PuppeteerApp.cc",
		"src/Scene.cc",
		"src/Shader.cc",
}}

format = "markdown"
