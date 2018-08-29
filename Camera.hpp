#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>


enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};
class Player;
class Camera
{
    public:
        
        Camera( float       width,
                float       height,
                glm::vec3   position ,
                glm::vec3   direction,
                glm::vec3   worldUp     = glm::vec3(0.0f,1.0f,0.0f),
                GLfloat     yaw         = 0.0f,
                GLfloat     pitch       = 89.0f,
                GLfloat     sensitivity = 0.1f,
                GLfloat     speed       = 2.5f
                );

        glm::mat4   GetViewMatrix();
        glm::mat4   GetProjectionMatrix();
        void        Update(Player* parent);
        
        
        glm::vec3   direction;
        glm::vec3   up;
        glm::vec3   right;
        glm::vec3   worldUp;
        float       speed;
        glm::vec3   position;

    private:
        

        float       yaw;
        float       pitch;
        float       sensitivity;
        glm::mat4   projectionMatrix;
        float       width;
        float       height;
};