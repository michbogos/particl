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

unsigned int windowWidth = 800;
unsigned int windowHeight = 800;

#define SHADER_DIM 4
#define LOCAL_DIM 8
#define DOMAIN_DIM 4.0f
#define PARTICLE_SIZE ((DOMAIN_DIM)/(float(SHADER_DIM)*float(LOCAL_DIM)))

bx::Vec3 direction = {0.0f, 0.0f,  -1.0f};
bx::Vec3 rotation = {0, 0, 0};
bx::Vec3 eye = {0.0f, 0.0f, -10.0f};
float deltaTime = 0.0f;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float rot[16];
    switch(key){
        case GLFW_KEY_W:
                eye.z += 0.1*deltaTime;
            break;
        case GLFW_KEY_S:
            if (action == GLFW_PRESS)
                eye.z -= 0.1*deltaTime;
            break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS)
                eye.x -= 0.1*deltaTime;
            break;
        case GLFW_KEY_D:
            if (action == GLFW_PRESS)
                eye.x += 0.1*deltaTime;
            break;
        case GLFW_KEY_SPACE:
            if(action == GLFW_PRESS && !(mods&GLFW_MOD_SHIFT))
                eye.y += 0.1*deltaTime;
            if(action == GLFW_PRESS && (mods&GLFW_MOD_SHIFT))
                eye.y -= 0.1*deltaTime;
            break;
        
        case GLFW_KEY_UP:
            if (action == GLFW_PRESS)
                rotation.x += 0.01*deltaTime;
                bx::mtxRotateXYZ(rot, rotation.x, rotation.y, rotation.z);
                direction = bx::mul({0, 0, -1}, rot);
            break;
        case GLFW_KEY_DOWN:
            if (action == GLFW_PRESS)
                rotation.x -= 0.01*deltaTime;
                bx::mtxRotateXYZ(rot, rotation.x, rotation.y, rotation.z);
                direction = bx::mul({0, 0, -1}, rot);
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS)
                rotation.y += 0.01*deltaTime;
                bx::mtxRotateXYZ(rot, rotation.x, rotation.y, rotation.z);
                direction = bx::mul({0, 0, -1}, rot);
            break;
        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS)
                rotation.y -= 0.01*deltaTime;
                bx::mtxRotateXYZ(rot, rotation.x, rotation.y, rotation.z);
                direction = bx::mul({0, 0, -1}, rot);
            break;
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    bgfx::reset(width, height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(0, 0, 0, width, height);
    glfwSetWindowSize(window, width, height);
    windowWidth = width;
    windowHeight = height;
}



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

    const aiScene* scene = importer.ReadFile("assets/sphere_lowres.obj", aiProcess_CalcTangentSpace| aiProcess_FlipWindingOrder |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);
    
    struct PosColorVertex
    {
        float x;
        float y;
        float z;
    };

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

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Particl", NULL, NULL);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);


    bgfx::PlatformData pd;
    pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
    pd.ndt = glfwGetX11Display();

    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Vulkan; // Automatically choose a renderer.
    bgfxInit.resolution.width = windowWidth;
    bgfxInit.resolution.height = windowHeight;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfxInit.platformData = pd;
    bgfx::init(bgfxInit);

    std::cout << "Successfully inited bgfx\n";

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, windowWidth, windowHeight);

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

    bgfx::VertexLayout computeVertexLayout;
			computeVertexLayout.begin()
				.add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
			.end();

    bgfx::DynamicVertexBufferHandle positionBuffer = bgfx::createDynamicVertexBuffer(SHADER_DIM*SHADER_DIM*SHADER_DIM*LOCAL_DIM*LOCAL_DIM*LOCAL_DIM, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
    bgfx::DynamicVertexBufferHandle velocityBuffer = bgfx::createDynamicVertexBuffer(SHADER_DIM*SHADER_DIM*SHADER_DIM*LOCAL_DIM*LOCAL_DIM*LOCAL_DIM, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);

    bgfx::ProgramHandle initProgram = bgfx::createProgram(loadShader("spirv/init.cs.bin"), true);
    bgfx::ProgramHandle simulationProgram = bgfx::createProgram(loadShader("spirv/simulate.cs.bin"), true);

    bgfx::setBuffer(0, positionBuffer, bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, velocityBuffer, bgfx::Access::ReadWrite);

    bgfx::UniformHandle uniformHandle = bgfx::createUniform("uniforms", bgfx::UniformType::Vec4, 1);
    float uniform[4] = {SHADER_DIM, 0.0f , 0.0f, counter/100.0f};
    bgfx::setUniform(uniformHandle, uniform, 1);

    bgfx::dispatch(0, initProgram, SHADER_DIM,SHADER_DIM,SHADER_DIM);

    float prevTime = glfwGetTime();

    while(!glfwWindowShouldClose(window)) {   

        float currentTime = glfwGetTime();
        deltaTime = currentTime - prevTime;
        currentTime = prevTime;

        glfwPollEvents();

        bgfx::touch(0);
        float view[16];
        bx::mtxLookAt(view, eye, bx::sub(eye,direction));
        float proj[16];
        bx::mtxProj(proj, 100.0f, float(windowWidth) / float(windowHeight), 0.1f, 1000.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::setUniform(uniformHandle, uniform, 1);
        bgfx::setBuffer(0, positionBuffer, bgfx::Access::ReadWrite);
        bgfx::setBuffer(1, velocityBuffer, bgfx::Access::ReadWrite);

        uniform[0] = SHADER_DIM;
        uniform[1] = DOMAIN_DIM;
        uniform[2] = PARTICLE_SIZE;
        uniform[3] = deltaTime;
        bgfx::setUniform(uniformHandle, uniform, 1);

        bgfx::dispatch(0, simulationProgram, SHADER_DIM, SHADER_DIM, SHADER_DIM);

        bgfx::setInstanceDataBuffer(positionBuffer, 0, SHADER_DIM*SHADER_DIM*SHADER_DIM*LOCAL_DIM*LOCAL_DIM*LOCAL_DIM);

        //bgfx::setState(BGFX_STATE_DEFAULT|BGFX_STATE_PT_POINTS);
        bgfx::submit(0, program);
        bgfx::frame();

        bx::swap(positionBuffer, velocityBuffer);

        counter ++;

    }

    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}