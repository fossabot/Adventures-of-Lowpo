#include <vector>
#include <iostream>
#include <GL/glew.h>

#include "RenderingSystem.hpp"
#include "../../Components/RenderingComponent.hpp"
#include "../../Components/TransformComponent.hpp"
#include "../../Entity.hpp"
#include "../../External/stb_image.hpp"

RenderingSystem::RenderingSystem(std::vector<std::string> shaders, std::vector<std::string> shadowShaders)
{
    this->primaryBitset =  ComponentType::Rendering | ComponentType::Transform;
    assert(shaders.size() == shadowShaders.size());
    for (int i = 0; i < shaders.size(); i += 2)
    {
        this->shaders.push_back(Shader(shaders[i], shaders[i+1]));
        this->shadowShaders.push_back(Shader(shadowShaders[i], shadowShaders[i+1]));
    }
    this->width = 800;
    this->height = 600;
    this->ambient = 0.3f;
    this->diffuse = 0.5f;
    this->lightDirection = glm::vec3(-48.f,-128.f,0.f);
    // TODO : Make shadows appear
}

RenderingSystem::~RenderingSystem()
{

}

void RenderingSystem::Update(std::vector<std::shared_ptr<Entity>>& entities)
{
    // initial prep
    glViewport(0, 0, this->width, this->height);
    glClearColor(0.529f, 0.807f, 0.980f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // TODO : Camera
    // set projection and view
    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i]->IsEligibleForSystem(this->primaryBitset))
        {
            RenderingComponent renderingComponent = entities[i]->GetComponent<RenderingComponent>(ComponentType::Rendering);
            TransformComponent transformComponent = entities[i]->GetComponent<TransformComponent>(ComponentType::Transform);
            ShaderType shaderType = renderingComponent.shader;
            this->shaders[shaderType].Use();
            this->shaders[shaderType].SetVector3f("light.direction", this->lightDirection);
            this->shaders[shaderType].SetVector3f("light.ambient", ambient, ambient, ambient);
            this->shaders[shaderType].SetVector3f("light.diffuse", diffuse, diffuse, diffuse);
            this->shaders[shaderType].SetMat4("model", transformComponent.GetWorldTransform());

            // texture bind
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderingComponent.textureID);

            // vao bind
            glBindVertexArray(renderingComponent.vertexArrayID);

            // draw
            glDrawArrays(GL_TRIANGLES, 0, renderingComponent.vertexCount);

            // unbind vao
            glBindVertexArray(0);

            // unbind texture
            glBindTexture(GL_TEXTURE_2D, 0);

        }
    }
}

std::pair<unsigned int, unsigned int> RenderingSystem::BufferData(float* data, int size, bool animated)
{
    int floatCount = 8;
    if (animated)
    {
        floatCount = 16;
    }
    unsigned int VAO,VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatCount * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, floatCount * sizeof(float),(void*)(sizeof(float)*3));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, floatCount * sizeof(float), (void*)(sizeof(float)*6));

    if (animated)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, floatCount * sizeof(float), (void*)(sizeof(float)*8));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, floatCount * sizeof(float), (void*)(sizeof(float)*12));
    }
    return std::make_pair(VAO,VBO);
}

unsigned int RenderingSystem::CreateTexture(std::string filename)
{

    if (this->loadedTextures.find(filename) != this->loadedTextures.end())
    {
        return this->loadedTextures[filename];
    }

    int nrChannels;
    unsigned int id;
    int width;
    int height;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // configure
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Cannot load texture : " << filename << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    this->loadedTextures[filename] = id;
}