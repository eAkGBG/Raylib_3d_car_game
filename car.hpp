#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>

class Car{
    public:
        Model carModel; 
        Vector3 position;
        float speed;
        float mass;
        Vector3 angle;
        Vector3 size;
        Ray rayForward;
        Ray rayBackward;
        Ray rayLeft;
        Ray rayRight;
        Ray rayDownF;
        Ray rayDownB;

        Car (Vector3 setSize = {0.5f, 0.4f, 1.0f}, Vector3 setPosition = {0.0f, 0.5f, 0.0f})
            : position (setPosition), size(setSize),
            speed(0.0f),
            mass(2.0f),
            gravityVelocityY(0.0f),
            previousY(0.0f),
            climbing(false),
            angle({0.0f,0.0f,0.0f}),
            rayForward(setPosition, {0.0f, 0.25f, 0.0f}), //a little hack point the rays slightly upp to compensate for elevation changes in the track.
            rayBackward(setPosition, {-1.0f, 0.25f, 0.0f}),
            rayLeft(setPosition, {0.0f, 0.25f, 1.0f}),
            rayRight(setPosition, {0.0f, 0.25f, -1.0f}),
            rayDownF(setPosition, {0.0f,-1.0f,0.0f}),
            rayDownB(setPosition, {0.0f,-1.0f,0.0f})
        {
            //Mesh cubeMesh = GenMeshCube(size.x,size.y,size.z);
            carModel = LoadModel("./models/car.glb");
        }

        void translate(const float& power, const float& dt);
        void rotate(const float& delta);
        void updateRays();
        void updateTransformation();
        void gravity(const RayCollision& cA, const RayCollision& cB, const float& dt);
    private:
        static constexpr float TWO_PI = 2.0f * PI;
        static float maxSpeed;
        static float minSpeed;
        float gravityVelocityY;
        float previousY;   // vertical position from the last frame
        bool climbing; 
};