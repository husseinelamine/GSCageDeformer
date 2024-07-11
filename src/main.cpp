#include "Tetrahedron.h"
#include "PointCloud.h"
#include "TopologyMapper.h"
#include "Renderer.h"
#include "MeshReader.h"
#include "TetrahedronMeshRenderer.h"
#include "core/log.h"
#include "core/util.h"

#include "camerasconfig.h"
#include "camerapathrenderer.h"
#include "flycam.h"
#include "gaussiancloud.h"
#include "magiccarpet.h"
#include "pointcloud.h"
#include "pointrenderer.h"
#include "splatrenderer.h"

#include "core/framebuffer.h"

#include <filesystem>


static void Clear(glm::ivec2 windowSize, bool setViewport = true)
{
    int width = windowSize.x;
    int height = windowSize.y;
    if (setViewport)
    {
        glViewport(0, 0, width, height);
    }

    // pre-multiplied alpha blending
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec4 clearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // NOTE: if depth buffer has less then 24 bits, it can mess up splat rendering.
    glEnable(GL_DEPTH_TEST);

}

// searches for file named configFilename, dir that contains plyFilename, it's parent and grandparent dirs.
static std::string FindConfigFile(const std::string& plyFilename, const std::string& configFilename)
{
	std::filesystem::path plyPath(plyFilename);

	if (!std::filesystem::exists(plyPath) || !std::filesystem::is_regular_file(plyPath))
	{
		Log::E("PLY file does not exist or is not a file: \"%s\"", plyFilename.c_str());
		return "";
	}

	std::filesystem::path directory = plyPath.parent_path();

	for (int i = 0; i < 3; ++i) // Check current, parent, and grandparent directories
	{
		std::filesystem::path configPath = directory / configFilename;
		if (std::filesystem::exists(configPath) && std::filesystem::is_regular_file(configPath))
		{
			return configPath.string();
		}
		if (directory.has_parent_path())
		{
			directory = directory.parent_path();
		}
		else
		{
			break;
		}
	}

	return "";
}
// AJT: TODO this wrapper func is not needed anymore
static std::shared_ptr<PointCloud> LoadPointCloud(const std::string& plyFilename)
{
    auto pointCloud = std::make_shared<PointCloud>();

    if (!pointCloud->ImportPly(plyFilename))
    {
        Log::E("Error loading PointCloud!\n");
        return nullptr;
    }
    return pointCloud;
}

// AJT: TODO this wrapper func is not needed anymore
static std::shared_ptr<GaussianCloud> LoadGaussianCloud(const std::string& plyFilename)
{
    auto gaussianCloud = std::make_shared<GaussianCloud>();

    if (!gaussianCloud->ImportPly(plyFilename))
    {
        Log::E("Error loading GaussianCloud!\n");
        return nullptr;
    }

    return gaussianCloud;
}


static void RenderDesktop(glm::ivec2 windowSize, std::shared_ptr<Program> desktopProgram, uint32_t colorTexture, bool adjustAspect)
{
    int width = windowSize.x;
    int height = windowSize.y;

    glViewport(0, 0, width, height);
    glm::vec4 clearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 projMat = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -10.0f, 10.0f);

    if (colorTexture > 0)
    {
        desktopProgram->Bind();
        desktopProgram->SetUniform("modelViewProjMat", projMat);
        desktopProgram->SetUniform("color", glm::vec4(1.0f));

        // use texture unit 0 for colorTexture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        desktopProgram->SetUniform("colorTexture", 0);

        glm::vec2 xyLowerLeft(0.0f, 0.0f);
        glm::vec2 xyUpperRight((float)width, (float)height);
        if (adjustAspect)
        {
            xyLowerLeft = glm::vec2(0.0f, (height - width) / 2.0f);
            xyUpperRight = glm::vec2((float)width, (height + width) / 2.0f);
        }
        glm::vec2 uvLowerLeft(0.0f, 0.0f);
        glm::vec2 uvUpperRight(1.0f, 1.0f);

        float depth = -9.0f;
        glm::vec3 positions[] = { glm::vec3(xyLowerLeft, depth), glm::vec3(xyUpperRight.x, xyLowerLeft.y, depth),
                                 glm::vec3(xyUpperRight, depth), glm::vec3(xyLowerLeft.x, xyUpperRight.y, depth) };
        desktopProgram->SetAttrib("position", positions);

        glm::vec2 uvs[] = { uvLowerLeft, glm::vec2(uvUpperRight.x, uvLowerLeft.y),
                           uvUpperRight, glm::vec2(uvLowerLeft.x, uvUpperRight.y) };
        desktopProgram->SetAttrib("uv", uvs);

        const size_t NUM_INDICES = 6;
        uint16_t indices[NUM_INDICES] = { 0, 1, 2, 0, 2, 3 };
        glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_SHORT, indices);
    }
}

std::string plyFilename;
std::string camerasConfigFilename;
std::shared_ptr<CamerasConfig> camerasConfig;
std::shared_ptr<CameraPathRenderer> cameraPathRenderer;
std::shared_ptr<FlyCam> flyCam;
std::shared_ptr<MagicCarpet> magicCarpet;
std::string pointCloudFilename;
std::shared_ptr<PointCloud> pointCloud;
std::shared_ptr<PointRenderer> pointRenderer;
std::shared_ptr<GaussianCloud> gaussianCloud;
std::shared_ptr<SplatRenderer> splatRenderer;
std::shared_ptr<Program> desktopProgram;
// fbo
std::shared_ptr<FrameBuffer> fbo;
glm::ivec2 fboSize = { 0, 0 };
std::shared_ptr<Texture> fboColorTex;



//#define HIDE_CONSOLE_WINDOW 1

// check if windows os
#ifdef _WIN32
#include <windows.h>
#endif

// with args
bool init() {
	plyFilename = "C:/Users/helamine/source/repos/splatapult/data/mug/point_cloud/iteration_30000/point_cloud.ply";
	gaussianCloud = std::make_shared<GaussianCloud>();
	if (!gaussianCloud->ImportPly(plyFilename))
	{
		Log::E("Failed to initialize GaussianCloud\n");
		return false;
	}

#if 0
	const uint32_t SPLAT_COUNT = 25000;
	glm::vec3 focalPoint = flyCam->GetCameraMat()[3];
	//gaussianCloud->PruneSplats(glm::vec3(flyCam->GetCameraMat()[3]), SPLAT_COUNT);
	gaussianCloud->PruneSplats(focalPoint, SPLAT_COUNT);
#endif
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
# define MOVE_SPEED 0.1f
# define ROT_SPEED 0.01f

	bool isFramebufferSRGBEnabled = false;

	flyCam = std::make_shared<FlyCam>(floorMatUp, flyCamPos, flyCamRot, MOVE_SPEED, ROT_SPEED);

    // i want to use flyCam oustide of this function so i will assign it to the global variable
    ::flyCam = flyCam;

	magicCarpet = std::make_shared<MagicCarpet>(floorMat, MOVE_SPEED);
	if (!magicCarpet->Init(isFramebufferSRGBEnabled))
	{
		Log::E("Error initalizing MagicCarpet\n");
		return false;
	}

    pointCloudFilename = FindConfigFile(plyFilename, "input.ply");
    if (!pointCloudFilename.empty())
    {
        pointCloud = LoadPointCloud(pointCloudFilename);
        if (!pointCloud)
        {
            Log::E("Error loading PointCloud\n");
            return false;
        }

        pointRenderer = std::make_shared<PointRenderer>();
        if (!pointRenderer->Init(pointCloud, isFramebufferSRGBEnabled))
        {
            Log::E("Error initializing point renderer!\n");
            return false;
        }
    }
    else
    {
        Log::D("Could not find input.ply\n");
    }

    //gaussianCloud = LoadGaussianCloud(plyFilename);
    if (!gaussianCloud)
    {
        Log::E("Error loading GaussianCloud\n");
        return false;
    }

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

}

#define Z_NEAR 0.1f
#define Z_FAR 100.0f
#define FOVY 45.0f


void render() {


    int width = 720, height = 480; // window size
    
    // check fboSize against window size
    if(fboSize.x != width || fboSize.y != height)
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
000        RenderDesktop(fboSize, desktopProgram, fbo->GetColorTexture()->texture, false);
    }

    glm::mat4 cameraMat = flyCam->GetCameraMat();
    glm::vec4 viewport(0.0f, 0.0f, (float)width, (float)height);
    glm::vec2 nearFar(Z_NEAR, Z_FAR);
    glm::mat4 projMat = glm::perspective(FOVY, (float)width / (float)height, Z_NEAR, Z_FAR);

    splatRenderer->Sort(cameraMat, projMat, viewport, nearFar);
    splatRenderer->Render(cameraMat, projMat, viewport, nearFar);

}

int main(int argc, char* argv[]) {

#ifdef _WIN32
#ifdef HIDE_CONSOLE_WINDOW
	// hide console window
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
#endif

	/*//std::vector<Tetrahedron> tetrahedrons{
	//	{
	//		{ 0, 0, 0 },
	//		{ 1, 0, 0 },
	//		{ 0, 1, 0 },
	//		{ 0, 0, 1 }
	//	},
	//	{
	//		{ 0, 0, 0 },
	//		{ 1, 0, 0 },
	//		{ 0, 1, 0 },
	//		{ 0, 0, 1 }
	//	},
	//	{
	//		{ 0, 0, 0 },
	//		{ 1, 0, 0 },
	//		{ 0, 1, 0 },
	//		{ 0, 0, 1 }
	//	}
	//};

	//TetCage cage{ tetrahedrons };
	//cage.init();

	//const std::vector<Eigen::Vector3f> points{
	//	{0.5, 0.23, 0.78},
	//	{0.1, 0.2, 0.3},
	//	{0.7, 0.8, 0.9},
	//	{0.4, 0.5, 0.6}
	//};

	//PointCloud pc{points};

	//PointCloud barycentricPC;
	//TopologyMapper::barycentricPCtoTetCage(pc, cage, barycentricPC);*/

	Renderer renderer{ 720, 480 }; 
	if (renderer.init() == -1)
	{
		return -1;
	}

	init();

	MeshReader meshReader;
	Mesh mesh;
	meshReader.readMsh("C:/Users/helamine/source/repos/ISProjectsV1/ISPhysics/assets/meshes/mug/surface_cage.obj_.msh", mesh);

	meshReader.reset();
	Mesh mesh2;
	meshReader.readMsh("C:/Users/helamine/source/repos/ISProjectsV1/ISPhysics/assets/meshes/mug/surface_cage.obj_.msh", mesh2);

	std::vector<Tetrahedron> tetrahedrons = meshReader.getTetrahedrons(mesh);
	TetCage cage{ tetrahedrons };
	cage.init();

	TetrahedronMeshRenderer tetrahedronMeshRenderer{ &cage };
	renderer.addMeshRenderer(&tetrahedronMeshRenderer);

	renderer.setupBuffers();

	while (!renderer.shouldClose())
	{
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//renderer.render();

        render();

        glfwSwapBuffers(renderer.getWindow());
        glfwPollEvents();


	}


 
	return 0;
}

/*
    Copyright (c) 2024 Anthony J. Thibault
    This software is licensed under the MIT License. See LICENSE for more details.
*/
/*
// 3d gaussian splat renderer
#define GAUSSIAN_PATH "C:/Users/helamine/Documents/3DGS_Tests/Cottage/output/point_cloud/iteration_30000/point_cloud.ply"
#include <chrono>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_syswm.h>
#include <stdint.h>
#include <thread>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#else
#define FrameMark do {} while(0)
#endif

#ifndef SHIPPING
#define SHIPPING 1
#endif

#include "core/log.h"
#include "core/util.h"

#include "app.h"

//#define SOFTWARE_SPLATS

struct GlobalContext
{
    bool quitting = false;
    SDL_Window* window = NULL;
    SDL_GLContext gl_context;
};

GlobalContext ctx;

int SDLCALL Watch(void* userdata, SDL_Event* event)
{
    if (event->type == SDL_APP_WILLENTERBACKGROUND)
    {
        ctx.quitting = true;
    }

    return 1;
}

int main(int argc, char* argv[])
{
    Log::SetAppName("splataplut");
    MainContext mainContext;
    App app(mainContext);
    App::ParseResult parseResult = app.ParseArguments(argc, (const char**)argv);
    switch (parseResult)
    {
    case App::SUCCESS_RESULT:
        break;
    case App::ERROR_RESULT:
        Log::E("App::ParseArguments failed!\n");
        return 1;
    case App::QUIT_RESULT:
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) != 0)
    {
        Log::E("Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    const int32_t WIDTH = 1024;
    const int32_t HEIGHT = 768;

    // Allow us to use automatic linear->sRGB conversion.
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    // increase depth buffer size
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    uint32_t windowFlags = SDL_WINDOW_OPENGL;
    if (app.IsFullscreen())
    {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    else
    {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }
    ctx.window = SDL_CreateWindow("splatapult", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, windowFlags);

    if (!ctx.window)
    {
        Log::E("Failed to create window: %s\n", SDL_GetError());
        return 1;
    }

    ctx.gl_context = SDL_GL_CreateContext(ctx.window);

    SDL_GL_MakeCurrent(ctx.window, ctx.gl_context);

#ifdef __linux__
    // Initialize context from the SDL window
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version)
        auto ret = SDL_GetWindowWMInfo(ctx.window, &info);
    if (ret != SDL_TRUE)
    {
        Log::W("Failed to retrieve SDL window info: %s\n", SDL_GetError());
    }
    else
    {
        mainContext.xdisplay = info.info.x11.display;
        mainContext.glxDrawable = (GLXWindow)info.info.x11.window;
        mainContext.glxContext = (GLXContext)ctx.gl_context;
    }
#endif

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        Log::E("Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    // AJT: TODO REMOVE disable vsync for benchmarks
    SDL_GL_SetSwapInterval(0);

    SDL_AddEventWatch(Watch, NULL);

    if (!app.Init())
    {
        Log::E("App::Init failed\n");
        return 1;
    }

    bool shouldQuit = false;
    app.OnQuit([&shouldQuit]()
    {
        shouldQuit = true;
    });

    app.OnResize([](int newWidth, int newHeight)
    {
        //SDL_RenderSetLogicalSize(ctx.renderer, newWidth, newHeight);
        // AJT: TODO resize texture?
    });

    uint32_t frameCount = 1;
    uint32_t frameTicks = SDL_GetTicks();
    uint32_t lastTicks = SDL_GetTicks();
    while (!ctx.quitting && !shouldQuit)
    {
        // update dt
        uint32_t ticks = SDL_GetTicks();

        const int FPS_FRAMES = 100;
        if ((frameCount % FPS_FRAMES) == 0)
        {
            float delta = (ticks - frameTicks) / 1000.0f;
            float fps = (float)FPS_FRAMES / delta;
            frameTicks = ticks;
            app.UpdateFps(fps);
        }
        float dt = (ticks - lastTicks) / 1000.0f;
        lastTicks = ticks;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            app.ProcessEvent(event);
        }

        if (!app.Process(dt))
        {
            Log::E("App::Process failed!\n");
            return 1;
        }

        SDL_GL_MakeCurrent(ctx.window, ctx.gl_context);

        int width, height;
        SDL_GetWindowSize(ctx.window, &width, &height);
        if (!app.Render(dt, glm::ivec2(width, height)))
        {
            Log::E("App::Render failed!\n");
            return 1;
        }

        SDL_GL_SwapWindow(ctx.window);

        frameCount++;

        FrameMark;
    }

    SDL_DelEventWatch(Watch, NULL);
    SDL_GL_DeleteContext(ctx.gl_context);

    SDL_DestroyWindow(ctx.window);
    SDL_Quit();

    return 0;
}
*/