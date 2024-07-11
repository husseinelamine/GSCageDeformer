#include "Renderer.h"
#include "ShaderManager.h"
#include <iostream>
#include "UtilMath.h"
#include "MeshReader.h"
#include "TopologyMapper.h"


Renderer::Renderer(int width, int height) : width(width), height(height) {}

Renderer::~Renderer() {
}

int Renderer::init() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(width, height, "GSCageDeformer", NULL, NULL);

	if (!window) {
		glfwTerminate();
		std::cerr << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	bindKeyCallback([](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	});

	glEnable(GL_DEPTH_TEST);


	//setupBuffers();
	//setupShaders();

	if (!initSplatRenderer()) {
		return -1;
	}

	//MeshReader meshReader;
	//Mesh mesh;
	//meshReader.readMsh("C:/Users/helamine/source/repos/ISProjectsV1/ISPhysics/assets/meshes/mug/surface_cage.obj_.msh", mesh);

	//std::vector<Tetrahedron> tetrahedrons = meshReader.getTetrahedrons(mesh);
	//TetCage cage{ tetrahedrons };
	//cage.init();
	//cage.simplify();

	std::vector<Eigen::Vector3f> barycentricGS_;
	TetCage cage_;

	// record how many milliseconds it takes to run the function
	auto start = std::chrono::high_resolution_clock::now();
	TopologyMapper::barycentricGStoTetCageFromEmptyCage(*gaussianCloud, cage_, barycentricGS_);
	//TopologyMapper::barycentricGStoTetCage(*gaussianCloud, cage, barycentricGS);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	std::cout << cage_.tetrahedrons.size() << std::endl;
	std::cout << barycentricGS_.size() << std::endl;

	cage = std::make_shared<TetCage>(cage_);
	barycentricGS = std::make_shared<std::vector<Eigen::Vector3f>>(barycentricGS_);




	return 1;
}

void Renderer::setupShaders() {
	ShaderManager::init();
	shaderManager.loadShader("basic");
}

void Renderer::setupBuffers() {
	for (auto meshRenderer : meshRenderers) {
		meshRenderer->setupBuffers();
	}
}

bool Renderer::initSplatRenderer() {
	plyFilename = "C:/Users/helamine/source/repos/splatapult/data/mug/point_cloud/iteration_30000/point_cloud.ply";
	gaussianCloud = std::make_shared<GaussianCloud>();
	if (!gaussianCloud->ImportPly(plyFilename))
	{
		Log::E("Failed to initialize GaussianCloud\n");
		return false;
	}


	camerasConfigFilename = FindConfigFile(plyFilename, "cameras.json");
	if (!camerasConfigFilename.empty())
	{
		camerasConfig = std::make_shared<CamerasConfig>();
		if (!camerasConfig->ImportJson(camerasConfigFilename))
		{
			Log::W("Error loading cameras.json\n");
			camerasConfig.reset();
		}
	}
	else
	{
		Log::D("Could not find cameras.json\n");
	}


	if (camerasConfig)
	{
		cameraPathRenderer = std::make_shared<CameraPathRenderer>();
		if (!cameraPathRenderer->Init(camerasConfig->GetCameraVec()))
		{
			Log::E("CameraPathRenderer Init failed\n");
			return false;
		}
	}

	int cameraIndex = 0;

	glm::mat4 flyCamMat(1.0f);
	glm::mat4 floorMat(1.0f);
	if (camerasConfig)
	{
		flyCamMat = camerasConfig->GetCameraVec()[cameraIndex].mat;

		// initialize magicCarpet from first camera and estimated floor position.
		if (camerasConfig->GetNumCameras() > 0)
		{
			glm::vec3 floorNormal, floorPos;
			camerasConfig->EstimateFloorPlane(floorNormal, floorPos);
			glm::vec3 floorZ = camerasConfig->GetCameraVec()[0].mat[2];
			glm::vec3 floorY = floorNormal;
			glm::vec3 floorX = glm::cross(floorY, floorZ);
			floorZ = glm::cross(floorX, floorY);

			floorMat = glm::mat4(glm::vec4(floorX, 0.0f),
				glm::vec4(floorY, 0.0f),
				glm::vec4(floorZ, 0.0f),
				glm::vec4(floorPos, 1.0f));
		}
	}

	glm::vec3 flyCamPos, flyCamScale, floorMatUp;
	glm::quat flyCamRot;
	floorMatUp = glm::vec3(floorMat[1]);
	Decompose(flyCamMat, &flyCamScale, &flyCamRot, &flyCamPos);

	std::shared_ptr<FlyCam> flyCam;
	std::shared_ptr<MagicCarpet> magicCarpet;

	bool isFramebufferSRGBEnabled = false;

	this->flyCam = std::make_shared<FlyCam>(floorMatUp, flyCamPos, flyCamRot, MOVE_SPEED, ROT_SPEED);


#ifndef KEEP_ALL_SPLATS
	const uint32_t SPLAT_COUNT = 100;
	glm::vec3 focalPoint = this->flyCam->GetCameraMat()[3];
	//gaussianCloud->PruneSplats(glm::vec3(flyCam->GetCameraMat()[3]), SPLAT_COUNT);
	gaussianCloud->PruneSplats(focalPoint, SPLAT_COUNT);
#endif

	splatRenderer = std::make_shared<SplatRenderer>();

	bool useFullSH = true;
	bool useRgcSortOverride = false;

	if (!splatRenderer->Init(gaussianCloud, isFramebufferSRGBEnabled, useFullSH, useRgcSortOverride))
	{
		Log::E("Error initializing splat renderer!\n");
		return false;
	}
	desktopProgram = std::make_shared<Program>();
	if (!desktopProgram->LoadVertFrag("shader/desktop_vert.glsl", "shader/desktop_frag.glsl"))
	{
		Log::E("Error loading desktop shader!\n");
		return 1;
	}

	return true;

}



void Renderer::addMeshRenderer(MeshRenderer* meshRenderer) {
	meshRenderers.push_back(meshRenderer);
}

void Renderer::render() {
	/*glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

	Clear(glm::ivec2(width, height), true);

	float time = glfwGetTime();

	/*shaderManager.useShader("basic");

	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	model = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, 0.0f)).matrix();
	view = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0f, -100.0f)).matrix();
	projection = Eigen::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

	GLint modelLoc = glGetUniformLocation(shaderManager.getShader("basic")->getID(), "model");
	GLint viewLoc = glGetUniformLocation(shaderManager.getShader("basic")->getID(), "view");
	GLint projLoc = glGetUniformLocation(shaderManager.getShader("basic")->getID(), "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.data());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.data());
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.data());

	for (auto meshRenderer : meshRenderers) {
		meshRenderer->render();
	}*/

	// check fboSize against window size
	if (fboSize.x != width || fboSize.y != height)
	{
		fboSize = glm::ivec2(width, height);
		fbo = std::make_shared<FrameBuffer>();

		Texture::Params texParams;
		texParams.minFilter = FilterType::Nearest;
		texParams.magFilter = FilterType::Nearest;
		texParams.sWrap = WrapType::ClampToEdge;
		texParams.tWrap = WrapType::ClampToEdge;

		fboColorTex = std::make_shared<Texture>(width, height,
			GL_RGBA32F, GL_RGBA, GL_FLOAT, texParams);

		fbo->AttachColor(fboColorTex);
		fbo->Bind();

	}

	if (fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Clear(fboSize, true);
		RenderDesktop(fboSize, desktopProgram, fbo->GetColorTexture()->texture, false);
	}

	glm::mat4 cameraMat = this->flyCam->GetCameraMat();
	glm::vec4 viewport(0.0f, 0.0f, (float)width, (float)height);
	glm::vec2 nearFar(Z_NEAR, Z_FAR);
	glm::mat4 projMat = glm::perspective(FOVY, (float)width / (float)height, Z_NEAR, Z_FAR);

	//splatRenderer->deformCov(gaussianCloud);
	splatRenderer->Sort(cameraMat, projMat, viewport, nearFar);
	splatRenderer->Render(cameraMat, projMat, viewport, nearFar);

	glfwSwapBuffers(window);
	glfwPollEvents();
}
