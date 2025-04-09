#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>

#include<iostream>
#include<vector>

#include<string.h>
#include<strings.h>
#include<stdio.h>

#define SHADER_DIM 1
#define LOCAL_DIM 8
#define DOMAIN_DIM 16.0f
#define PARTICLE_SIZE ((DOMAIN_DIM)/(float(SHADER_DIM)*float(LOCAL_DIM)))

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
    struct PosColorVertex
    {
        float x;
        float y;
        float z;
    };

    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Noop; // Automatically choose a renderer.
    bgfx::init(bgfxInit);

    std::cout << "Successfully inited bgfx\n";

    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .end();

    bgfx::VertexLayout computeVertexLayout;
			computeVertexLayout.begin()
				.add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
			.end();

    bgfx::DynamicVertexBufferHandle positionBuffer = bgfx::createDynamicVertexBuffer(SHADER_DIM*SHADER_DIM*SHADER_DIM*LOCAL_DIM*LOCAL_DIM*LOCAL_DIM, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
    bgfx::DynamicVertexBufferHandle prevBuffer = bgfx::createDynamicVertexBuffer(SHADER_DIM*SHADER_DIM*SHADER_DIM*LOCAL_DIM*LOCAL_DIM*LOCAL_DIM, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
    bgfx::DynamicVertexBufferHandle positionBuffer2 = bgfx::createDynamicVertexBuffer(SHADER_DIM*SHADER_DIM*SHADER_DIM*LOCAL_DIM*LOCAL_DIM*LOCAL_DIM, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
    //bgfx::DynamicVertexBufferHandle prevBuffer2 = bgfx::createDynamicVertexBuffer(SHADER_DIM*SHADER_DIM*SHADER_DIM*LOCAL_DIM*LOCAL_DIM*LOCAL_DIM, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);

    bgfx::ProgramHandle initProgram = bgfx::createProgram(loadShader("spirv/init.cs.bin"), true);
    bgfx::ProgramHandle simulationProgram = bgfx::createProgram(loadShader("spirv/simulate.cs.bin"), true);

    bgfx::setBuffer(0, positionBuffer, bgfx::Access::ReadWrite);
    bgfx::setBuffer(1, prevBuffer, bgfx::Access::ReadWrite);
    bgfx::setBuffer(2, positionBuffer2, bgfx::Access::ReadWrite);
    bgfx::setTexture()
    //bgfx::setBuffer(3, prevBuffer2, bgfx::Access::ReadWrite);


    bgfx::UniformHandle uniformHandle = bgfx::createUniform("uniforms", bgfx::UniformType::Vec4, 1);
    float uniform[4] = {SHADER_DIM, 0.0f , 0.0f, 0.0};
    bgfx::setUniform(uniformHandle, uniform, 1);

    bgfx::dispatch(0, initProgram, SHADER_DIM,SHADER_DIM,SHADER_DIM);

    bgfx::shutdown();

    return 0;
}