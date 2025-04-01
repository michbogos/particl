#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include<iostream>
#include<vector>

#include<string.h>
#include<strings.h>
#include<stdio.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800



bgfx::ShaderHandle loadShader(const char *FILENAME)
{
    const char* shaderPath = "???";

    size_t shaderLen = strlen(shaderPath);
    size_t fileLen = strlen(FILENAME);
    char *filePath = (char *)malloc(shaderLen + fileLen);
    memcpy(filePath, shaderPath, shaderLen);
    memcpy(&filePath[shaderLen], FILENAME, fileLen);

    FILE *file = fopen(FILENAME, "rb");
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
    fread(mem->data, 1, fileSize, file);
    mem->data[mem->size - 1] = '\0';
    fclose(file);

    return bgfx::createShader(mem);
}


int main(void)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("assets/icosphere.obj", aiProcess_CalcTangentSpace| aiProcess_FlipWindingOrder |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);
    
    struct PosColorVertex
    {
        float x;
        float y;
        float z;
    };

    // static PosColorVertex cubeVertices[] =
    // {
    //     {-1.0f,  1.0f,  1.0f, 0xff000000 },
    //     { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    //     {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    //     { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    //     {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    //     { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    //     {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    //     { 1.0f, -1.0f, -1.0f, 0xffffffff },
    // };

    if(scene == nullptr){
        std::cout << "Couldn't load obj file";
    }

    aiVector3D* vertices = scene->mMeshes[0]->mVertices;
    aiFace* faces = scene->mMeshes[0]->mFaces;

    std::vector<uint16_t> modelTriList;
    std::vector<PosColorVertex> modelVertexList;

    for(int i = 0; i < scene->mMeshes[0]->mNumVertices; i++){
        modelVertexList.push_back({vertices[i].x, vertices[i].y, vertices[i].z});
    }
    for(int face = 0; face < scene->mMeshes[0]->mNumFaces; face ++){
        for(int index = 0; index < faces[face].mNumIndices; index++){
            modelTriList.push_back(faces[face].mIndices[index]);
        }
    }



    // static const uint16_t cubeTriList[] =
    // {
    //     0, 1, 2,
    //     1, 3, 2,
    //     4, 6, 5,
    //     5, 6, 7,
    //     0, 2, 4,
    //     4, 2, 6,
    //     1, 5, 3,
    //     5, 7, 3,
    //     0, 4, 1,
    //     4, 5, 1,
    //     2, 3, 6,
    //     6, 3, 7,
    // };


    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Particl", NULL, NULL);

    bgfx::PlatformData pd;
    pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
    pd.ndt = glfwGetX11Display();

    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Vulkan; // Automatically choose a renderer.
    bgfxInit.resolution.width = WINDOW_WIDTH;
    bgfxInit.resolution.height = WINDOW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfxInit.platformData = pd;
    bgfx::init(bgfxInit);

    std::cout << "Successfully inited bgfx\n";

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    bgfx::ShaderHandle vsh = loadShader("spirv/shader.vs.bin");
    bgfx::ShaderHandle fsh = loadShader("spirv/shader.fs.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    unsigned int counter = 0;

    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices, modelVertexList.size()*sizeof(PosColorVertex)), pcvDecl);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(modelTriList.data(), modelTriList.size()*sizeof(uint16_t)));

    // bgfx::InstanceDataBuffer idb;
    // bgfx::allocInstanceDataBuffer(&idb, 1000, 16);

    // float* data = (float*)idb.data;

    bgfx::VertexLayout computeVertexLayout;
			computeVertexLayout.begin()
				.add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
			.end();

    bgfx::DynamicVertexBufferHandle positionBuffer = bgfx::createDynamicVertexBuffer(128*128*128*2, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
    bgfx::DynamicVertexBufferHandle velocityBuffer = bgfx::createDynamicVertexBuffer(128*128*128*2, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);

    bgfx::ProgramHandle initProgram = bgfx::createProgram(loadShader("spirv/init.cs.bin"), true);
    bgfx::ProgramHandle simulationProgram = bgfx::createProgram(loadShader("spirv/simulate.cs.bin"), true);

    bgfx::setBuffer(0, positionBuffer, bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, velocityBuffer, bgfx::Access::ReadWrite);

    bgfx::dispatch(0, initProgram, 32,32,32);

    std::cout << "Ran shader\n";

    while(!glfwWindowShouldClose(window)) {   
        glfwPollEvents(); 
        bgfx::touch(0);
        const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
        const bx::Vec3 eye = {0.0f, 0.0f, -50.0f};
        float view[16];
        float transform[16];
        bx::mtxLookAt(view, eye, at);
        float proj[16];
        bx::mtxProj(proj, 100.0f, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 1000.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        float mtx[16];
        bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
        bgfx::setTransform(mtx);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::UniformHandle uniformHandle = bgfx::createUniform("time", bgfx::UniformType::Vec4, 1);

        float time[4] = {counter/100.0f, counter/100.0f ,counter/100.0f, counter/100.0f}; 
        bgfx::setUniform(uniformHandle, time, 1);
        bgfx::setBuffer(0, positionBuffer, bgfx::Access::Write);
        bgfx::setBuffer(1, velocityBuffer, bgfx::Access::Write);
        bgfx::dispatch(0, simulationProgram, 32, 32, 32);

        bgfx::setInstanceDataBuffer(positionBuffer, 0, 128*128*128);

        bgfx::setState(BGFX_STATE_DEFAULT);

        bgfx::submit(0, program);
        bgfx::frame();

        counter ++;
    }

    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}