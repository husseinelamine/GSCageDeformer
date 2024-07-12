
#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "MeshRenderer.h"
#include "ShaderManager.h"
#include "TopologyMapper.h"
#include "PointCloud.h"
#include "TetrahedronMeshRenderer.h"


#include <Eigen/Dense>

#include "camerasconfig.h"
#include "camerapathrenderer.h"
#include "flycam.h"
#include "gaussiancloud.h"
#include "magiccarpet.h"
#include "pointcloud.h"
#include "pointrenderer.h"
#include "splatrenderer.h"

#include "core/framebuffer.h"
#include "core/util.h"

#include <filesystem>


#define Z_NEAR 0.1f
#define Z_FAR 100.0f
#define FOVY 45.0f
#define MOVE_SPEED 0.1f
#define ROT_SPEED 0.01f
#define KEEP_ALL_SPLATS 0




class Renderer {
public:
    Renderer(int width = 720, int height = 480);
    ~Renderer();
    void render();
    int init();
    void update();
    void cleanup();
    bool shouldClose() { return glfwWindowShouldClose(window); }
    void setupShaders();
    void setupBuffers();
    bool initSplatRenderer();
    bool loadSplats();
    GLFWwindow* getWindow() { return window; }

    void bindKeyCallback(GLFWkeyfun callback) {
    	glfwSetKeyCallback(window, callback);
    }

    void addMeshRenderer(MeshRenderer* meshRenderer);

private:
    int width, height;
    GLFWwindow* window;
    ShaderManager shaderManager;
    std::vector<MeshRenderer*> meshRenderers;


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
    std::shared_ptr<TetrahedronMeshRenderer> tetrahedronMeshRenderer;

    std::shared_ptr<TetCage> cage;
    std::shared_ptr<std::vector<Eigen::Vector4f>> barycentricGS;
    // fbo
    std::shared_ptr<FrameBuffer> fbo;
    glm::ivec2 fboSize = { 0, 0 };
    std::shared_ptr<Texture> fboColorTex;

    double lastTime = 0.0;


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



};

#endif // RENDERER_H
