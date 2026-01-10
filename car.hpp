#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>

class Car{
    public:
        Model carModel; 
        Vector3 position;
        float speed;
        Vector3 angle;
        Vector3 size;
        Ray rayForward;
        Ray rayBackward;
        Ray rayLeft;
        Ray rayRight;
        Ray rayDown;

        Car (Vector3 setSize = {0.5f, 0.4f, 1.0f}, Vector3 setPosition = {0.0f, 0.5f, 0.0f})
            : position (setPosition), size(setSize),
            speed(0.0f),
            angle({0.0f,0.0f,0.0f}),
            rayForward(setPosition, {0.0f, 0.25f, 0.0f}),
            rayBackward(setPosition, {-1.0f, 0.25f, 0.0f}),
            rayLeft(setPosition, {0.0f, 0.25f, 1.0f}),
            rayRight(setPosition, {0.0f, 0.25f, -1.0f}),
            rayDown(setPosition, {0.0f,-1.0f,0.0f})
        {
            //Mesh cubeMesh = GenMeshCube(size.x,size.y,size.z);
            carModel = LoadModel("./models/car.glb");
        }

        void translate(const float& power, const float& dt);
        void rotate(const float& delta);
        void updateRays();
        void updateTransformation();
        void gravity(const RayCollision& c);
    private:
        static constexpr float TWO_PI = 2.0f * PI;
        static float maxSpeed;
        static float minSpeed;
};